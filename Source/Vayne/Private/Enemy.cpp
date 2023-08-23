// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "EnemyAnim.h"
#include "EnemyFSM.h"
#include "Components/CapsuleComponent.h"
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

	
	// Character Mesh Setup
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->OnBeginCursorOver.AddDynamic(this, &AEnemy::CursorOver);
	GetMesh()->OnEndCursorOver.AddDynamic(this, &AEnemy::CursorOverEnd);

	// Set Walk Speed
	//GetCharacterMovement()->MaxWalkSpeed=200.f;

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

	enemyAnim = Cast<UEnemyAnim>(GetMesh()->GetAnimInstance());
	gameMode = Cast<AVayneGameMode>(GetWorld()->GetAuthGameMode());
	//playerController=Cast<AVaynePlayerController>(GetWorld()->GetFirstPlayerController());

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
	//playerController->SetMouseCursorWidget(EMouseCursor::Default, enemyCursor);
	gameMode->isCursorOnEnemy=true;
}

void AEnemy::CursorOverEnd(UPrimitiveComponent* primComp)
{
	// Outline Render
	GetMesh()->SetRenderCustomDepth(false);
	//playerController->SetMouseCursorWidget(EMouseCursor::Default, defaultCursor);
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
	//SetActorEnableCollision(false);
	capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().SetTimer(destroyHandle, this, &AEnemy::OnDestroy, 10.0f, false);
}

void AEnemy::OnDamaged()
{
	GetController()->StopMovement();
	GetCharacterMovement()->Deactivate();
	PlayAnimMontage(damageMontage, 1);
}

void AEnemy::OnDestroy()
{
	this->Destroy();
}

