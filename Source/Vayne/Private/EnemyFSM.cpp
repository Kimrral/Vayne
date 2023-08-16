// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"

#include "Enemy.h"
#include "EnemyAnim.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Vayne/VayneCharacter.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	state = EEnemyState::IDLE;
	me = Cast<AEnemy>(GetOwner());
	
	// Set HP
	curHP=maxHP;
	
}

// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//SetRotToPlayer();
	
	switch (state)
	{
	case EEnemyState::IDLE:
		TickIdle();
		break;
	case EEnemyState::MOVE:
		TickMove();
		break;
	case EEnemyState::ATTACK:
		TickAttack();
		break;
	case EEnemyState::DAMAGE:
		TickDamage();
		break;
	case EEnemyState::DIE:
		TickDie();
		break;
	}
	
}

void UEnemyFSM::TickIdle()
{
	// Find Player
	player = Cast<AVayneCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	auto distToPlayer = player->GetDistanceTo(me);
	if(player&&distToPlayer<=aggressiveRange)
	{
		SetState(EEnemyState::MOVE);
	}
}

void UEnemyFSM::TickMove()
{
	FVector dir = player->GetActorLocation() - me->GetActorLocation();
	SetRotToPlayer();
	me->AddMovementInput(dir.GetSafeNormal());	
	float dist = player->GetDistanceTo(me);
	if(dist<=attackRange)
	{
		SetState(EEnemyState::ATTACK);
	}
}

void UEnemyFSM::TickAttack()
{
	curTime+=GetWorld()->DeltaTimeSeconds;
	if(!bIsAttacking&&curTime>=0.1f)
	{
		me->enemyAnim->bIsAttacking=true;
		bIsAttacking=true;
		float dist = player->GetDistanceTo(me);
		{
			if(dist<=attackRange)
			{
				if(player)
				{
		
				}
			}
		}
	}
	if(curTime>=attackDelayTime)
	{
		// 플레이어와의 거리 도출
		float dist = player->GetDistanceTo(me);
		// 공격거리보다 멀어졌다면
		if(dist>attackRange)
		{
			// 이동상태로 전이한다
			SetState(EEnemyState::MOVE);
		}
		else
		{
			curTime=0;
			bIsAttacking=false;
			me->enemyAnim->bIsAttacking=false;
		}
	}
}

void UEnemyFSM::TickDamage()
{
	curTime+=GetWorld()->GetDeltaSeconds();
	if(curTime>1)
	{
		SetState(EEnemyState::MOVE);
		curTime=0;
	}
}

void UEnemyFSM::TickDie()
{
	if(bTickDie==false)
	{
		bTickDie=true;
		me->OnDie();
	}
}

void UEnemyFSM::OnDamageProcess(int damageValue)
{
	curHP=FMath::Clamp(curHP-=damageValue, 0, 100);
	if(curHP<=0)
	{
		// Die Process
		SetState(EEnemyState::DIE);
	}
	else
	{
		// Damage Process
		SetState(EEnemyState::DAMAGE);
	}
	UE_LOG(LogTemp, Warning, TEXT("Enemy HP : %d"), curHP);
}

void UEnemyFSM::SetState(EEnemyState next)
{
	state=next;
	me->enemyAnim->state=next;
}

void UEnemyFSM::SetRotToPlayer()
{
	if(player)
	{
		FVector dir = player->GetActorLocation() - me->GetActorLocation();
		FRotator attackRot = UKismetMathLibrary::MakeRotFromXZ(dir, player->GetActorUpVector());
		me->SetActorRotation(FRotator(0, attackRot.Yaw, 0));
	}
}




