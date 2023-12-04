// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

UCLASS()
class SHAPESHIFTER_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	// Widget that will be used as a warning for some actions
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UWarningWidget> WarningWidgetClass;

	UPROPERTY(meta = (BindWidget))
	UButton* NewGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitGameButton;

private:
	UPROPERTY(EditAnywhere, Category = "New Game")
	TSoftObjectPtr<UWorld> FirstLevel;

	FString MessageBeforeStart = "Start a new game?";
	FString MessageBeforeLeave = "Leave the game?";

	UFUNCTION()
	void OnNewGameButtonClicked();

	UFUNCTION()
	void OnContinueGameButtonClicked();

	UFUNCTION()
	void OnExitGameButtonClicked();

	// Load the first level and rewrite the save
	UFUNCTION()
	void OpenFirstLevel();

	UFUNCTION()
	void ExitGame();
};