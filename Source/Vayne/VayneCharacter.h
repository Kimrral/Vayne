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

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* AttackCirclePlane;

	// Fire Function
	UFUNCTION()
	void FireInput();
	UFUNCTION()
	void FireInputReleased();

	// Space Bar Function
	UFUNCTION()
	void SpaceInput();

	// Outline Function
	UFUNCTION()
	void CursorOver(UPrimitiveComponent* primComp);
	UFUNCTION()
	void CursorOverEnd(UPrimitiveComponent* primComp);

	UFUNCTION()
	void OffAttackMode();

	UFUNCTION()
	void StartTargetAttack(AEnemy* enemy);

	UFUNCTION()
	void OnOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void EndOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	class AActor* hitActors;

	UPROPERTY()
	class AVayneGameMode* gameMode;

	UPROPERTY()
	class AVaynePlayerController* playerController;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, Category=Montage)
	class UAnimMontage* FireMontage;
	
	UPROPERTY(EditAnywhere)
	class UParticleSystem* bulletImpactFactory;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* fireFactory;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* targetMarkFactory;
	
	UPROPERTY(EditAnywhere)
	class UParticleSystem* bulletTrailFactory;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* bulletTrailFactory2;

	UPROPERTY()
	FTransform bulletImpactTrans;

	UPROPERTY()
	int32 enemyArrayNum;

	UPROPERTY()
	FTimerHandle attackModeHandle;

	UPROPERTY(EditAnywhere, Category=Settings)
	float attackRange = 550.0f;

	UPROPERTY()
	float isMovingToAttackRange=false;

	UPROPERTY(EditAnywhere, Category=Settings)
	float attackDelay = 0.6f;

	UPROPERTY()
	FTimerHandle attackDelayHandle;

	UPROPERTY()
	float attackTimerSeconds = 0;

	UPROPERTY()
	FVector CachedEnemyLoc;

	UPROPERTY()
	float enemyDist;

	UPROPERTY()
	bool bIsNotAttackableRange;

	UPROPERTY()
	bool isAPressed;

	UPROPERTY()
	bool bAttackMode = false;

	


};

