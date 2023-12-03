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

private:
	// Whether the game will be saved on any level open or not. If not than save will be loaded instead of saving.
	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	bool bAllowAutoSave = true;
};