// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class VAYNE_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	
	UPROPERTY()
	class AVayneCharacter* me;

	UPROPERTY()
	class AVaynePlayerController* playerController;
	
	UFUNCTION()
	void AnimNotify_PlayerAttackStart();

	UFUNCTION()
	void AnimNotify_PlayerAttackEnd();

	UFUNCTION()
	void AnimNotify_DashStart();
	
	UFUNCTION()
	void AnimNotify_DashEnd();
	
};
