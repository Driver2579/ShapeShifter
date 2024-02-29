// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "ActivatableLinesSubsystem.generated.h"

#if WITH_EDITOR
/**
 * 
 */
UCLASS()
class SHAPESHIFTER_API UActivatableLinesSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	void OnMapOpened(const FString& Filename, bool bAsTemplate) const;
};
#endif