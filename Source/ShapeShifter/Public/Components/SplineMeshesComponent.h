// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "SplineMeshesComponent.generated.h"

// Generates a series of SplineMeshComponents along the spline
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHAPESHIFTER_API USplineMeshesComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	virtual void UpdateSpline() override;

	virtual void OnRegister()  override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> StaticMesh;

	// Stores all meshes that create the current rail.
	TInlineComponentArray<class USplineMeshComponent*> MeshesAlongSpline;

	void ConstructMeshesAlongSpline();
	void ReconstructMeshesAlongSpline();
};