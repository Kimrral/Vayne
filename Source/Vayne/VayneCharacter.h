// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VayneCharacter.generated.h"

UCLASS(Blueprintable)
class AVayneCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AVayneCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }


	// Attack Circle Particle
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* AttackCircle;

	// Fire Function
	UFUNCTION()
	void FireInput();

	// Outline Function
	UFUNCTION()
	void CursorOver(UPrimitiveComponent* primComp);
	UFUNCTION()
	void CursorOverEnd(UPrimitiveComponent* primComp);

	UPROPERTY()
	class AActor* hitActors;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, Category=Montage)
	class UAnimMontage* FireMontage;
	


};

