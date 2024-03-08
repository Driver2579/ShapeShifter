// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HUDs/PlayerHUD.h"
#include "ShapeShifterPlayerController.generated.h"

UCLASS()
class SHAPESHIFTER_API AShapeShifterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SwitchPause();

	// Open pause menu
	void PauseGame();

	// Close pause menu
	void UnpauseGame();

protected:
	virtual void BeginPlay() override;

	// MappingContext that will always work, even when the player is not on stage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* ShapeShifterMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputAction* PauseAction;

	virtual void SetupInputComponent() override;

private:
	// HUD that is tied to the level
	TWeakObjectPtr<APlayerHUD> CurrentHUD;
};