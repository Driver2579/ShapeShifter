// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/WarningWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetTree.h"

UWarningWidget* const UWarningWidget::Show(UUserWidget* const Parent, TSubclassOf<UWarningWidget> WarningWidgetClass)
{
	UWarningWidget* const WarningWidget = CreateWidget<UWarningWidget>(Parent, WarningWidgetClass);

	if (!IsValid(WarningWidget))
	{
		UE_LOG(LogTemp, Error, TEXT("UWarningWidget::Create: WarningWidget is invalid!"));

		return nullptr;
	}

	Parent->GetRootWidget()->SetIsEnabled(false);
	WarningWidget->AddToViewport();

	return WarningWidget;
}

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
	// Enables the root widget of the parent widget
	Cast<UWidgetTree>(GetOuter())->RootWidget->SetIsEnabled(true);

	RemoveFromParent();
}