// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	IDLE,
	MOVE,
	ATTACK,
	DAMAGE,
	DIE,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VAYNE_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void TickIdle();
	UFUNCTION()
	void TickMove();
	UFUNCTION()
	void TickAttack();
	UFUNCTION()
	void TickDamage();
	UFUNCTION()
	void TickDie();
	UFUNCTION()
	void OnDamageProcess(int damageValue);
	UFUNCTION()
	void SetState(EEnemyState next);
	UFUNCTION()
	void SetRotToPlayer();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	EEnemyState state;

	UPROPERTY()
	class AVayneCharacter* player;

	UPROPERTY()
	class AEnemy* me;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float aggressiveRange = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float attackRange = 300.f;
	
	UPROPERTY()
	float curTime;
	UPROPERTY()
	bool bIsAttacking;

	UPROPERTY()
	int curHP;
	UPROPERTY()
	int maxHP = 100.0f;
	UPROPERTY()
	float attackDelayTime = 2.1f;

	bool bTickDie;

	UPROPERTY()
	float alpha;

	UPROPERTY(EditAnywhere)  // Timeline 생성
	FTimeline Timeline;					

	UPROPERTY(EditAnywhere)  // Timeline 커브
	UCurveFloat* CurveFloat;  






	
};
