// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ShapeShifterGameInstance.generated.h"

UCLASS()
class SHAPESHIFTER_API UShapeShifterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	bool IsAutoSaveAllowed() const;
	void SetAllowAutoSave(const bool bNewAllowAutoSave);

	bool WillEnableAllowAutoSaveOnOpenLevel() const;
	void SetDisableAllowAutoSaveOnOpenLevel(const bool bNewEnableAllowAutoSaveOnOpenLevel);

private:
	// Whether the game will be saved on any level open or not. If not than save will be loaded instead of saving.
	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	bool bAllowAutoSave = true;

	/**
	 * If true than bAllowAutoSave will be set to true on any level open. Use it if you need to disable bAllowAutoSave
	 * for 1 level loading only. This will not work if SaveGameManager is inactive.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	bool bEnableAllowAutoSaveOnOpenLevel = false;
};