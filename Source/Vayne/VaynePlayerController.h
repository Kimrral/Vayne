// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "Components/TimelineComponent.h"
#include "VaynePlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;

UCLASS()
class AVaynePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AVaynePlayerController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursorRed;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* SetDestinationClickAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* SetDestinationTouchAction;

	// Attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* IA_Attack;

	// Stop
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* IA_Stop;

	// Fire
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* IA_Fire;

	// Cam Zoom
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* IA_CamZoom;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* IA_CamZoomOut;

	// Space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* IA_Space;

	// Tab
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* IA_Tab;

	// Player Character Casting
	UPROPERTY()
	class AVayneCharacter* PlayerChar;

	UFUNCTION()
	void MovementReenable();

	UPROPERTY()
	float alpha;

	UPROPERTY(EditAnywhere)  // Timeline 생성
	FTimeline Timeline;					

	UPROPERTY(EditAnywhere)  // Timeline 커브
	UCurveFloat* CurveFloat;  

	UFUNCTION()  // Bind function
	void SetPlayerRot(float Value);

	UFUNCTION()
	void SetPlayerRotNoLerp();

	UPROPERTY()
	class AVayneGameMode* gameMode;

	UPROPERTY()
	FTimerHandle attackEnableHandle;
	
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();
	void OnAttack();
	void OnAttackReleased();
	void OnStop();
	void OnFire();
	void OnTab();
	void OnTabReleased();
	void OnFireReleased();
	void OnCamZoom();
	void OnCamZoomOut();
	void OnSpace();
	void SetAttackModeDisable();
	
	// 회전처리 함수
	void Turn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AxisValue;

private:

	FVector CachedDestination;
	FRotator CachedRotation;

	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed
};


