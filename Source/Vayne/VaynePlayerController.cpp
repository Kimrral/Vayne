// Copyright Epic Games, Inc. All Rights Reserved.

#include "VaynePlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "VayneCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FrameTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AVaynePlayerController::AVaynePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
	bEnableMouseOverEvents=true;
}

void AVaynePlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Player Casting
	PlayerChar=Cast<AVayneCharacter>(GetPawn());

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Timeline Binding
	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &AVaynePlayerController::SetPlayerRot);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}
	
}

void AVaynePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Timeline.TickTimeline(DeltaSeconds);
}

void AVaynePlayerController::SetPlayerRot(float Value)
{
	FVector WorldDirection = (CachedDestination - PlayerChar->GetActorLocation());
	auto fireRot = UKismetMathLibrary::MakeRotFromXZ(WorldDirection, PlayerChar->GetActorUpVector());
	auto startRot = PlayerChar->GetActorRotation();
	auto endRot = fireRot;
	auto lerp = UKismetMathLibrary::RLerp(startRot, endRot, Value, true);
	PlayerChar->SetActorRotation(FRotator(0, lerp.Yaw, 0));
}

void AVaynePlayerController::SetPlayerRotNoLerp()
{
	FVector WorldDirection = (CachedDestination - PlayerChar->GetActorLocation());
	auto fireRot = UKismetMathLibrary::MakeRotFromXZ(WorldDirection, PlayerChar->GetActorUpVector());
	PlayerChar->SetActorRotation(FRotator(0, fireRot.Yaw, 0));
}

void AVaynePlayerController::SetPlayerRotToEnemy(FVector enemyLoc)
{
	FVector WorldDirection = (enemyLoc - PlayerChar->GetActorLocation());
	auto fireRot = UKismetMathLibrary::MakeRotFromXZ(WorldDirection, PlayerChar->GetActorUpVector());
	PlayerChar->SetActorRotation(FRotator(0, fireRot.Yaw, 0));
}

void AVaynePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AVaynePlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AVaynePlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AVaynePlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AVaynePlayerController::OnSetDestinationReleased);

		// Attack input events
		EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &AVaynePlayerController::OnAttack);
		EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Completed, this, &AVaynePlayerController::OnAttackReleased);

		// Stop input events
		EnhancedInputComponent->BindAction(IA_Stop, ETriggerEvent::Triggered, this, &AVaynePlayerController::OnStop);

		// Fire input events
		EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Triggered, this, &AVaynePlayerController::OnFire);

		// Cam Zoom input events
		EnhancedInputComponent->BindAction(IA_CamZoom, ETriggerEvent::Triggered, this, &AVaynePlayerController::OnCamZoom);
		EnhancedInputComponent->BindAction(IA_CamZoomOut, ETriggerEvent::Triggered, this, &AVaynePlayerController::OnCamZoomOut);

		// Space input events
		EnhancedInputComponent->BindAction(IA_Space, ETriggerEvent::Triggered, this, &AVaynePlayerController::OnSpace);

		
		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AVaynePlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AVaynePlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AVaynePlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AVaynePlayerController::OnTouchReleased);
	}
}

void AVaynePlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void AVaynePlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		Timeline.PlayFromStart();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AVaynePlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		Timeline.PlayFromStart();
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void AVaynePlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AVaynePlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void AVaynePlayerController::OnAttack()
{
	if(PlayerChar)
	PlayerChar->AttackCircle->SetVisibility(true);
}

void AVaynePlayerController::OnAttackReleased()
{
	if(PlayerChar)
	PlayerChar->AttackCircle->SetVisibility(false);
}

void AVaynePlayerController::OnStop()
{
	StopMovement();
}

void AVaynePlayerController::OnFire()
{
	FHitResult Hit;
	bool bHitSuccessful = false;
	bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, Hit);
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
		if(PlayerChar)
		{
			PlayerChar->GetCharacterMovement()->DisableMovement();
			StopMovement();
			bool isMontagePlaying = PlayerChar->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
			if(!isMontagePlaying)
			{
				PlayerChar->FireInput();
			}
			FTimerHandle movementHandle;
			GetWorldTimerManager().SetTimer(movementHandle, this, &AVaynePlayerController::MovementReenable, 0.2f, false);			
		}
	}
}

void AVaynePlayerController::OnCamZoom()
{
	PlayerChar->CameraBoom->TargetArmLength=FMath::Clamp(PlayerChar->CameraBoom->TargetArmLength-=100, 600, 2500);
}

void AVaynePlayerController::OnCamZoomOut()
{
	PlayerChar->CameraBoom->TargetArmLength=FMath::Clamp(PlayerChar->CameraBoom->TargetArmLength+=100, 600, 2500);
}

void AVaynePlayerController::OnSpace()
{
}

void AVaynePlayerController::MovementReenable()
{
	if(PlayerChar)
	PlayerChar->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}