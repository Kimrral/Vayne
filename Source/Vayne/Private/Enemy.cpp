// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Character Mesh Setup
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->OnBeginCursorOver.AddDynamic(this, &AEnemy::CursorOver);
	GetMesh()->OnEndCursorOver.AddDynamic(this, &AEnemy::CursorOverEnd);
	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
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
	GetMesh()->SetRenderCustomDepth(true);
}

void AEnemy::CursorOverEnd(UPrimitiveComponent* primComp)
{
	GetMesh()->SetRenderCustomDepth(false);
}

