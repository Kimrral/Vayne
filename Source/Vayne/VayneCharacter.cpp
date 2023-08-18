// Copyright Epic Games, Inc. All Rights Reserved.

#include "VayneCharacter.h"

#include "Enemy.h"
#include "EnemyFSM.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AVayneCharacter::AVayneCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 700.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Attack Circle Particle
	AttackCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackCircle"));
	AttackCircle->SetupAttachment(RootComponent);
	AttackCircle->SetVisibility(false);
	AttackCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Character Mesh Setup
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AVayneCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AVayneCharacter::BeginPlay()
{
	Super::BeginPlay();
	

}

void AVayneCharacter::FireInput()
{
	PlayAnimMontage(FireMontage, 1);
	bool isTargetted = AttackCircle->IsVisible();
	if(isTargetted)
	{
		FVector startLoc = GetMesh()->GetSocketLocation(FName("SMG_Barrel"));
		FVector endLoc = startLoc + GetActorForwardVector()*500.0f;
		FRotator fireRot = GetMesh()->GetSocketRotation("SMG_Barrel");
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), targetFireFactory,startLoc, fireRot, FVector(1, 1, 1));
	}
	else
	{
		TArray<FHitResult> hits;
		FVector startLoc = GetMesh()->GetSocketLocation(FName("SMG_Barrel"));
		FVector endLoc = startLoc + GetActorForwardVector()*750.0f;
		FRotator fireRot = GetMesh()->GetSocketRotation("SMG_Barrel");
		//DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red, false, 2.0f);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireFactory,startLoc, fireRot, FVector(1, 1, 1));
		bool bHit = GetWorld()->LineTraceMultiByChannel(hits, startLoc, endLoc, ECC_Pawn);
		if(bHit)
		{
			for(int i=0; i<hits.Num(); ++i)
			{
				hitActors = hits[i].GetActor();
				enemyArrayNum=i;
			}		
			if(hitActors)
			{
				AEnemy* enemy = Cast<AEnemy>(hitActors);
				if(enemy)
				{
					auto emitterLoc = enemy->GetMesh()->GetSocketLocation(FName("HitEffectSocket"));
					auto emitterRot = enemy->GetMesh()->GetSocketRotation(FName("HitEffectSocket"));
					FTransform targetTrans(hits[enemyArrayNum].ImpactPoint);
					FVector targetLoc = targetTrans.GetLocation();
					FRotator targetRot = targetTrans.Rotator();
					UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(FName("enemyFSM")));
					if(fsm)
					{
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory,emitterLoc, emitterRot, FVector(2, 2, 2));
						//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), targetMarkFactory, targetLoc, GetActorRotation()+FRotator(0, -90, 0), FVector(4));
						fsm->OnDamageProcess(30);
						enemy->OnDamaged();
					}
				}
			}
		}
	}
}

void AVayneCharacter::CursorOver(UPrimitiveComponent* primComp)
{
	//GetMesh()->SetRenderCustomDepth(true);
}

void AVayneCharacter::CursorOverEnd(UPrimitiveComponent* primComp)
{
	//GetMesh()->SetRenderCustomDepth(false);
}
