// Copyright Epic Games, Inc. All Rights Reserved.

#include "VayneCharacter.h"

#include "Enemy.h"
#include "EnemyFSM.h"
#include "VayneGameMode.h"
#include "VaynePlayerController.h"
#include "AI/NavigationSystemBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
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
	GetCharacterMovement()->RotationRate = FRotator(0.f, 700.f, 0.f);
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

	// Attack Circle Particle
	AttackCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackCircle"));
	AttackCircle->SetupAttachment(RootComponent);
	AttackCircle->SetVisibility(false);
	AttackCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Attack Circle Particle
	AttackCirclePlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackCirclePlane"));
	AttackCirclePlane->SetupAttachment(RootComponent);
	AttackCirclePlane->SetVisibility(false);
	AttackCirclePlane->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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

}

void AVayneCharacter::BeginPlay()
{
	Super::BeginPlay();

	gameMode = Cast<AVayneGameMode>(GetWorld()->GetAuthGameMode());
	playerController = Cast<AVaynePlayerController>(GetWorld()->GetFirstPlayerController());

	AttackCirclePlane->OnComponentBeginOverlap.AddDynamic(this, &AVayneCharacter::OnOverlapEnemy);
	AttackCirclePlane->OnComponentEndOverlap.AddDynamic(this, &AVayneCharacter::EndOverlapEnemy);

}

void AVayneCharacter::FireInput()
{
	// if A Pressed
	if(isAPressed)
	{
		// if Cursor is on Enemy
		if(gameMode->isCursorOnEnemy==true)
		{
			bAttackMode = true;
			GetWorldTimerManager().ClearTimer(attackDelayHandle);
			GetWorldTimerManager().ClearTimer(attackModeHandle);
			FHitResult Hit;
			bool bHitSuccessful = playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
			if (bHitSuccessful)
			{
				hitActors = Hit.GetActor();
				if(hitActors)
				{
					AEnemy* enemy = Cast<AEnemy>(hitActors);
					if(enemy)
					{
						CachedEnemyLoc=enemy->GetActorLocation();
						enemyDist = this->GetDistanceTo(enemy);
						// if attackable Range
						if(enemyDist<=attackRange)
						{
							bool isMontagePlaying = GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
							if(!isMontagePlaying)
							{
								StartTargetAttack(enemy);
								FTimerDelegate attackDelegate = FTimerDelegate::CreateUObject( this, &AVayneCharacter::StartTargetAttack, enemy);
								GetWorldTimerManager().SetTimer(attackDelayHandle, attackDelegate, attackDelay, true);
							}
						}
						// if not attackable Range
						else
						{
							bIsNotAttackableRange=true;
							//playerController->StopMovement();
							auto targetLoc = (enemy->GetActorLocation()-this->GetActorLocation()).GetSafeNormal();
							UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(FName("enemyFSM")));
							if(fsm)
							{
								FVector WorldDirection = (enemy->GetActorLocation() - GetActorLocation());
								auto charRot = UKismetMathLibrary::MakeRotFromXZ(WorldDirection, GetActorUpVector());
								SetActorRotation(FRotator(0, charRot.Yaw, 0));	
							}							
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
			//playerController->OnSetDestinationTriggered();
		}
	}
	// if A is not Pressed
	else
	{

	}
}

void AVayneCharacter::FireInputReleased()
{
	if(isAPressed)
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
}

void AVayneCharacter::StartTargetAttack(AEnemy* enemy)
{
	UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(FName("enemyFSM")));
	if(fsm&&fsm->curHP>0)
	{		
		FVector WorldDirection = (CachedEnemyLoc - this->GetActorLocation());
		auto charRot = UKismetMathLibrary::MakeRotFromXZ(WorldDirection, this->GetActorUpVector());
		this->SetActorRotation(FRotator(0, charRot.Yaw, 0));
		FVector startLoc = GetMesh()->GetSocketLocation(FName("SMG_Barrel"));
		FRotator fireRot = GetMesh()->GetSocketRotation("SMG_Barrel");
		auto emitterLoc = enemy->GetMesh()->GetSocketLocation(FName("HitEffectSocket"));
		auto emitterRot = enemy->GetMesh()->GetSocketRotation(FName("HitEffectSocket"));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireFactory,startLoc, fireRot, FVector(1, 1, 1));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletTrailFactory,startLoc, fireRot, FVector(0.5));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletTrailFactory2,startLoc, fireRot, FVector(0.3, 0.1, 0.1));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory,emitterLoc, emitterRot, FVector(2, 2, 2));		
		PlayAnimMontage(FireMontage, 1);
		fsm->OnDamageProcess(20);
		enemy->OnDamaged();
		AttackCircle->SetVisibility(false);
		AttackCirclePlane->SetVisibility(false);
		GetWorldTimerManager().SetTimer(attackModeHandle, this, &AVayneCharacter::OffAttackMode, 3.0, false);
	}
}

void AVayneCharacter::OnOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy* enemy = Cast<AEnemy>(OtherActor);
	if(enemy&&bAttackMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("enemy casted"))
		StartTargetAttack(enemy);
		FTimerDelegate attackDelegate = FTimerDelegate::CreateUObject( this, &AVayneCharacter::StartTargetAttack, enemy);
		GetWorldTimerManager().SetTimer(attackDelayHandle, attackDelegate, attackDelay, true);
	}
}

void AVayneCharacter::EndOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bAttackMode=false;
	AEnemy* enemy = Cast<AEnemy>(OtherActor);
	if(enemy&&bAttackMode)
	{
		GetWorldTimerManager().ClearTimer(attackDelayHandle);
	}
}

