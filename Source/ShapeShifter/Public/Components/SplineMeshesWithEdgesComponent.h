// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineMeshesComponent.h"
#include "SplineMeshesWithEdgesComponent.generated.h"

/**
 * The same as SplineMeshesComponent but with two additional meshes at the edges of the spline (one at the start and one
 * at the end).
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHAPESHIFTER_API USplineMeshesWithEdgesComponent : public USplineMeshesComponent
{
	GENERATED_BODY()

public:
	USplineMeshesWithEdgesComponent();

protected:
	virtual bool ConstructMeshesAlongSpline() override;

private:
	// The class with default parameters for the edges' SplineMeshComponents
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USplineMeshComponent> EdgeSplineMeshComponentClass;

	// The offset of the EdgeSplineMeshComponent's location
	UPROPERTY(EditDefaultsOnly)
	FVector EdgeSplineMeshComponentOffset = FVector::ZeroVector;

	// The axis along which the length of the EdgeSplineMeshComponent is calculated
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EAxis::Type> EdgeSplineMeshComponentLengthAxis = EAxis::X;

	/**
	 * @param EdgeStaticMesh The StaticMesh of the EdgeSplineMeshComponent. 
	 * @return The length of the EdgeSplineMeshComponent along the EdgeSplineMeshComponentLengthAxis.
	 */
	float GetEdgeSplineMeshComponentLength(const UStaticMesh* EdgeStaticMesh) const;

	// Creates an EdgeSplineMeshComponent at the start or end of the spline and returns it
	USplineMeshComponent* ConstructEdgeSplineMeshComponent(const bool bStartEdge,
		const float EdgeSplineMeshComponentLength);
};