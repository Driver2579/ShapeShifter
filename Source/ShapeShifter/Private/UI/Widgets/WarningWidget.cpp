// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/WarningWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

void UWarningWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(OkButton))
	{
		OkButton->OnClicked.AddDynamic(this, &UWarningWidget::CloseWidget);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UWarningWidget::NativeConstruct: OkButton is invalid!"));
	}

	if (IsValid(CancelButton))
	{
		CancelButton->OnClicked.AddDynamic(this, &UWarningWidget::CloseWidget);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UWarningWidget::NativeConstruct: CancelButton is invalid!"));
	}
}

const UWarningWidget* UWarningWidget::Show(UUserWidget* Parent, TSubclassOf<UWarningWidget> WarningWidgetClass)
{
	if (!IsValid(Parent))
	{
		UE_LOG(LogTemp, Error, TEXT("UWarningWidget::Show: Parent is invalid!"));

		return nullptr;
	}

	if (!WarningWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UWarningWidget::Show: WarningWidgetClass is invalid!"));

		return nullptr;
	}

	UWarningWidget* WarningWidget = CreateWidget<UWarningWidget>(Parent, WarningWidgetClass);

	if (!IsValid(WarningWidget))
	{
		UE_LOG(LogTemp, Error, TEXT("UWarningWidget::Show: WarningWidget is invalid!"));

		return nullptr;
	}

	const UWidgetTree* ParentWidgetTree = Cast<UWidgetTree>(Parent->WidgetTree);

	if (!IsValid(ParentWidgetTree))
	{
		UE_LOG(LogTemp, Error, TEXT("UWarningWidget::Show: ParentWidgetTree is invalid!"));

		return nullptr;
	}

	if (!IsValid(ParentWidgetTree->RootWidget))
	{
		UE_LOG(LogTemp, Error, TEXT("UWarningWidget::Show: ParentRootWidget is invalid!"));

		return nullptr;
	}

	ParentWidgetTree->RootWidget->SetIsEnabled(false);

	WarningWidget->AddToViewport();

	return WarningWidget;
}

void UWarningWidget::SetMessage(const FString& Message) const
{
	check(MessageTextBlock)
	
	MessageTextBlock->SetText(FText::FromString(Message));
}

UButton* UWarningWidget::GetOkButton() const 
{
	return OkButton;
}

void UWarningWidget::CloseWidget()
{
	const UWidgetTree* OuterWidgetTree = Cast<UWidgetTree>(GetOuter());

	if (!IsValid(OuterWidgetTree))
	{
		UE_LOG(LogTemp, Error, TEXT("UWarningWidget::CloseWidget: OuterWidgetTree is invalid!"));

		return;
	}

	UWidget* OuterRootWidget = OuterWidgetTree->RootWidget;

	if (!IsValid(OuterRootWidget))
	{
		UE_LOG(LogTemp, Error, TEXT("UWarningWidget::CloseWidget: OuterRootWidget is invalid!"));

		return;
	}

	// Enable root widget of the parent widget
	OuterRootWidget->SetIsEnabled(true);

	RemoveFromParent();
}