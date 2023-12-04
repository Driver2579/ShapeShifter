// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/MainMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widgets/WarningWidget.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (IsValid(NewGameButton))
	{
		NewGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnNewGameButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::NativeConstruct: NewGameButton is invalid!"));
	}

	if (IsValid(ContinueGameButton))
	{
		ContinueGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnContinueGameButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::NativeConstruct: ContinueGameButton is invalid!"));
	}

	if (IsValid(ExitGameButton))
	{
		ExitGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnExitGameButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::NativeConstruct: ExitGameButton is invalid!"));
	}
}

void UMainMenuWidget::OnNewGameButtonClicked()
{
	if (!IsValid(WarningWidgetClass))
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::OnNewGameButtonClicked: WarningWidgetClass is invalid!"));

		return;
	}

	const UWarningWidget* WarningWidget = UWarningWidget::Show(this, WarningWidgetClass);

	WarningWidget->SetMessange(MessageBeforeStart);
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UMainMenuWidget::OpenFirstLevel);
}

void UMainMenuWidget::OnContinueGameButtonClicked()
{
	// Will be done in the future ;)
}

void UMainMenuWidget::OnExitGameButtonClicked()
{
	if (!IsValid(WarningWidgetClass))
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::OnExitGameButtonClicked: WarningWidgetClass is invalid!"));

		return;
	}

	const UWarningWidget* WarningWidget = UWarningWidget::Show(this, WarningWidgetClass);

	WarningWidget->SetMessange(MessageBeforeLeave);
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UMainMenuWidget::ExitGame);
}

void UMainMenuWidget::OpenFirstLevel()
{
	if (!FirstLevel.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, FirstLevel);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::OpenFirstLevel: FirstLevel is invalid!"));
	}
}

void UMainMenuWidget::ExitGame()
{
	if (!IsValid(GetOwningPlayer()))
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::ExitGame: PlayerController is invalid!"));

		return;
	}

	GetOwningPlayer()->ConsoleCommand("quit");
}