// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ShapeShifterSaveGame.generated.h"

enum class EBallPawnForm;

// Use this class as a struct to store variables you want to save/load for any class
UCLASS()
class SHAPESHIFTER_API UShapeShifterSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FTransform PlayerTransform;

	UPROPERTY()
	FVector PlayerVelocity;

	UPROPERTY()
	EBallPawnForm PlayerForm;

	// Whether Clone was spawned in saved game or not
	UPROPERTY()
	bool bHasPlayerClone;

	UPROPERTY()
	FTransform CloneTransform;

	UPROPERTY()
	FVector CloneVelocity;
};