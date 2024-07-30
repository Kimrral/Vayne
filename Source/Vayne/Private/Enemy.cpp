// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "EnemyAnim.h"
#include "EnemyFSM.h"
#include "EnemyHPWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Vayne/VayneGameMode.h"
#include "Vayne/VaynePlayerController.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Enemy FSM
	enemyFSM=CreateDefaultSubobject<UEnemyFSM>(TEXT("enemyFSM"));

	// HP Widget Component
	HPWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPWidgetComponent"));
	HPWidgetComponent->SetupAttachment(RootComponent);

	
	// Character Mesh Setup
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->OnBeginCursorOver.AddDynamic(this, &AEnemy::CursorOver);
	GetMesh()->OnEndCursorOver.AddDynamic(this, &AEnemy::CursorOverEnd);

	// Enemy Anim Blueprints
	ConstructorHelpers::FClassFinder<UAnimInstance> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/ABP_Guardian.ABP_Guardian_C'"));
	if(tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}

	// Aiming Pointer Setup
	aimingPointer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("aimingPointer"));
	aimingPointer->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Set HP
	curHP=maxHP;

	enemyAnim = Cast<UEnemyAnim>(GetMesh()->GetAnimInstance());
	gameMode = Cast<AVayneGameMode>(GetWorld()->GetAuthGameMode());
	enemyHPWidget = Cast<UEnemyHPWidget>(HPWidgetComponent->GetWidget());
	PC = Cast<AVaynePlayerController>(GetWorld()->GetFirstPlayerController());

	aimingPointer->SetVisibility(false);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
		
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::CursorOver(UPrimitiveComponent* primComp)
{
	if(PC)
	{
		PC->bShowMouseCursor=false;
	}
	// Outline Render
	GetMesh()->SetRenderCustomDepth(true);
	aimingPointer->SetVisibility(true);
	gameMode->isCursorOnEnemy=true;
}

void AEnemy::CursorOverEnd(UPrimitiveComponent* primComp)
{
	if(PC)
	{
		PC->bShowMouseCursor=true;
	}
	// Outline Render
	GetMesh()->SetRenderCustomDepth(false);
	aimingPointer->SetVisibility(false);
	gameMode->isCursorOnEnemy=false;
}

// 적 캐릭터의 머리에 커서가 있을 때 호출되는 함수
void AEnemy::HeadCursorOver(UPrimitiveComponent* primComp)
{
	// 게임 모드에서 커서가 적의 머리에 있는지 여부를 true로 설정
	gameMode->isCursorOnEnemyHead = true;
	// 조준 포인터의 머리용 머티리얼로 변경
	aimingPointer->SetMaterial(0, M_aimingPointerHead);
	UE_LOG(LogTemp, Warning, TEXT("Head on")) // 디버그 로그 출력
}

// 적 캐릭터의 머리에서 커서가 벗어났을 때 호출되는 함수
void AEnemy::HeadCursorOverEnd(UPrimitiveComponent* primComp)
{
	// 게임 모드에서 커서가 적의 머리에 있는지 여부를 false로 설정
	gameMode->isCursorOnEnemyHead = false;
	// 조준 포인터의 기본 머티리얼로 변경
	aimingPointer->SetMaterial(0, M_aimingPointer);
	UE_LOG(LogTemp, Warning, TEXT("Head off")) // 디버그 로그 출력
}

// 적 캐릭터의 이동 함수 (현재 구현되지 않음)
void AEnemy::Move()
{
}

// 적 캐릭터가 죽었을 때 호출되는 함수
void AEnemy::OnDie()
{
	FTimerHandle destroyHandle; // 타이머 핸들
	GetCharacterMovement()->Deactivate(); // 캐릭터의 이동 비활성화
	StopAnimMontage(); // 애니메이션 몽타주 중지
	auto capsule = GetCapsuleComponent(); // 캡슐 컴포넌트 가져오기
	capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 캡슐의 충돌 비활성화
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메시의 충돌 비활성화
	// 10초 후에 OnDestroy 함수를 호출하도록 타이머 설정
	GetWorldTimerManager().SetTimer(destroyHandle, this, &AEnemy::OnDestroy, 10.0f, false);
}

// 적 캐릭터가 피해를 입었을 때 호출되는 함수
void AEnemy::OnDamaged()
{
	FTimerHandle overlayMatHandle; // 타이머 핸들
	GetMesh()->SetOverlayMaterial(overlayMatRed); // 메시의 오버레이 머티리얼을 빨간색으로 설정
	// 이전 타이머 제거
	GetWorldTimerManager().ClearTimer(overlayMatHandle);
	// 0.3초 후에 오버레이 머티리얼을 제거하도록 타이머 설정
	GetWorldTimerManager().SetTimer(overlayMatHandle, FTimerDelegate::CreateLambda([this]()->void
	{
		GetMesh()->SetOverlayMaterial(nullptr);
	}), 0.3f, false);
}

// 적 캐릭터의 머리가 피해를 입었을 때 호출되는 함수
void AEnemy::OnHeadDamaged()
{
	FTimerHandle overlayMatHandle; // 타이머 핸들
	GetController()->StopMovement(); // 컨트롤러의 이동 중지
	GetCharacterMovement()->Deactivate(); // 캐릭터의 이동 비활성화
	GetMesh()->SetOverlayMaterial(overlayMatRed); // 메시의 오버레이 머티리얼을 빨간색으로 설정
	PlayAnimMontage(damageMontage, 1); // 피해 애니메이션 재생
	// 이전 타이머 제거
	GetWorldTimerManager().ClearTimer(overlayMatHandle);
	// 0.3초 후에 오버레이 머티리얼을 제거하도록 타이머 설정
	GetWorldTimerManager().SetTimer(overlayMatHandle, FTimerDelegate::CreateLambda([this]()->void
	{
		GetMesh()->SetOverlayMaterial(nullptr);
	}), 0.3f, false);
}

// 적 캐릭터를 파괴하는 함수
void AEnemy::OnDestroy()
{
	this->Destroy(); // 캐릭터 파괴
}

// HP 위젯을 숨기는 함수
void AEnemy::SetHPWidgetInvisible()
{
	// 3초 후에 HP 위젯을 숨기도록 타이머 설정
	GetWorldTimerManager().SetTimer(HPWidgetInvisibleHandle, FTimerDelegate::CreateLambda([this]()->void
	{
		HPWidgetComponent->SetVisibility(false);
	}), 3.0f, false);
}
