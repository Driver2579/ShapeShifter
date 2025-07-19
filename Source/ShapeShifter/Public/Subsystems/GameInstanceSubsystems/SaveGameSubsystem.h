// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGameSubsystem.generated.h"

class UShapeShifterSaveGame;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSaveLoadGameSignature, UShapeShifterSaveGame* SaveGameObject);

UCLASS()
class SHAPESHIFTER_API USaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USaveGameSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Get SaveGameObject with last save data inside or create and load it if it's not valid
	UShapeShifterSaveGame* GetOrCreateSaveGameObject(const bool bLoadSaveGameObject = true);

	// Call this after BeginPlay to save all variables of subscribed Actors to OnSaveGame delegate to save file
	void SaveGame();

	// Call this after BeginPlay to load all variables to subscribed Actors to OnLoadGame delegate from save file
	void LoadGame() const;

	// Called right before saving game. Use it to save data for a specific class to the SaveGameObject.
	FOnSaveLoadGameSignature OnSaveGame;

	// Called right after loading game. Use it to load data for a specific class from the SaveGameObject.
	FOnSaveLoadGameSignature OnLoadGame;

	FString GetSaveGameSlotName() const;

	bool IsAutoSaveAllowed() const;
	void SetAllowAutoSave(const bool bNewAllowAutoSave);

	bool WillEnableAllowAutoSaveOnOpenLevel() const;
	void SetEnableAllowAutoSaveOnOpenLevel(const bool bNewEnableAllowAutoSaveOnOpenLevel);

protected:
	virtual void OnGameStartOrLevelChanged();

	// Celled after all Actors have begun play
	virtual void OnWorldBeginPlay();

private:
	// Levels where any saving doesn't work
	TArray<FString> LevelsToIgnoreSaving;

	TWeakObjectPtr<UShapeShifterSaveGame> SaveGameObject;

	UPROPERTY(EditDefaultsOnly)
	FString SaveGameSlotName = TEXT("SaveGame");

	// Collect all Actors with Savable interface and bind their functions to OnSaveGame and OnLoadGame delegates
	void BindAllSavables();

	FAsyncLoadGameFromSlotDelegate OnAsyncLoadGameFinishedDelegate;

	void OnAsyncLoadGameFinished(const FString& SlotName, const int32 UserIndex, USaveGame* SaveGameObjectPtr);

	// Whether the game will be saved on any level open or not. If not than save will be loaded instead of saving.
	bool bAllowAutoSave = true;

	/**
	 * If true, then bAllowAutoSave will be set to true on any level open. Use it if you need to disable bAllowAutoSave
	 * for one level loading only.
	 */
	bool bEnableAllowAutoSaveOnOpenLevel = false;
};