// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleActor.h"

#include "Vayne/VayneGameMode.h"

// Sets default values
AInteractibleActor::AInteractibleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	SetRootComponent(mesh);

	//Mesh Setup
	mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	mesh->OnBeginCursorOver.AddDynamic(this, &AInteractibleActor::CursorOver);
	mesh->OnEndCursorOver.AddDynamic(this, &AInteractibleActor::CursorOverEnd);
}

// Called when the game starts or when spawned
void AInteractibleActor::BeginPlay()
{
	Super::BeginPlay();

	gameMode = Cast<AVayneGameMode>(GetWorld()->GetAuthGameMode());
	
}

void AInteractibleActor::CursorOver(UPrimitiveComponent* primComp)
{
	// Outline Render
	mesh->SetRenderCustomDepth(true);
	gameMode->isCursorOnInteractibleActor=true;
}

void AInteractibleActor::CursorOverEnd(UPrimitiveComponent* primComp)
{
	// Outline Render
	mesh->SetRenderCustomDepth(false);
	gameMode->isCursorOnInteractibleActor=false;
}



// Called every frame
void AInteractibleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

