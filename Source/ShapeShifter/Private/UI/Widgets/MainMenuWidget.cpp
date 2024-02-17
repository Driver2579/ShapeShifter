// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/MainMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/ShapeShifterSaveGame.h"
#include "Subsystems/GameInstanceSubsystems/SaveGameSubsystem.h"
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
	if (!WarningWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::OnNewGameButtonClicked: WarningWidgetClass is invalid!"));

		return;
	}
	
	const UWarningWidget* WarningWidget = UWarningWidget::Show(this, WarningWidgetClass);

	if (!IsValid(WarningWidget))
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::OnNewGameButtonClicked: WarningWidget is invalid!"));

		return;
	}
	
	WarningWidget->SetMessage(MessageBeforeStart);

	if (!IsValid(WarningWidget->GetOkButton()))
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::OnNewGameButtonClicked: OkButton is invalid!"));

		return;
	}
	
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UMainMenuWidget::OpenFirstLevel);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UMainMenuWidget::OnContinueGameButtonClicked()
{
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();

	const UShapeShifterSaveGame* SaveGameObject = SaveGameSubsystem->CreateAndLoadSaveGameObject();

	if (!IsValid(SaveGameObject))
	{
		UE_LOG(LogTemp, Error,
			TEXT("UMainMenuWidget::OnContinueGameButtonClicked: Failed to get SaveGameObject from the "
				"SaveGameSubsystem!"));

		return;
	}

	// Disable auto save to load a saved game on level load instead of saving it
	SaveGameSubsystem->SetAllowAutoSave(false);

	// Enable auto save back after loading the level and saved game
	SaveGameSubsystem->SetDisableAllowAutoSaveOnOpenLevel(true);

	// Open saved level
	UGameplayStatics::OpenLevel(this, *SaveGameObject->LevelName);
}

void UMainMenuWidget::OnExitGameButtonClicked()
{
	if (!WarningWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::OnExitGameButtonClicked: WarningWidgetClass is invalid!"));

		return;
	}
	
	const UWarningWidget* WarningWidget = UWarningWidget::Show(this, WarningWidgetClass);

	if (!IsValid(WarningWidget))
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::OnExitGameButtonClicked: WarningWidget is invalid!"));

		return;
	}
	
	WarningWidget->SetMessage(MessageBeforeLeave);

	if (!IsValid(WarningWidget->GetOkButton()))
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::OnExitGameButtonClicked: OkButton is invalid!"));

		return;
	}
	
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UMainMenuWidget::ExitGame);
}

// ReSharper disable once CppMemberFunctionMayBeConst
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

// ReSharper disable once CppMemberFunctionMayBeConst
void UMainMenuWidget::ExitGame()
{
	if (!IsValid(GetOwningPlayer()))
	{
		UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::ExitGame: PlayerController is invalid!"));

		return;
	}

	GetOwningPlayer()->ConsoleCommand("quit");
}