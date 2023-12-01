// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

UCLASS()
class SHAPESHIFTER_API AMainMenuHUD : public AHUD
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	virtual void BeginPlay() override;

private:
	TWeakObjectPtr<UUserWidget> MainMenuWidget;
};