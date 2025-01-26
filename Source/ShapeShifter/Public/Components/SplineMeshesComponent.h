// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "SplineMeshesComponent.generated.h"

class USplineMeshComponent;

// Generates a series of SplineMeshComponents along the spline. Closed loop is not supported.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHAPESHIFTER_API USplineMeshesComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	USplineMeshesComponent();

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	virtual void UpdateSpline() override;

	const TSubclassOf<USplineMeshComponent>& GetSplineMeshComponentClass() const { return SplineMeshComponentClass; }

protected:
	/**
	 * Creates the meshes along the spline.
	 * @return Whether the meshes were allowed to be created.
	 * @remark If you override this function with more created meshes there, then you must add them to the
	 * MeshesAlongSpline array.
	 */
	virtual bool ConstructMeshesAlongSpline();

	// Stores all meshes created along the spline
	UPROPERTY(Transient)
	TArray<TObjectPtr<USplineMeshComponent>> MeshesAlongSpline;

private:
	// The class with default parameters for the SplineMeshComponents along the spline
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USplineMeshComponent> SplineMeshComponentClass;

	// Destroys all created meshes
	void DestroyMeshesAlongSpline();

	/**
	 * Destroys all created meshes using the DestroyMeshesAlongSpline function and creates new ones using the
	 * ConstructMeshesAlongSpline function.
	 */
	void ReconstructMeshesAlongSpline();
};