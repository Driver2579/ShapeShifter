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
	// Make PauseWIdget visible
	void OpenPauseMenu();

	// Make PauseWidget hidden
	void ClosePauseMenu();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PauseWidgetClass;

private:
	TWeakObjectPtr<UUserWidget> PauseWidget;
};