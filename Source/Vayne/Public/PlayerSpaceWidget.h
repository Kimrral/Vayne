// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerSpaceWidget.generated.h"

/**
 * 
 */
UCLASS()
class VAYNE_API UPlayerSpaceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* TimerText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* RollingIconImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* HoverButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* SkillInfo;
	
	UPROPERTY()
	class AVaynePlayerController* controller;
	
	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void UnHovered();
	UFUNCTION()
	void SetSkillInfoLoc();
};
