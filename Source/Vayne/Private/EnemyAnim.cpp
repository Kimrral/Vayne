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
	}
}

void UEnemyAnim::AnimNotify_DamageEnd()
{
	me->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}