// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/MainMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widgets/WarningWidget.h"
#include "Blueprint/WidgetTree.h"

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
	UWarningWidget* const WarningWidget = CreateWarningWidget();

	WarningWidget->SetMessenge("Start a new game?");
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UMainMenuWidget::NewGame);

	GetRootWidget()->SetIsEnabled(false);
}

void UMainMenuWidget::OnContinueGameButtonClicked()
{
	// Will be done in the future ;)
}

void UMainMenuWidget::OnExitGameButtonClicked()
{
	UWarningWidget* const WarningWidget = CreateWarningWidget();

	WarningWidget->SetMessenge("Leave the game?");
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UMainMenuWidget::ExitGame);

	GetRootWidget()->SetIsEnabled(false);
}

void UMainMenuWidget::NewGame()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, FirstLevel);

	//if (FirstLevel.IsValid())
	//{
	//	UGameplayStatics::OpenLevelBySoftObjectPtr(this, FirstLevel);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::NewGame: FirstLevel is invalid!"));
	//}
}

void UMainMenuWidget::ExitGame()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::ExitGame: PlayerController is invalid!"));

		return;
	}

	PlayerController->ConsoleCommand("quit");
}

UWarningWidget* const UMainMenuWidget::CreateWarningWidget()
{
	if (!WarningWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::CreateWarningWidget: WarningWidgetClass is NONE!"));

		return nullptr;
	}

	UWarningWidget* const WarningWidget = CreateWidget<UWarningWidget>(this, WarningWidgetClass);

	if (!WarningWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::CreateWarningWidget: WarningWidget is invalid!"));

		return nullptr;
	}

	WarningWidget->AddToViewport();

	return WarningWidget;
}
