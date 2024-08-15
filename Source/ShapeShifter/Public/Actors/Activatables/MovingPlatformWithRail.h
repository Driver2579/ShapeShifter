// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Activatables/MovingPlatform.h"
#include "MovingPlatformWithRail.generated.h"

/**
 *
 *
 */
UCLASS()
class SHAPESHIFTER_API AMovingPlatformWithRail : public AMovingPlatform
{
	GENERATED_BODY()

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> RailStaticMesh;

	// Stores all meshes that create the current rail.
	TInlineComponentArray<class USplineMeshComponent*> Rails;
	
	void ConstructRail();
};