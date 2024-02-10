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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds")
	USoundCue* MenuMusic;
	
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
	void CloseWidget();

	// Widget that will be used as a warning for some actions
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UWarningWidget> WarningWidgetClass;

	// Level to exit
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> MainMenuLevel;
	
	UPROPERTY(EditDefaultsOnly)
	FString MessageBeforeLoad = "Load save?";

	UPROPERTY(EditDefaultsOnly)
	FString MessageBeforeRestart = "Restart level?";

	UPROPERTY(EditDefaultsOnly)
	FString MessageBeforeExit = "Exit to menu?";
	
	TWeakObjectPtr<class ASaveGameManager> SaveGameManager;
	
	// This is needed to have control over MenuMusic from any part of this class code
	TSoftObjectPtr<UAudioComponent> MenuMusicAudioComponent;
	
	UFUNCTION()
	void SetPauseMusicState(ESlateVisibility InVisibility);
};