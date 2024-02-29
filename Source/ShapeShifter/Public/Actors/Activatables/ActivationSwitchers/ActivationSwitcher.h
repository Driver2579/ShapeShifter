// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActivationSwitcher.generated.h"

/**
 * This class is used to create mechanisms which will activate or deactivate other Actors on scene which implement
 * Activatable interface.
 */
UCLASS(Abstract)
class SHAPESHIFTER_API AActivationSwitcher : public AActor
{
	GENERATED_BODY()

public:
	void GetActorsToSwitchActivation(TArray<AActor*>& OutActorsToSwitchActivation) const;

protected:
	/**
	 * Actors in this array must implement Activatable interface. They will be activated or deactivated on overlap
	 * depending on their current state
	 */
	UPROPERTY(EditInstanceOnly, Category = "Activation")
	TArray<AActor*> ActorsToSwitchActivation;

	void SwitchActorsActivation() const;
};