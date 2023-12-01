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
	
	// Widget that will be used as a warning
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UWarningWidget> WarningWidgetClass;

	UPROPERTY(meta = (BindWidget))
	UButton* NewGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitGameButton;

private:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> FirstLevel;

	UFUNCTION()
	void OnNewGameButtonClicked();

	UFUNCTION()
	void OnContinueGameButtonClicked();

	UFUNCTION()
	void OnExitGameButtonClicked();

	UFUNCTION()
	void NewGame();

	UFUNCTION()
	void ExitGame();
};