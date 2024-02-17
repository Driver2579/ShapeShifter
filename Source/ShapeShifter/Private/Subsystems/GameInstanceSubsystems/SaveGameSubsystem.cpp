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
	// Initial SaveGameObject initialization. Recreate on world change.
	CreateSaveGameObject(false);

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

UShapeShifterSaveGame* USaveGameSubsystem::GetSaveGameObject() const
{
	return SaveGameObject.Get();
}

bool USaveGameSubsystem::CreateSaveGameObject(const bool bCreateOnlyIfNotExist)
{
	// Don't create if bCreateOnlyIfNotExist and SaveGameObject already exist
	if (bCreateOnlyIfNotExist && SaveGameObject.IsValid())
	{
		return true;
	}

	SaveGameObject = CastChecked<UShapeShifterSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UShapeShifterSaveGame::StaticClass()));

	if (!SaveGameObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("USaveGameSubsystem::CreateSaveGameObject: Failed to create SaveGameObject!"));
			
		return false;
	}

	LoadSaveGameObject();

	return true;
}

void USaveGameSubsystem::LoadSaveGameObject()
{
	FAsyncLoadGameFromSlotDelegate OnLoadFinished;

	// Load SaveGameObject on OnLoadFinished
	OnLoadFinished.BindLambda([this](const FString& SlotName, const int32 UserIndex,
		USaveGame* SaveGameObjectPtr)
	{
		SaveGameObject = CastChecked<UShapeShifterSaveGame>(SaveGameObjectPtr);
	});

	// Load SaveGameObject values if save game slot exists
	if (UGameplayStatics::DoesSaveGameExist(SaveGameSlotName, 0))
	{
		UGameplayStatics::AsyncLoadGameFromSlot(SaveGameSlotName, 0, OnLoadFinished);
	}
}

void USaveGameSubsystem::BindAllSavables()
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
	}
}

void USaveGameSubsystem::SaveGame()
{
	// Create SaveGameObject just in case and return if it's not valid 
	if (!CreateSaveGameObject())
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

void USaveGameSubsystem::LoadGame() const
{
	// Load the game if save game slot exists
	if (UGameplayStatics::DoesSaveGameExist(SaveGameSlotName, 0))
	{
		UGameplayStatics::AsyncLoadGameFromSlot(SaveGameSlotName, 0, OnAsyncLoadGameFinishedDelegate);
	}
}

void USaveGameSubsystem::OnAsyncLoadGameFinished(const FString& SlotName, const int32 UserIndex,
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
		UE_LOG(LogTemp, Warning, TEXT("USaveGameSubsystem::LoadGame: Unable to load the save from another level! "
			"Current level: %s Saved level: %s"), *CurrentLevelName, *ShapeShifterSaveGameObject->LevelName);
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