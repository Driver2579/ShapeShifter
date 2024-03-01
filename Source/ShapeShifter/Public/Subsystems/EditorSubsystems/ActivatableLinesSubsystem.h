// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "ActivatableLinesSubsystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SHAPESHIFTER_API UActivatableLinesSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void RebuildActivatableLines(const UWorld* World) const;

private:
	TWeakObjectPtr<UWorld> LastOpenedWorld;

	void OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS);
	void OnMapOpened(const FString& Filename, bool bAsTemplate) const;
};