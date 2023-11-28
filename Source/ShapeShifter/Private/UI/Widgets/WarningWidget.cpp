// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/WarningWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetTree.h"

void UWarningWidget::SetMessenge(const FString& Messenge)
{
	MessageTextBlock->SetText(FText::FromString(Messenge));
}

UButton* UWarningWidget::GetOkButton()
{
	return OkButton;
}

void UWarningWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(OkButton))
	{
		OkButton->OnClicked.AddDynamic(this, &UWarningWidget::Close);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UWarningWidget::NativeConstruct: OkButton is invalid!"));
	}

	if (IsValid(CancelButton))
	{
		CancelButton->OnClicked.AddDynamic(this, &UWarningWidget::Close);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UWarningWidget::NativeConstruct: CancelButton is invalid!"));
	}
}

void UWarningWidget::Close()
{
	// Set Enabled to true for the root widget of the parent widget
	Cast<UWidgetTree>(GetOuter())->RootWidget->SetIsEnabled(true);

	RemoveFromParent();
}