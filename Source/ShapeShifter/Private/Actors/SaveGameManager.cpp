// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/SaveGameManager.h"

#include "Kismet/GameplayStatics.h"
#include "Objects/ShapeShifterSaveGame.h"

ASaveGameManager::ASaveGameManager()
{
	SaveGameClass = UShapeShifterSaveGame::StaticClass();
}

void ASaveGameManager::BeginPlay()
{
	Super::BeginPlay();

	CreateSaveGameObjectIfNotExists();

	// Broadcast OnLoadGame to all subscribed Actors to load their variables once OnAsyncLoadGameFinished was executed
	OnAsyncLoadGameFinished.BindLambda([this](const FString& SlotName, const int32 UserIndex,
		USaveGame* SaveGameObject)
	{
		OnLoadGame.Broadcast(CastChecked<UShapeShifterSaveGame>(SaveGameObject));
	});
}

bool ASaveGameManager::CreateSaveGameObjectIfNotExists()
{
	// Don't create SaveGameObject if it's already exists
	if (SaveGameObject.IsValid())
	{
		return true;
	}

	if (!SaveGameClass)
	{
		UE_LOG(LogTemp, Error,
			TEXT("ASaveGameManager::CreateSaveGameObjectIfNotExists: Failed to create SaveGameObject. SaveGameClass is "
				"invalid!"));

		return false;
	}
	
	SaveGameObject = CastChecked<UShapeShifterSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));

	if (!SaveGameObject.IsValid())
	{
		UE_LOG(LogTemp, Error,
			TEXT("ASaveGameManager::CreateSaveGameObjectIfNotExists: Failed to create SaveGameObject!"));
			
		return false;
	}
	
	return true;
}

UShapeShifterSaveGame* ASaveGameManager::GetSaveGameObject() const
{
	return SaveGameObject.Get();
}

void ASaveGameManager::SaveGame()
{
	// Create SaveGameObject just in case and return if it's not valid 
	if (!CreateSaveGameObjectIfNotExists())
	{
		return;
	}

	// Broadcast OnSaveGame to all subscribed Actors to save their variables
	OnSaveGame.Broadcast(SaveGameObject.Get());

	// Save the game
	UGameplayStatics::AsyncSaveGameToSlot(SaveGameObject.Get(), SaveGameSlotName, 0);
}

void ASaveGameManager::LoadGame()
{
	// Load the game if save game slot exists
	if (UGameplayStatics::DoesSaveGameExist(SaveGameSlotName, 0))
	{
		UGameplayStatics::AsyncLoadGameFromSlot(SaveGameSlotName, 0, OnAsyncLoadGameFinished);
	}
}