// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Activatable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActivatable : public UInterface
{
	GENERATED_BODY()
};

// An interface for Actors which should have active and inactive state
class SHAPESHIFTER_API IActivatable
{
	GENERATED_BODY()

public:
	virtual void Activate() = 0;
	virtual void Deactivate() = 0;

	virtual bool IsActive() const = 0;
	void SetActive(const bool bNewActive);
};