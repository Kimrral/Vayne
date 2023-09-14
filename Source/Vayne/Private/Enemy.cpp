// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "EnemyAnim.h"
#include "EnemyFSM.h"
#include "EnemyHPWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Vayne/VayneGameMode.h"

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
	// Outline Render
	GetMesh()->SetRenderCustomDepth(true);	
	gameMode->isCursorOnEnemy=true;
}

void AEnemy::CursorOverEnd(UPrimitiveComponent* primComp)
{
	// Outline Render
	GetMesh()->SetRenderCustomDepth(false);
	gameMode->isCursorOnEnemy=false;
}

void AEnemy::Move()
{
}



void AEnemy::OnDie()
{
	FTimerHandle destroyHandle;
	GetCharacterMovement()->Deactivate();
	StopAnimMontage();
	auto capsule = GetCapsuleComponent();
	capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().SetTimer(destroyHandle, this, &AEnemy::OnDestroy, 10.0f, false);
}

void AEnemy::OnDamaged()
{
	FTimerHandle overlayMatHandle;
	GetController()->StopMovement();
	GetCharacterMovement()->Deactivate();
	GetMesh()->SetOverlayMaterial(overlayMatRed);
	PlayAnimMontage(damageMontage, 1);
	GetWorldTimerManager().ClearTimer(overlayMatHandle);
	GetWorldTimerManager().SetTimer(overlayMatHandle, FTimerDelegate::CreateLambda([this]()->void
	{
		GetMesh()->SetOverlayMaterial(nullptr);
	}), 0.3f, false);
}

void AEnemy::OnDestroy()
{
	this->Destroy();
}

void AEnemy::SetHPWidgetInvisible()
{	
	GetWorldTimerManager().SetTimer(HPWidgetInvisibleHandle, FTimerDelegate::CreateLambda([this]()->void
	{
		HPWidgetComponent->SetVisibility(false);		
	}), 3.0f, false);
}

