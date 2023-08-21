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
	//me->GetMesh()->BodyInstance.bLockRotation = true;
	//me->GetMesh()->BodyInstance.CreateDOFLock();
	playerController->StopMovement();
	me->GetCharacterMovement()->Deactivate();
	me->AttackCirclePlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//UE_LOG(LogTemp, Warning, TEXT("Animstart"))
}

void UPlayerAnim::AnimNotify_PlayerAttackEnd()
{
	me->GetCharacterMovement()->Activate();
	me->AttackCirclePlane->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//me->GetMesh()->BodyInstance.bLockRotation = false;
	//me->GetMesh()->BodyInstance.SetDOFLock(EDOFMode::None);
	//UE_LOG(LogTemp, Warning, TEXT("Animend"))


}
