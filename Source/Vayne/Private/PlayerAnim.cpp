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
	playerController->DisableInput(playerController);
	playerController->StopMovement();
	//me->GetCharacterMovement()->DisableMovement();
	me->GetCharacterMovement()->MaxWalkSpeed=0;
	UE_LOG(LogTemp, Warning, TEXT("Animstart"))
}

void UPlayerAnim::AnimNotify_PlayerAttackEnd()
{
	me->GetController()->StopMovement();
	//me->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	me->GetCharacterMovement()->MaxWalkSpeed=380.f;
	playerController->EnableInput(playerController);
	UE_LOG(LogTemp, Warning, TEXT("Animend"))


}
