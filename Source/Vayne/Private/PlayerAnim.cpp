// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Vayne/VayneCharacter.h"

void UPlayerAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	me =  Cast<AVayneCharacter>(TryGetPawnOwner());
}

void UPlayerAnim::AnimNotify_PlayerAttackStart()
{
	me->GetCharacterMovement()->MaxWalkSpeed=0;
}

void UPlayerAnim::AnimNotify_PlayerAttackEnd()
{
	me->GetCharacterMovement()->MaxWalkSpeed=380.f;

}
