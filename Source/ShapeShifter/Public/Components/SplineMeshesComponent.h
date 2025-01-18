// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "SplineMeshesComponent.generated.h"

class USplineMeshComponent;

// Generates a series of SplineMeshComponents along the spline
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHAPESHIFTER_API USplineMeshesComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	USplineMeshesComponent();

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	virtual void UpdateSpline() override;

private:
	// The class with default parameters for the SplineMeshComponents
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USplineMeshComponent> SplineMeshComponentClass;

	// Stores all meshes created along the spline
	UPROPERTY(Transient)
	TArray<TObjectPtr<USplineMeshComponent>> MeshesAlongSpline;

	// Creates the meshes along the spline
	void ConstructMeshesAlongSpline();

	// Destroys all created meshes
	void DestroyMeshesAlongSpline();

	/**
	 * Destroys all created meshes using the DestroyMeshesAlongSpline function and creates new ones using the
	 * ConstructMeshesAlongSpline function.
	 */
	void ReconstructMeshesAlongSpline();
};