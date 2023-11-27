// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/WarningWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UWarningWidget::SetMessenge(const FString& Messenge)
{
	MessageTextBlock->SetText(FText::FromString(Messenge));
}

UButton* UWarningWidget::GetOkButton()
{
	return OKButton;
}

void UWarningWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (OKButton)
	{
		OKButton->OnClicked.AddDynamic(this, &UWarningWidget::OnOK);
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &UWarningWidget::OnCancel);
	}
}

void UWarningWidget::OnOK()
{
	RemoveFromParent();
}

void UWarningWidget::OnCancel()
{
	RemoveFromParent();
}

