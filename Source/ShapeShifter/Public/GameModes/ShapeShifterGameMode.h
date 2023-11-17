// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShapeShifterGameMode.generated.h"

class ASaveGameManager;

UCLASS()
class SHAPESHIFTER_API AShapeShifterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASaveGameManager* GetSaveGameManager() const;

protected:
	virtual void StartPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	TSubclassOf<ASaveGameManager> SaveGameManagerClass;

	TWeakObjectPtr<ASaveGameManager> SaveGameManager;
};