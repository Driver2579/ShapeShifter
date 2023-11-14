// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Savable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USavable : public UInterface
{
	GENERATED_BODY()
};

class UShapeShifterSaveGame;

// An interface for Actors that should have active and inactive state
class SHAPESHIFTER_API ISavable
{
	GENERATED_BODY()

public:
	/**
	 * Called after SaveGameManager set up OnSaveGame and OnLoadGame for this Actor. It's not necessary to implement
	 * this function. The default function body is empty.
	 */
	virtual void OnSavableSetup(class ASaveGameManager* SaveGameManager) { }

	// Called right before saving game. Use it to save data for a specific class to the SaveGameObject.
	virtual void OnSaveGame(UShapeShifterSaveGame* SaveGameObject) = 0;

	// Called right after loading game. Use it to load data for a specific class from the SaveGameObject.
	virtual void OnLoadGame(UShapeShifterSaveGame* SaveGameObject) = 0;
};