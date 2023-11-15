// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Common/Structs/SaveData/BallPawnSaveData.h"
#include "Common/Structs/SaveData/MovingPlatformSaveData.h"
#include "ShapeShifterSaveGame.generated.h"

// Use this class as a struct to store variables you want to save/load for any class
UCLASS()
class SHAPESHIFTER_API UShapeShifterSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FBallPawnSaveData BallPawnSaveData;

	// Lever instance unique name (use GetName) and Active state
	UPROPERTY()
	TMap<FString, bool> LeverSaveData;

	// MovingPlatform instance unique name (use GetName) and FMovingPlatformSaveData struct
	UPROPERTY()
	TMap<FString, FMovingPlatformSaveData> MovingPlatformSaveData;
};