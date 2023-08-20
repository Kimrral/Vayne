// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VayneGameMode.generated.h"

UCLASS(minimalapi)
class AVayneGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVayneGameMode();

	UPROPERTY()
	bool isCursorOnEnemy = false;

	UPROPERTY()
	bool isCursorOnInteractibleActor = false;
};



