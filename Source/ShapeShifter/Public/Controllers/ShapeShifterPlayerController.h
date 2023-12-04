// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HUDs/ShapeShifterHUD.h"
#include "ShapeShifterPlayerController.generated.h"

UCLASS()
class SHAPESHIFTER_API AShapeShifterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// Set pause true
	void Pause();

	// Set pause false
	void Unpause();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* ShapeShifterMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputAction* PauseAction;

	virtual void SetupInputComponent() override;

private:
	TWeakObjectPtr<AShapeShifterHUD> CurrentHUD;

	// Pause or unpause game and open pause menu
	void SetPause(bool bPaused);
};