// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/PauseWidget.h"

#include "Controllers/ShapeShifterPlayerController.h"
#include "Components/Button.h"
#include "GameModes/ShapeShifterGameMode.h"
#include "Actors/SaveGameManager.h"
#include "UI/Widgets/WarningWidget.h"

void UPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(ContinueButton))
	{
		ContinueButton->OnClicked.AddDynamic(this, &UPauseWidget::CloseWidget);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::NativeConstruct: ContinueButton is invalid!"));
	}

	if (IsValid(LoadButton))
	{
		LoadButton->OnClicked.AddDynamic(this, &UPauseWidget::OnLoadButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::NativeConstruct: LoadButton is invalid!"));
	}

	if (IsValid(SaveButton))
	{
		SaveButton->OnClicked.AddDynamic(this, &UPauseWidget::OnSaveButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::NativeConstruct: SaveButton is invalid!"));
	}

	if (IsValid(RestartButton))
	{
		RestartButton->OnClicked.AddDynamic(this, &UPauseWidget::OnRestartButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::NativeConstruct: RestartButton is invalid!"));
	}

	if (IsValid(ExitButton))
	{
		ExitButton->OnClicked.AddDynamic(this, &UPauseWidget::OnExitButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::NativeConstruct: ExitButton is invalid!"));
	}

	const AShapeShifterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShapeShifterGameMode>();

	if (!IsValid(GameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::NativeConstruct: GameMode is invalid!"));

		return;
	}

	SaveGameManager = GameMode->GetSaveGameManager();
}

void UPauseWidget::OnLoadButtonClicked()
{
	if (!IsValid(WarningWidgetClass)) 
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnLoadButtonClicked: WarningWidgetClass is invalid!"));

		return;
	}

	const UWarningWidget* WarningWidget = UWarningWidget::Show(this, WarningWidgetClass);

	WarningWidget->SetMessange(MessageBeforeLoad);
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UPauseWidget::Load);
}

void UPauseWidget::OnSaveButtonClicked()
{
	if (!SaveGameManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnSaveButtonClicked: SaveGameManager is invalid!"));

		return;
	}

	SaveGameManager->SaveGame();

	CloseWidget();
}

void UPauseWidget::OnRestartButtonClicked()
{
	if (!IsValid(WarningWidgetClass))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnRestartButtonClicked: WarningWidgetClass is invalid!"));

		return;
	}

	const UWarningWidget* WarningWidget = UWarningWidget::Show(this, WarningWidgetClass);

	WarningWidget->SetMessange(MessageBeforeRestart);
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UPauseWidget::Restart);
}

void UPauseWidget::OnExitButtonClicked()
{
	if (!IsValid(WarningWidgetClass))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnExitButtonClicked: WarningWidgetClass is invalid!"));

		return;
	}

	const UWarningWidget* WarningWidget = UWarningWidget::Show(this, WarningWidgetClass);

	WarningWidget->SetMessange(MessageBeforeExit);
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UPauseWidget::Exit);
}

void UPauseWidget::Load()
{
	if (!SaveGameManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::Load: SaveGameManager is invalid!"));

		return;
	}

	SaveGameManager->LoadGame();

	CloseWidget();
}

void UPauseWidget::Restart()
{
	FString CurrentLevelName = *UGameplayStatics::GetCurrentLevelName(this, true);

	if (CurrentLevelName.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::Restart: CurrentLevelName is empty!"));

		return;
	}
	
	// Restart the level
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName), true);
}

void UPauseWidget::Exit()
{
	// Open MainMenuLevel if it's valid
	if (!MainMenuLevel.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainMenuLevel);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::Exit: MainMenuLevel is invalid!"));
	}
}

void UPauseWidget::CloseWidget()
{
	// Get PlayerController to call OnUnpause
	AShapeShifterPlayerController* PlayerController = GetOwningPlayer<AShapeShifterPlayerController>();

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::CloseWidget: PlayerController is invalid!"));

		return;
	}

	PlayerController->Unpause();
}