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

	UPROPERTY(meta = (BindWidget))
	UButton* NewGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitGameButton;

private:
	// Widget that will be used as a warning for some actions
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UWarningWidget> WarningWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "New Game")
	TSoftObjectPtr<UWorld> FirstLevel;

	UPROPERTY(EditDefaultsOnly)
	FString MessageBeforeStart = "Start a new game?";

	UPROPERTY(EditDefaultsOnly)
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