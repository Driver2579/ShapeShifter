// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Activatables/MovingPlatformWithRail.h"
#include "MovingPlatformWithRailAndEnds.generated.h"

/**
 * 
 */
UCLASS()
class SHAPESHIFTER_API AMovingPlatformWithRailAndEnds : public AMovingPlatformWithRail
{
	GENERATED_BODY()
public:
	AMovingPlatformWithRailAndEnds();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	// TODO: It might be necessary to make an heir with an end and a beginning
	//UPROPERTY(EditAnywhere)
	//TObjectPtr<UStaticMesh> RailStartAndEndStaticMesh;
	// 
	//UPROPERTY(VisibleAnywhere, Category = "Components")
	//USceneComponent* RailStartScene;
	//
	//UPROPERTY(VisibleAnywhere, Category = "Components")
	//USceneComponent* RailEndScene;

	//UPROPERTY(EditAnywhere)
	//TObjectPtr<UStaticMeshComponent> RailStartStaticMesh;

	//UPROPERTY(EditAnywhere)
	//TObjectPtr<UStaticMeshComponent> RailEndStaticMesh;
};
