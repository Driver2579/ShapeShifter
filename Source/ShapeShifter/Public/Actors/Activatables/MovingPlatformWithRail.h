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
public:
	AMovingPlatformWithRail();
	
protected:
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* RailStartScene;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* RailEndScene;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> RailStartStaticMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> RailEndStaticMesh;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> RailStaticMesh;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> RailStartAndEndStaticMesh;

	TInlineComponentArray<class USplineMeshComponent*> Rails;
	
	void ConstructRail();
};