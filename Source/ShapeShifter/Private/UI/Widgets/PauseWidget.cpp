// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/PauseWidget.h"

#include "Controllers/ShapeShifterPlayerController.h"
#include "Components/Button.h"
#include "GameModes/ShapeShifterGameMode.h"
#include "Actors/SaveGameManager.h"
#include "UI/Widgets/WarningWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widgets/WarningWidget.h"
#include "Blueprint/WidgetTree.h"


void UPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(ContinueButton))
	{
		ContinueButton->OnClicked.AddDynamic(this, &UPauseWidget::OnContinueButtonClicked);
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
}

void UPauseWidget::OnContinueButtonClicked()
{
	Close();
}

void UPauseWidget::OnLoadButtonClicked()
{
	UWarningWidget* const WarningWidget = CreateWarningWidget();

	WarningWidget->SetMessenge("Load save?");
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UPauseWidget::Load);

	GetRootWidget()->SetIsEnabled(false);
}

void UPauseWidget::OnSaveButtonClicked()
{
	AShapeShifterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShapeShifterGameMode>();

	if (!IsValid(GameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnSaveButtonClicked: GameMode is invalid!"));

		return;
	}

	ASaveGameManager* SaveGameManager = GameMode->GetSaveGameManager();

	if (!IsValid(SaveGameManager))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnSaveButtonClicked: SaveGameManager is invalid!"));

		return;
	}

	SaveGameManager->SaveGame();

	Close();
}

void UPauseWidget::OnRestartButtonClicked()
{
	UWarningWidget* const WarningWidget = CreateWarningWidget();

	WarningWidget->SetMessenge("Restart level?");
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UPauseWidget::Restart);

	GetRootWidget()->SetIsEnabled(false);
}

void UPauseWidget::OnExitButtonClicked()
{
	UWarningWidget* const WarningWidget = CreateWarningWidget();

	WarningWidget->SetMessenge("Exit to menu?");
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UPauseWidget::Exit);

	GetRootWidget()->SetIsEnabled(false);
}

void UPauseWidget::Load()
{
	AShapeShifterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShapeShifterGameMode>();

	if (!IsValid(GameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnLoadButtonClicked: GameMode is invalid!"));

		return;
	}

	ASaveGameManager* SaveGameManager = GameMode->GetSaveGameManager();

	if (!IsValid(SaveGameManager))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnLoadButtonClicked: SaveGameManager is invalid!"));

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
	AShapeShifterPlayerController* PlayerController = Cast<AShapeShifterPlayerController>(GetWorld()->GetFirstPlayerController());

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnContinueButtonClicked: PlayerController is invalid!"));

		return;
	}

	PlayerController->OnUnpause();
}

UWarningWidget* const UPauseWidget::CreateWarningWidget()
{
	if (!WarningWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::CreateWarningWidget: WarningWidgetClass is NONE!"));

		return nullptr;
	}

	UWarningWidget* const WarningWidget = CreateWidget<UWarningWidget>(this, WarningWidgetClass);

	if (!WarningWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::CreateWarningWidget: WarningWidget is invalid!"));

		return nullptr;
	}

	WarningWidget->AddToViewport();

	return WarningWidget;
}
