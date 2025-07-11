// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

UCLASS()
class SHAPESHIFTER_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	// Make PauseWIdget visible
	void OpenPauseMenu();

	// Make PauseWidget hidden
	void ClosePauseMenu();

protected:
	virtual void BeginPlay() override;

private:
	TWeakObjectPtr<UUserWidget> PauseWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PauseWidgetClass;
};