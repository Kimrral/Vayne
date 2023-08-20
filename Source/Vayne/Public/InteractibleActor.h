// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractibleActor.generated.h"

UCLASS()
class VAYNE_API AInteractibleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractibleActor();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* mesh;

	UPROPERTY()
	class AVayneGameMode* gameMode;
	
	// Outline Function
	UFUNCTION()
	void CursorOver(UPrimitiveComponent* primComp);
	UFUNCTION()
	void CursorOverEnd(UPrimitiveComponent* primComp);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
