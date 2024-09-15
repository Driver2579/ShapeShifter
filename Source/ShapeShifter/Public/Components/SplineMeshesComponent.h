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
	virtual void OnRegister() override;

	virtual void UpdateSpline() override;

private:
	// The mesh that will be spawned along the spline
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> StaticMesh;

	// Stores all meshes that were created along the spline
	UPROPERTY(Transient)
	TArray<TObjectPtr<class USplineMeshComponent>> MeshesAlongSpline;

	// Creates the meshes along the spline
	void ConstructMeshesAlongSpline();

	// Destroys all created meshes and creates new ones using ConstructMeshesAlongSpline function
	void ReconstructMeshesAlongSpline();
};