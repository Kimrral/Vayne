// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFSM.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnim.generated.h"

/**
 * 
 */
UCLASS()
class VAYNE_API UEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemyState state;

	virtual void NativeBeginPlay() override;

	UFUNCTION()
	void AnimNotify_AttackStart();

	UFUNCTION()
	void AnimNotify_AttackEnd();

	UFUNCTION()
	void AnimNotify_DamageEnd();
	
	// Casting 변수를 전역변수로 선언한다.
	UPROPERTY()
	class AEnemy* me;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAttacking;
	
};
