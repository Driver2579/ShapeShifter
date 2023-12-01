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

	// Bind buttons to functions

	if (IsValid(ContinueButton))
	{
		ContinueButton->OnClicked.AddDynamic(this, &UPauseWidget::Close);
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

	// Get SaveGameManager to save it

	AShapeShifterGameMode* const GameMode = GetWorld()->GetAuthGameMode<AShapeShifterGameMode>();

	if (!IsValid(GameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnLoadButtonClicked: GameMode is invalid!"));

		return;
	}

	SaveGameManager = GameMode->GetSaveGameManager();
}

void UPauseWidget::OnLoadButtonClicked()
{
	UWarningWidget* const WarningWidget = UWarningWidget::Show(this, WarningWidgetClass);

	WarningWidget->SetMessenge("Load save?");
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

	Close();
}

void UPauseWidget::OnRestartButtonClicked()
{
	UWarningWidget* const WarningWidget = UWarningWidget::Show(this, WarningWidgetClass);

	WarningWidget->SetMessenge("Restart level?");
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UPauseWidget::Restart);
}

void UPauseWidget::OnExitButtonClicked()
{
	UWarningWidget* const WarningWidget = UWarningWidget::Show(this, WarningWidgetClass);

	WarningWidget->SetMessenge("Exit to menu?");
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

	Close();
}

void UPauseWidget::Restart()
{
	// Get the current level name
	FString CurrentLevelName = *UGameplayStatics::GetCurrentLevelName(this, true);

	if (CurrentLevelName.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::Restart: CurrentLevelName is empty!"));

		return;
	}
	
	// Reload a level
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName), true);
}

void UPauseWidget::Exit()
{
	// Open main menu if is valid
	if (!MainMenuLevel.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainMenuLevel);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::Exit: MainMenuLevel is invalid!"));
	}
}

void UPauseWidget::Close()
{
	// Get PlayerController to OnUnpause
	AShapeShifterPlayerController* PlayerController = Cast<AShapeShifterPlayerController>(GetWorld()->GetFirstPlayerController());

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::Close: PlayerController is invalid!"));

		return;
	}

	PlayerController->OnUnpause();
}