// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyHPWidget.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class VAYNE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Outline Function
	UFUNCTION()
	void CursorOver(UPrimitiveComponent* primComp);
	UFUNCTION()
	void CursorOverEnd(UPrimitiveComponent* primComp);

	UPROPERTY()
	class AVayneGameMode* gameMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FSM)
	class UEnemyFSM* enemyFSM;

	UPROPERTY()
	class UEnemyAnim* enemyAnim;

	//UPROPERTY()
	//class AVaynePlayerController* playerController;

	//UPROPERTY(EditAnywhere, Category=Cursor)
	//class UUserWidget* enemyCursor;

	//UPROPERTY(EditAnywhere, Category=Cursor)
	//class UUserWidget* defaultCursor;

	UFUNCTION()
	void Move();

	UFUNCTION()
	void OnDie();
	
	UFUNCTION()
	void OnDamaged();

	UFUNCTION()
	void OnDestroy();

	UFUNCTION()
	void SetHPWidgetInvisible();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int curHP;
	
	UPROPERTY(EditAnywhere, Category=EnemySettings)
	int maxHP = 100.0f;

	UPROPERTY(EditAnywhere, Category=EnemySettings)
	UAnimMontage* damageMontage;

	UPROPERTY(EditAnywhere, Category=EnemySettings)
	class UWidgetComponent* HPWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category=EnemySettings)
	class UMaterialInterface* overlayMatRed;

	UPROPERTY()
	class UEnemyHPWidget* enemyHPWidget;

	UPROPERTY()
	FTimerHandle HPWidgetInvisibleHandle;




};
