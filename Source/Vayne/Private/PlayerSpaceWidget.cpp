// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSpaceWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Vayne/VaynePlayerController.h"

void UPlayerSpaceWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	//HoverButton->OnHovered.AddDynamic(this, &UPlayerSpaceWidget::OnHovered);
	//HoverButton->OnUnhovered.AddDynamic(this, &UPlayerSpaceWidget::UnHovered);
}

void UPlayerSpaceWidget::NativeConstruct()
{
	Super::NativeConstruct();	

	controller = Cast<AVaynePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
}

void UPlayerSpaceWidget::NativeDestruct()
{
	Super::NativeDestruct();

	//UnHovered();
}

void UPlayerSpaceWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	SetSkillInfoLoc();
	
}

void UPlayerSpaceWidget::OnHovered()
{
	SkillInfo->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerSpaceWidget::UnHovered()
{
	bool isV = SkillInfo->IsVisible();
	if(isV)
	{
		SkillInfo->SetVisibility(ESlateVisibility::Hidden);
	}

}

void UPlayerSpaceWidget::SetSkillInfoLoc()
{
	if(controller)
	{
		float XLoc;
		float YLoc;
		controller->GetMousePosition(XLoc, YLoc);
		auto renderLoc = UKismetMathLibrary::MakeVector2D(XLoc, YLoc);
		SkillInfo->SetRenderTranslation(renderLoc);
	}
}
