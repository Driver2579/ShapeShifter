// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/GameInstanceSubsystems/SaveGameSubsystem.h"

#include "GameInstances/ShapeShifterGameInstance.h"
#include "Interfaces/Savable.h"
#include "Objects/ShapeShifterSaveGame.h"

USaveGameSubsystem::USaveGameSubsystem()
{
	LevelsToIgnoreSaving.Add(TEXT("L_MainMenu"));
}

void USaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Get GameInstance and subscribe to the OnGameStartOrLevelChanged
	UShapeShifterGameInstance* GameInstance = CastChecked<UShapeShifterGameInstance>(GetGameInstance());
	GameInstance->OnGameStartOrLevelChanged.AddUObject(this, &USaveGameSubsystem::OnGameStartOrLevelChanged);
}

void USaveGameSubsystem::OnGameStartOrLevelChanged()
{
	/**
	 * Bind OnWorldBeginPlay function to OnWorldBeginPlay delegate which will be broadcast after all Actors called their
	 * BeginPlay
	 */
	GetWorld()->OnWorldBeginPlay.AddUObject(this, &USaveGameSubsystem::OnWorldBeginPlay);
}

void USaveGameSubsystem::OnWorldBeginPlay()
{
	if (LevelsToIgnoreSaving.Contains(UGameplayStatics::GetCurrentLevelName(this)))
	{
		return;
	}

	BindAllSavables();

	OnAsyncLoadGameFinishedDelegate.BindUObject(this, &USaveGameSubsystem::OnAsyncLoadGameFinished);

	/**
	 * We always save the game on level load if auto save is allowed. We call it after BeginPlay to be able to load the
	 * game on BeginPlay if needed.
	 */
	if (bAllowAutoSave)
	{
		SaveGame();

		return;
	}

	// Load the game in another case as bAllowAutoSave documentation in the ShapeShifterGameInstance says
	LoadGame();

	// Enable AllowAutoSave if we need to
	if (bEnableAllowAutoSaveOnOpenLevel)
	{
		bAllowAutoSave = true;
	}
}

UShapeShifterSaveGame* USaveGameSubsystem::GetOrCreateSaveGameObject(const bool bLoadSaveGameObject)
{
	if (SaveGameObject.IsValid())
	{
		return SaveGameObject.Get();
	}

	// Load SaveGameObject values if we have to and save game slot exists
	if (bLoadSaveGameObject && UGameplayStatics::DoesSaveGameExist(SaveGameSlotName, 0))
	{
		SaveGameObject = CastChecked<UShapeShifterSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SaveGameSlotName, 0));
	}
	else
	{
		SaveGameObject = CastChecked<UShapeShifterSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UShapeShifterSaveGame::StaticClass()));
	}

	if (!SaveGameObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("USaveGameSubsystem::GetOrCreateSaveGameObject: Failed to create SaveGameObject!"));
			
		return nullptr;
	}

	return SaveGameObject.Get();
}

void USaveGameSubsystem::BindAllSavables()
{
	// Clear all subscriptions from the previous level
	OnSaveGame.Clear();
	OnLoadGame.Clear();

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
	}
}

void USaveGameSubsystem::SaveGame()
{
	if (!IsValid(GetOrCreateSaveGameObject()))
	{
		UE_LOG(LogTemp, Error, TEXT("USaveGameSubsystem::SaveGame: SaveGameObject is invalid!"));

		return;
	}

	// Save current level name
	SaveGameObject->LevelName = UGameplayStatics::GetCurrentLevelName(this);

	// Broadcast OnSaveGame to all subscribed Actors to save their variables
	OnSaveGame.Broadcast(SaveGameObject.Get());

	// Save the game
	UGameplayStatics::AsyncSaveGameToSlot(SaveGameObject.Get(), SaveGameSlotName, 0);
}

void USaveGameSubsystem::LoadGame() const
{
	// Load the game if save game slot exists
	if (UGameplayStatics::DoesSaveGameExist(SaveGameSlotName, 0))
	{
		UGameplayStatics::AsyncLoadGameFromSlot(SaveGameSlotName, 0, OnAsyncLoadGameFinishedDelegate);
	}
}

void USaveGameSubsystem::OnAsyncLoadGameFinished(const FString& SlotName, const int32 UserIndex,
	USaveGame* SaveGameObjectPtr)
{
	const FString& CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);

	SaveGameObject = CastChecked<UShapeShifterSaveGame>(SaveGameObjectPtr);

	/**
	 * Broadcast OnLoadGame to all subscribed Actors to load their variables but only if CurrentLevelName is same as
	 * saved LevelName
	 */
	if (CurrentLevelName == SaveGameObject->LevelName)
	{
		OnLoadGame.Broadcast(SaveGameObject.Get());
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("USaveGameSubsystem::OnAsyncLoadGameFinished: Unable to load the save from another level! Current "
				"level: %s Saved level: %s"), *CurrentLevelName, *SaveGameObject->LevelName);
	}
}

FString USaveGameSubsystem::GetSaveGameSlotName() const
{
	return SaveGameSlotName;
}

bool USaveGameSubsystem::IsAutoSaveAllowed() const
{
	return bAllowAutoSave;
}

void USaveGameSubsystem::SetAllowAutoSave(const bool bNewAllowAutoSave)
{
	bAllowAutoSave = bNewAllowAutoSave;
}

bool USaveGameSubsystem::WillEnableAllowAutoSaveOnOpenLevel() const
{
	return bEnableAllowAutoSaveOnOpenLevel;
}

void USaveGameSubsystem::SetDisableAllowAutoSaveOnOpenLevel(const bool bNewEnableAllowAutoSaveOnOpenLevel)
{
	bEnableAllowAutoSaveOnOpenLevel = bNewEnableAllowAutoSaveOnOpenLevel;
}