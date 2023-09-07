// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"

#include "AI/NavigationSystemBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Vayne/VayneCharacter.h"
#include "Vayne/VaynePlayerController.h"

void UPlayerAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	me =  Cast<AVayneCharacter>(TryGetPawnOwner());
	playerController = Cast<AVaynePlayerController>(GetWorld()->GetFirstPlayerController());
}

void UPlayerAnim::AnimNotify_PlayerAttackStart()
{
	playerController->StopMovement();
	me->GetCharacterMovement()->Deactivate();
	me->AttackCirclePlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UPlayerAnim::AnimNotify_PlayerAttackEnd()
{
	playerController->StopMovement();
	me->GetCharacterMovement()->Activate();
	me->AttackCirclePlane->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UPlayerAnim::AnimNotify_DashStart()
{
}

void UPlayerAnim::AnimNotify_DashEnd()
{
	me->GetCharacterMovement()->StopActiveMovement();
	playerController->EnableInput(playerController);
	//playerController->SetIgnoreMoveInput(false);

}
