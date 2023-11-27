// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/MainMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widgets/WarningWidget.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (NewGameButton)
	{
		NewGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnNewGame);
	}

	if (ContinueGameButton)
	{
		ContinueGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnContinueGame);
	}

	if (ExitGameButton)
	{
		ExitGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnExitGame);
	}
}

void UMainMenuWidget::OnNewGame()
{
	//if (!WarningWidgetClass)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::OnStartGame: WarningWidgetClass is NONE!"));

	//	return;
	//}

	//UWarningWidget* const WarningWidget = CreateWidget<UWarningWidget>(this, WarningWidgetClass);

	//if (!WarningWidget)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("UMainMenuWidget::OnStartGame: WarningWidget is invalid!"));

	//	return;
	//}

	//WarningWidget->SetMessenge("New Game");
	////WarningWidget->GetOkButton()->OnClicked.AddDynamic(this, &UMainMenuWidget::NewGame);

	//SetIsEnabled(false);

	//WarningWidget->AddToViewport();
}

void UMainMenuWidget::OnContinueGame()
{

}

void UMainMenuWidget::OnExitGame()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PlayerController)
	{
		PlayerController->ConsoleCommand("quit");
	}
}