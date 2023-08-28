// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHPWidget.h"

#include "Enemy.h"
#include "Components/Image.h"
#include "Kismet/KismetMaterialLibrary.h"

void UEnemyHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPC_EnemyHP, FName("EnemyHPAlpha"), 0.99);
}
