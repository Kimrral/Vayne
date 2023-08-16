// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"

#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


void UEnemyAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	me =  Cast<AEnemy>(TryGetPawnOwner());
	

}


void UEnemyAnim::AnimNotify_AttackStart()
{
	UEnemyFSM* fsm = Cast<UEnemyFSM>(me->GetDefaultSubobjectByName(FName("enemyFSM")));
	if(fsm)
	{
		me->enemyFSM->SetRotToPlayer();
		me->enemyFSM->bIsAttackReady=false;
	}
}

void UEnemyAnim::AnimNotify_AttackEnd()
{
	UEnemyFSM* fsm = Cast<UEnemyFSM>(me->GetDefaultSubobjectByName(FName("enemyFSM")));
	if(fsm)
	{
	}
}

