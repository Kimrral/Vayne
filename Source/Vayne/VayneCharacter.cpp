// Copyright Epic Games, Inc. All Rights Reserved.

#include "VayneCharacter.h"

#include "Enemy.h"
#include "EnemyFSM.h"
#include "PlayerSpaceWidget.h"
#include "VayneGameMode.h"
#include "VaynePlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AVayneCharacter::AVayneCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 600.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	TopDownCameraComponent->FieldOfView=65.0f;

	// Attack Circle Particle
	AttackCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackCircle"));
	AttackCircle->SetupAttachment(RootComponent);
	AttackCircle->SetVisibility(false);
	AttackCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Attack Circle Particle
	AttackCirclePlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackCirclePlane"));
	AttackCirclePlane->SetupAttachment(RootComponent);
	AttackCirclePlane->SetVisibility(false);
	AttackCirclePlane->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackCirclePlane->SetGenerateOverlapEvents(true);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Character Mesh Setup
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AVayneCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    // Timeline Tick Setting
	RollTimeline.TickTimeline(DeltaSeconds);
	// 마우스 커서가 적 위에 위치해 있다면
	if(gameMode&&gameMode->isCursorOnEnemy)
	{
		FHitResult Hit;
		// 커서 충돌 결과값 도출
		bool bHitSuccessful = playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
		if (bHitSuccessful)
		{
			hitActors = Hit.GetActor();
			auto hitBone = Hit.BoneName;
			if(hitActors)
			{
				// Enemy Casting
				enemyRef = Cast<AEnemy>(hitActors);
				if(enemyRef)
				{
					bHead=false;
					// Aiming Point Vector값 도출 및 전역변수에 캐싱
					aimingPointLoc = Hit.Location;
					// Aiming Point Location 설정
					enemyRef->aimingPointer->SetWorldLocation(aimingPointLoc);
				}
			}
		}
	}
}

void AVayneCharacter::BeginPlay()
{
	Super::BeginPlay();

	gameMode = Cast<AVayneGameMode>(GetWorld()->GetAuthGameMode());
	playerController = Cast<AVaynePlayerController>(GetWorld()->GetFirstPlayerController());

	AttackCirclePlane->OnComponentBeginOverlap.AddDynamic(this, &AVayneCharacter::OnOverlapEnemy);
	AttackCirclePlane->OnComponentEndOverlap.AddDynamic(this, &AVayneCharacter::EndOverlapEnemy);

	// Timeline Binding
	if (RollingCurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &AVayneCharacter::RollingTimeline);
		RollTimeline.AddInterpFloat(RollingCurveFloat, TimelineProgress);
		RollTimeline.SetTimelineLength(0.76f);
	}

	spaceUI=CreateWidget<UPlayerSpaceWidget>(GetWorld(), spaceWidget);

}

void AVayneCharacter::FireInput()
{
	// 마우스 커서가 적 위에 놓여있다면
	if(gameMode&&gameMode->isCursorOnEnemy==true)
	{
			bAttackMode = true;
			GetWorldTimerManager().ClearTimer(attackDelayHandle);
			GetWorldTimerManager().ClearTimer(attackModeHandle);
			FHitResult Hit;
			// 커서 충돌 결과값 도출
			bool bHitSuccessful = playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
			if (bHitSuccessful)
			{
			hitActors = Hit.GetActor();
			if(hitActors)
			{
				enemyRef = Cast<AEnemy>(hitActors);
				if(enemyRef&&enemyRef->curHP>0)
				{
					CachedEnemyLoc=enemyRef->GetActorLocation();
					enemyDist = this->GetDistanceTo(enemyRef);
					// 캐스팅한 적이 공격 가능한 범위에 있다면
					if(enemyDist<=attackRange)
					{
						// 다른 몽타주가 재생되고 있지 않다면
						bool isMontagePlaying = GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
						if(!isMontagePlaying)
						{
							// 타겟 공격 프로세스 수행
							StartTargetAttack(enemyRef);
							// 점사 공격 타이머 설정
							FTimerDelegate attackDelegate = FTimerDelegate::CreateUObject( this, &AVayneCharacter::StartTargetAttack, enemyRef);
							GetWorldTimerManager().SetTimer(attackDelayHandle, attackDelegate, attackDelay, true);
						}
					}
					// 캐스팅한 적이 공격 가능 범위 내에 있지 않다면
					else
					{
						bIsNotAttackableRange=true;
						auto targetLoc = (enemyRef->GetActorLocation()-this->GetActorLocation()).GetSafeNormal();
						// Enemy 상태머신 캐스팅
						UEnemyFSM* fsm = Cast<UEnemyFSM>(enemyRef->GetDefaultSubobjectByName(FName("enemyFSM")));
						if(fsm)
						{
							// 적 위치를 기반으로 캐릭터 회전 Yaw값 설정
							FVector WorldDirection = (enemyRef->GetActorLocation() - GetActorLocation());
							auto charRot = UKismetMathLibrary::MakeRotFromXZ(WorldDirection, GetActorUpVector());
							SetActorRotation(FRotator(0, charRot.Yaw, 0));	
						}
						// 캐치된 적 위치로 이동
						AddMovementInput(targetLoc, 1, false);
						UAIBlueprintHelperLibrary::SimpleMoveToLocation(playerController, CachedEnemyLoc);
					}
				}
			}
		}
	}
	// if Cursor is not on Enemy
	else
	{
		
	}
}

void AVayneCharacter::FireInputReleased()
{
	if(gameMode->isCursorOnEnemy)
	{
		if(bIsNotAttackableRange)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(playerController, CachedEnemyLoc);
		}
	}
	else
	{
		playerController->OnSetDestinationReleased();
	}
}

void AVayneCharacter::SpaceInput()
{
	// 1회 실행을 위한 불리언
	if(bIsRollingAvailable)
	{
		bIsRollingAvailable=false;
		StopAnimMontage();
		if(spaceUI)
		{
			spaceUI->AddToViewport();
		}
		playerController->DisableInput(playerController);
		//playerController->SetIgnoreMoveInput(true);
		playerController->Timeline.Stop();
		RollTimeline.Stop();
		AttackCirclePlane->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetCharacterMovement()->StopActiveMovement();
		GetCharacterMovement()->Activate();
		GetWorldTimerManager().ClearTimer(attackDelayHandle);
		GetWorldTimerManager().ClearTimer(burstHandle);
		GetWorldTimerManager().ClearTimer(burstHandle2nd);
		FTimerHandle spaceCooltimeHandle;
		FHitResult Hit;
		// 마우스 방향값 산출을 위한 HitResult 수행
		bool bHitSuccessful = playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, Hit);
		if (bHitSuccessful)
		{
			// 마우스 방향값 캐싱
			dashCursorLoc=Hit.Location;
		}
		FVector WorldDirection = (dashCursorLoc - this->GetActorLocation());
		auto fireRot = UKismetMathLibrary::MakeRotFromXZ(WorldDirection, this->GetActorUpVector());
		this->SetActorRotation(FRotator(0, fireRot.Yaw, 0));
		RollTimeline.PlayFromStart();
		PlayAnimMontage(DashMontage, 1);
		spaceUI->TimerText->SetText(FText::AsNumber(rollingCooltimeText));
		GetWorldTimerManager().SetTimer(cooldownTextHandle, this, &AVayneCharacter::SetWidgetCooldownText, 1, true);
		GetWorldTimerManager().SetTimer(spaceCooltimeHandle, this, &AVayneCharacter::RollingEnable, rollingCooltime, false);		
	}	
}

void AVayneCharacter::CursorOver(UPrimitiveComponent* primComp)
{
	//GetMesh()->SetRenderCustomDepth(true);
}

void AVayneCharacter::CursorOverEnd(UPrimitiveComponent* primComp)
{
	//GetMesh()->SetRenderCustomDepth(false);
}

void AVayneCharacter::OffAttackMode()
{
	bAttackMode=false;
}

// Targeting Attack Begin
void AVayneCharacter::StartTargetAttack(AEnemy* enemy)
{
	// Enemy FSM Casting
	UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(FName("enemyFSM")));
	if(fsm&&enemy)
	{
		// Enemy HP가 0이 아니면서 커서가 Enemy에 위치해 있다면
		if(enemy->curHP>0&&gameMode->isCursorOnEnemy)
		{
			// 전역변수에 캐싱한 Location값 기준으로 Target - Me
			FVector WorldDirection = (aimingPointLoc - this->GetActorLocation());
			// MakeRotFromXZ를 통한 Rotator값 생성
			FRotator charRot = UKismetMathLibrary::MakeRotFromXZ(WorldDirection, this->GetActorUpVector());
			// Attack Begin 시 적을 바라보도록 Yaw Rotator 설정
			this->SetActorRotation(FRotator(0, charRot.Yaw, 0));
			// Weapon Socket Location
			FVector startLoc = GetMesh()->GetSocketLocation(FName("SMG_Barrel"));
			// Weapon Socket Rotation
			FRotator fireRot = GetMesh()->GetSocketRotation("SMG_Barrel");
			// Hit Effect Socket Rotation
			FRotator emitterRot = enemy->GetMesh()->GetSocketRotation(FName("HitEffectSocket"));
			// Emitter Spawn
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireFactory,startLoc, fireRot, FVector(1, 1, 1));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletTrailFactory2,startLoc, charRot, FVector(0.2, 0.1, 0.1));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory,aimingPointLoc, emitterRot, FVector(2, 2, 2));
			// Fire Animation Montage 재생
			PlayAnimMontage(FireMontage, 1);
			FHitResult Hit;
			// 커서 충돌 결과값 도출
			bool bHitSuccessful = playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
			if (bHitSuccessful)
			{
				hitActors = Hit.GetActor();
				auto hitBone = Hit.BoneName;
				if(hitBone==FName("head"))
				{
					// FSM에 있는 Damage Process 호출		
					fsm->OnDamageProcess(20);
					// 헤드 적중 데미지 프로세스 호출
					enemy->OnHeadDamaged();
				}
				else
				{
					// FSM에 있는 Damage Process 호출		
					fsm->OnDamageProcess(10);
					// 일반 적중 데미지 프로세스 호출
					enemy->OnDamaged();
				}
			}					
			AttackCircle->SetVisibility(false);
			AttackCirclePlane->SetVisibility(false);
			// 두 번째 점사 공격 Timer Delegate 생성, 예약
			FTimerDelegate secondAttackDelegate = FTimerDelegate::CreateUObject( this, &AVayneCharacter::SecondTargetAttack, enemy);
			GetWorldTimerManager().SetTimer(burstHandle, secondAttackDelegate, 0.07f, false);
			// 세 번째 점사 공격 Timer Delegate 생성, 예약
			FTimerDelegate thirdAttackDelegate = FTimerDelegate::CreateUObject( this, &AVayneCharacter::ThirdTargetAttack, enemy);
			GetWorldTimerManager().SetTimer(burstHandle2nd, thirdAttackDelegate, 0.14f, false);
			// Disable Attack Mode With Timer
			GetWorldTimerManager().SetTimer(attackModeHandle, this, &AVayneCharacter::OffAttackMode, 3.0, false);
		}
		else
		{
			AttackCirclePlane->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			GetCharacterMovement()->StopActiveMovement();
			GetCharacterMovement()->Activate();
		}
	}
}

void AVayneCharacter::SecondTargetAttack(AEnemy* enemy)
{
	StopAnimMontage(FireMontage);
	UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(FName("enemyFSM")));
	if(fsm&&enemy)
	{
		if(enemy->curHP>0&&gameMode->isCursorOnEnemy)
		{
			FVector WorldDirection = (aimingPointLoc - this->GetActorLocation());
			auto charRot = UKismetMathLibrary::MakeRotFromXZ(WorldDirection, this->GetActorUpVector());
			this->SetActorRotation(FRotator(0, charRot.Yaw, 0));
			FVector startLoc = GetMesh()->GetSocketLocation(FName("SMG_Barrel"));
			FVector backFlashLoc = startLoc+GetActorForwardVector()*300.0f;
			FRotator fireRot = GetMesh()->GetSocketRotation("SMG_Barrel");
			float randX = FMath::FRandRange(0.f, 20.f);
			float randY = FMath::FRandRange(0.f, 4.f);
			float randZ = FMath::FRandRange(0.f, 4.f);
			FRotator trailRot = GetActorRotation();//+FRotator(randX, randY, randZ);
			auto emitterLoc = enemy->GetMesh()->GetSocketLocation(FName("HitEffectSocket"));
			auto emitterRot = enemy->GetMesh()->GetSocketRotation(FName("HitEffectSocket"));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireFactory,startLoc, fireRot, FVector(1, 1, 1));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletTrailFactory2,startLoc, charRot, FVector(0.2, 0.1, 0.1));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory,aimingPointLoc, emitterRot, FVector(2, 2, 2));		
			PlayAnimMontage(FireMontage, 1);
			FHitResult Hit;
			// 커서 충돌 결과값 도출
			bool bHitSuccessful = playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
			if (bHitSuccessful)
			{
				hitActors = Hit.GetActor();
				auto hitBone = Hit.BoneName;
				// 헤드에 적중했다면
				if(hitBone==FName("head"))
				{
					// FSM에 있는 Damage Process 호출		
					fsm->OnDamageProcess(20);
					// 헤드 적중 데미지 프로세스 호출
					enemy->OnHeadDamaged();
					UE_LOG(LogTemp, Warning, TEXT("headshot"))
				}
				else
				{
					// FSM에 있는 Damage Process 호출		
					fsm->OnDamageProcess(10);
					// 일반 적중 데미지 프로세스 호출
					enemy->OnDamaged();
				}
			}		
		}
		else
		{
			AttackCirclePlane->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			GetCharacterMovement()->StopActiveMovement();
			GetCharacterMovement()->Activate();
		}
	}
}

void AVayneCharacter::ThirdTargetAttack(AEnemy* enemy)
{
	StopAnimMontage(FireMontage);
	UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(FName("enemyFSM")));
		if(fsm&&enemy)
		{
			if(enemy->curHP>0&&gameMode->isCursorOnEnemy)
			{
				FVector WorldDirection = (aimingPointLoc - this->GetActorLocation());
				auto charRot = UKismetMathLibrary::MakeRotFromXZ(WorldDirection, this->GetActorUpVector());
				this->SetActorRotation(FRotator(0, charRot.Yaw, 0));
				FVector startLoc = GetMesh()->GetSocketLocation(FName("SMG_Barrel"));
				FVector backFlashLoc = startLoc+GetActorForwardVector()*300.0f;
				FRotator fireRot = GetMesh()->GetSocketRotation("SMG_Barrel");
				float randX = FMath::FRandRange(0.f, 20.f);
				float randY = FMath::FRandRange(0.f, 4.f);
				float randZ = FMath::FRandRange(0.f, 4.f);
				FRotator trailRot = GetActorRotation();//+FRotator(randX, randY, randZ);
				auto emitterLoc = enemy->GetMesh()->GetSocketLocation(FName("HitEffectSocket"));
				auto emitterRot = enemy->GetMesh()->GetSocketRotation(FName("HitEffectSocket"));
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireFactory,startLoc, fireRot, FVector(1, 1, 1));
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletTrailFactory2,startLoc, charRot, FVector(0.2, 0.1, 0.1));
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory,aimingPointLoc, emitterRot, FVector(2, 2, 2));		
				PlayAnimMontage(FireMontage, 1);
				FHitResult Hit;
				// 커서 충돌 결과값 도출
				bool bHitSuccessful = playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
				if (bHitSuccessful)
				{
					hitActors = Hit.GetActor();
					auto hitBone = Hit.BoneName;
					// 헤드에 적중했다면
					if(hitBone==FName("head"))
					{
						// FSM에 있는 Damage Process 호출		
						fsm->OnDamageProcess(20);
						// 헤드 적중 데미지 프로세스 호출
						enemy->OnHeadDamaged();
					}
					else
					{
						// FSM에 있는 Damage Process 호출		
						fsm->OnDamageProcess(10);
						// 일반 적중 데미지 프로세스 호출
						enemy->OnDamaged();
					}
				}			
			}
			else
			{
				AttackCirclePlane->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				GetCharacterMovement()->StopActiveMovement();
				GetCharacterMovement()->Activate();
			}
		}
}

void AVayneCharacter::OnOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		AEnemy* hpWidgetEnemy = Cast<AEnemy>(OtherActor);
		if(hpWidgetEnemy)
		{
			GetWorldTimerManager().ClearTimer(hpWidgetEnemy->HPWidgetInvisibleHandle);
			hpWidgetEnemy->enemyHPWidget->HPdynamicMat->SetScalarParameterValue(FName("HPAlpha"), hpWidgetEnemy->curHP*0.01-0.001);
			hpWidgetEnemy->HPWidgetComponent->SetVisibility(true);
		}
	}
	TArray<FHitResult> hits;
	if(SweepResult.GetNumOverlapHits(hits)>1)
	{
		FVector Center = AttackCirclePlane->GetComponentLocation();
		int32 Closest = 0;
		for(int i=0; i<hits.Num(); ++i)
		{
			float ClosestDist = FVector::Dist(hits[Closest].GetActor()->GetActorLocation(), Center);
			float NextDist = FVector::Dist(hits[i].GetActor()->GetActorLocation(), Center);

			if (NextDist < ClosestDist)
			{
				Closest = i;
			}
		}
		OtherActor=hits[Closest].GetActor();
	}
	bool isMontagePlaying = GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
	AEnemy* enemyOverlapped = Cast<AEnemy>(OtherActor);
	if(enemyOverlapped&&bAttackMode&&!isMontagePlaying&&enemyOverlapped==enemyRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("enemy casted"))
		StartTargetAttack(enemyOverlapped);
		FTimerDelegate attackDelegate = FTimerDelegate::CreateUObject( this, &AVayneCharacter::StartTargetAttack, enemyOverlapped);
		GetWorldTimerManager().SetTimer(attackDelayHandle, attackDelegate, attackDelay, true);
	}
}

void AVayneCharacter::EndOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		AEnemy* hpWidgetEnemy = Cast<AEnemy>(OtherActor);
		if(hpWidgetEnemy)
		{
			hpWidgetEnemy->SetHPWidgetInvisible();
		}
	}
	bAttackMode=false;
	AEnemy* enemy = Cast<AEnemy>(OtherActor);
	if(enemy&&bAttackMode)
	{
		GetWorldTimerManager().ClearTimer(attackDelayHandle);
	}
}

void AVayneCharacter::SetWidgetCooldownText()
{
	rollingCooltimeText-=1;
	spaceUI->TimerText->SetText(FText::AsNumber(rollingCooltimeText));
}

void AVayneCharacter::RollingTimeline(float Value)
{
	FVector newVelocity = (GetActorForwardVector()*750.0f)*Value;
	GetCharacterMovement()->Velocity=newVelocity;
}

void AVayneCharacter::RollingEnable()
{
	bIsRollingAvailable=true;
	GetWorldTimerManager().ClearTimer(cooldownTextHandle);
	rollingCooltimeText=5.f;
	spaceUI->UnHovered();
	spaceUI->RemoveFromParent();
}
