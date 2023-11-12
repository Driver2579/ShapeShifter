// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGameManager.generated.h"

class UShapeShifterSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveLoadGameSignature, UShapeShifterSaveGame*, SaveGameObject);

UCLASS()
class SHAPESHIFTER_API ASaveGameManager : public AActor
{
	GENERATED_BODY()

public:
	ASaveGameManager();

	UShapeShifterSaveGame* GetSaveGameObject() const;

	// Call this after BeginPlay to save all variables of subscribed Actors to OnSaveGame delegate to save file
	UFUNCTION(BlueprintCallable)
	void SaveGame();

	// Call this after BeginPlay to load all variables to subscribed Actors to OnLoadGame delegate from save file
	UFUNCTION(BlueprintCallable)
	void LoadGame();

	// Called right before saving game. Use it to save data for a specific class to the SaveGameObject.
	FOnSaveLoadGameSignature OnSaveGame;

	// Called right after loading game. Use it to load data for a specific class from the SaveGameObject.
	FOnSaveLoadGameSignature OnLoadGame;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UShapeShifterSaveGame> SaveGameClass;

	TWeakObjectPtr<UShapeShifterSaveGame> SaveGameObject;

	/**
	 * Create SaveGameObject if it doesn't exists.
	 * @return true if SaveGameObject was successfully created or it was created before.
	 * @return false if failed to create SaveGameObject.
	 */
	bool CreateSaveGameObjectIfNotExists();

	UPROPERTY(EditDefaultsOnly)
	FString SaveGameSlotName = TEXT("SaveGame");

	FAsyncLoadGameFromSlotDelegate OnAsyncLoadGameFinished;
};