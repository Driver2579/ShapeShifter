// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/PauseWidget.h"

#include "Controllers/ShapeShifterPlayerController.h"
#include "Components/Button.h"
#include "UI/Widgets/WarningWidget.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Subsystems/GameInstanceSubsystems/SaveGameSubsystem.h"

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

	OnVisibilityChanged.AddDynamic(this, &UPauseWidget::SetPauseMusicState);

	if (!IsValid(MenuMusic))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::NativeConstruct: MenuMusic is invalid!"));

		return;
	}

	MenuMusicAudioComponent = UGameplayStatics::SpawnSound2D(this, MenuMusic);
}

void UPauseWidget::OnLoadButtonClicked()
{
	if (!WarningWidgetClass) 
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnLoadButtonClicked: WarningWidgetClass is invalid!"));

		return;
	}

	const UWarningWidget* WarningWidget = UWarningWidget::Show(this, WarningWidgetClass);

	if (!IsValid(WarningWidget))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnLoadButtonClicked: WarningWidget is invalid!"));

		return;
	}
	
	WarningWidget->SetMessage(MessageBeforeLoad);

	if (!IsValid(WarningWidget->GetOkButton()))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnLoadButtonClicked: OkButton is invalid!"));

		return;
	}
	
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UPauseWidget::Load);
}

void UPauseWidget::OnSaveButtonClicked()
{
	GetGameInstance()->GetSubsystem<USaveGameSubsystem>()->SaveGame();

	CloseWidget();
}

void UPauseWidget::OnRestartButtonClicked()
{
	if (!WarningWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnRestartButtonClicked: WarningWidgetClass is invalid!"));

		return;
	}

	const UWarningWidget* WarningWidget = UWarningWidget::Show(this, WarningWidgetClass);

	if (!IsValid(WarningWidget))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnRestartButtonClicked: WarningWidget is invalid!"));

		return;
	}
	
	WarningWidget->SetMessage(MessageBeforeRestart);
	
	if (!IsValid(WarningWidget->GetOkButton()))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnRestartButtonClicked: OkButton is invalid!"));

		return;
	}
	
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UPauseWidget::Restart);
}

void UPauseWidget::OnExitButtonClicked()
{
	if (!WarningWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnExitButtonClicked: WarningWidgetClass is invalid!"));

		return;
	}

	const UWarningWidget* WarningWidget = UWarningWidget::Show(this, WarningWidgetClass);

	if (!IsValid(WarningWidget))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnExitButtonClicked: WarningWidget is invalid!"));

		return;
	}
	
	WarningWidget->SetMessage(MessageBeforeExit);

	if (!IsValid(WarningWidget->GetOkButton()))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::OnExitButtonClicked: OkButton is invalid!"));

		return;
	}
	
	WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UPauseWidget::Exit);
}

void UPauseWidget::Load()
{
	GetGameInstance()->GetSubsystem<USaveGameSubsystem>()->LoadGame();

	CloseWidget();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UPauseWidget::Restart()
{
	const FString CurrentLevelName = *UGameplayStatics::GetCurrentLevelName(this,
		true);
	
	// Restart the level
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName), true);
}

// ReSharper disable once CppMemberFunctionMayBeConst
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

// ReSharper disable once CppMemberFunctionMayBeConst
void UPauseWidget::CloseWidget()
{
	// Get PlayerController to call Unpause
	AShapeShifterPlayerController* PlayerController = GetOwningPlayer<AShapeShifterPlayerController>();

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::CloseWidget: PlayerController is invalid!"));

		return;
	}

	PlayerController->Unpause();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UPauseWidget::SetPauseMusicState(const ESlateVisibility InVisibility)
{
	if (!MenuMusicAudioComponent.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UPauseWidget::SetPauseMusicState: MenuMusicAudioComponent is invalid!"));

		return;
	}

	// Play music when the pause menu is open
	if (ESlateVisibility::Visible == InVisibility)
	{
		MenuMusicAudioComponent->SetPaused(false);
	}
	// Stop music when the pause menu is close
	else
	{
		MenuMusicAudioComponent->SetPaused(true);
	}
}
