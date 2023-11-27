// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "ShapeShifterPlayerController.generated.h"

UCLASS()
class SHAPESHIFTER_API AShapeShifterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void OnPause();
	void OnUnpause();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enhanced Input")
	class UInputMappingContext* ShapeShifterMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enhanced Input")
	class UInputAction* PauseAction;

private:
	void OnSetPause(bool bPaused);
};