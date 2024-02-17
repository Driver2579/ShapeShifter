// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ShapeShifterGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnWorldInitializedOrChangedDelegate);

UCLASS()
class SHAPESHIFTER_API UShapeShifterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	FOnWorldInitializedOrChangedDelegate OnGameStartOrLevelChanged;

protected:
	virtual void OnStart() override;
	virtual void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;
};