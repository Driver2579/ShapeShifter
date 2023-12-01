// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShapeShifterHUD.generated.h"

UCLASS()
class SHAPESHIFTER_API AShapeShifterHUD : public AHUD
{
	GENERATED_BODY()

public:
	// Make visible PauseWidget
	void OpenPauseMenu();

	// Make hidden PauseWidget
	void ClosePauseMenu();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PauseWidgetClass;

	virtual void BeginPlay() override;

private:
	TWeakObjectPtr<UUserWidget> PauseWidget;
};