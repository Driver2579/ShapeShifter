// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/PauseWidget.h"

#include "Controllers/ShapeShifterPlayerController.h"
#include "UI/HUDs/ShapeShifterHUD.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameModes/ShapeShifterGameMode.h"
#include "Actors/SaveGameManager.h"

void UPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UPauseWidget::OnContinue);
	}

	if (LoadButton)
	{
		LoadButton->OnClicked.AddDynamic(this, &UPauseWidget::OnLoad);
	}

	if (SaveButton)
	{
		SaveButton->OnClicked.AddDynamic(this, &UPauseWidget::OnSave);
	}

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UPauseWidget::OnRestart);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UPauseWidget::OnExit);
	}
}

void UPauseWidget::OnContinue()
{
	if (!IsValid(GetWorld()) || !IsValid(GetWorld()->GetAuthGameMode()) || !IsValid(GetWorld()->GetFirstPlayerController()))
	{
		return;
	}

	AShapeShifterPlayerController* PlayerController = Cast<AShapeShifterPlayerController>(GetWorld()->GetFirstPlayerController());

	PlayerController->OnUnpause();
}

void UPauseWidget::OnLoad()
{
	AShapeShifterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShapeShifterGameMode>();

	if (!IsValid(GameMode))
	{
		//

		return;
	}

	ASaveGameManager* SaveGameManager = GameMode->GetSaveGameManager();

	if (!IsValid(SaveGameManager))
	{
		// 

		return;
	}

	SaveGameManager->LoadGame();
}

void UPauseWidget::OnSave()
{
	AShapeShifterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShapeShifterGameMode>();

	if (!IsValid(GameMode))
	{
		//

		return;
	}

	ASaveGameManager* SaveGameManager = GameMode->GetSaveGameManager();

	if (!IsValid(SaveGameManager))
	{
		// 

		return;
	}

	SaveGameManager->SaveGame();
}

void UPauseWidget::OnRestart()
{

}

void UPauseWidget::OnExit()
{

}