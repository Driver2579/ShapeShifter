// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseWidget.generated.h"

class UButton;

UCLASS()
class SHAPESHIFTER_API UPauseWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueButton;

	UPROPERTY(meta = (BindWidget))
	UButton* LoadButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SaveButton;

	UPROPERTY(meta = (BindWidget))
	UButton* RestartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

private:
	// Widget that will be used as a warning
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UWarningWidget> WarningWidgetClass;

	// Level to exit
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> MainMenuLevel;

	TWeakObjectPtr<class ASaveGameManager> SaveGameManager;

	UFUNCTION()
	void OnLoadButtonClicked();

	UFUNCTION()
	void OnSaveButtonClicked();

	UFUNCTION()
	void OnRestartButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();

	// Load save
	UFUNCTION()
	void Load();

	// Restart all level
	UFUNCTION()
	void Restart();

	// Exit to main menu
	UFUNCTION()
	void Exit();

	// Close this menu
	UFUNCTION()
	void Close();
};