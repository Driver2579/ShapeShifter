// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/SaveGameManager.h"

#include "Interfaces/Savable.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/ShapeShifterSaveGame.h"

ASaveGameManager::ASaveGameManager()
{
	SaveGameClass = UShapeShifterSaveGame::StaticClass();
}

void ASaveGameManager::BeginPlay()
{
	Super::BeginPlay();

	// Initial SaveGameObject initializing
	CreateSaveGameObjectIfNotExists();

	BindAllSavables();

	OnAsyncLoadGameFinishedDelegate.BindUObject(this, &ASaveGameManager::OnAsyncLoadGameFinished);

	// Bind OnWorldBeginPlay function to OnWorldBeginPlay delegate which will be broadcast after all 
	GetWorld()->OnWorldBeginPlay.AddUObject(this, &ASaveGameManager::OnWorldBeginPlay);
}

void ASaveGameManager::OnWorldBeginPlay()
{
	/**
	 * We always save the game on level load. We call it after BeginPlay to be able to load the game on BeginPlay if
	 * needed.
	 */
	SaveGame();
}

UShapeShifterSaveGame* ASaveGameManager::GetSaveGameObject() const
{
	return SaveGameObject.Get();
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

void ASaveGameManager::BindAllSavables()
{
	// Get all Actors with Savable interface
	TArray<AActor*> SavableActors;
	UGameplayStatics::GetAllActorsWithInterface(this, USavable::StaticClass(),
		SavableActors);

	for (int i = 0; i < SavableActors.Num(); ++i)
	{
		// CastChecked to ISavable because we know for sure it's valid and it implements ISavable
		ISavable* SavableActor = CastChecked<ISavable>(SavableActors[i]);

		// Bind SavableActor to delegates
		OnSaveGame.AddRaw(SavableActor, &ISavable::OnSaveGame);
		OnLoadGame.AddRaw(SavableActor, &ISavable::OnLoadGame);

		// Call OnSavableSetup for SavableActor once we set it up
		SavableActor->OnSavableSetup(this);
	}
}

void ASaveGameManager::SaveGame()
{
	// Create SaveGameObject just in case and return if it's not valid 
	if (!CreateSaveGameObjectIfNotExists())
	{
		return;
	}

	// Save current level name
	SaveGameObject->LevelName = UGameplayStatics::GetCurrentLevelName(this);

	// Broadcast OnSaveGame to all subscribed Actors to save their variables
	OnSaveGame.Broadcast(SaveGameObject.Get());

	// Save the game
	UGameplayStatics::AsyncSaveGameToSlot(SaveGameObject.Get(), SaveGameSlotName, 0);
}

void ASaveGameManager::LoadGame() const
{
	// Load the game if save game slot exists
	if (UGameplayStatics::DoesSaveGameExist(SaveGameSlotName, 0))
	{
		UGameplayStatics::AsyncLoadGameFromSlot(SaveGameSlotName, 0, OnAsyncLoadGameFinishedDelegate);
	}
}

void ASaveGameManager::OnAsyncLoadGameFinished(const FString& SlotName, const int32 UserIndex,
	USaveGame* SaveGameObjectPtr) const
{
	const FString& CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);

	UShapeShifterSaveGame* ShapeShifterSaveGameObject = CastChecked<UShapeShifterSaveGame>(SaveGameObjectPtr);

	/**
	 * Broadcast OnLoadGame to all subscribed Actors to load their variables but only if CurrentLevelName is same as
	 * saved LevelName
	 */
	if (CurrentLevelName == ShapeShifterSaveGameObject->LevelName)
	{
		OnLoadGame.Broadcast(ShapeShifterSaveGameObject);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ASaveGameManager::LoadGame: Unable to load the save from another level! "
			"Current level: %s Saved level: %s"), *CurrentLevelName, *ShapeShifterSaveGameObject->LevelName);
	}
}