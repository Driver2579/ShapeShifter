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

	// Widget that will be used as a warning
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UWarningWidget> WarningWidgetClass;

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
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> MainMenuLevel;

	UFUNCTION()
	void OnContinueButtonClicked();

	UFUNCTION()
	void OnLoadButtonClicked();

	UFUNCTION()
	void OnSaveButtonClicked();

	UFUNCTION()
	void OnRestartButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();

	UFUNCTION()
	void Load();

	UFUNCTION()
	void Restart();

	UFUNCTION()
	void Exit();

	void Close();

	UWarningWidget* const CreateWarningWidget();
};