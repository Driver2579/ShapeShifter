// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SplineMeshesWithEdgesComponent.h"

#include "Components/SplineMeshComponent.h"

USplineMeshesWithEdgesComponent::USplineMeshesWithEdgesComponent()
{
	EdgeSplineMeshComponentClass = USplineMeshComponent::StaticClass();
}

bool USplineMeshesWithEdgesComponent::ConstructMeshesAlongSpline()
{
	// Don't do anything if the parent's function has failed
	if (!Super::ConstructMeshesAlongSpline())
	{
		return false;
	}

	// Don't do anything if EdgeSplineMeshComponentClass isn't set
	if (!IsValid(EdgeSplineMeshComponentClass) || !EdgeSplineMeshComponentClass->IsAsset())
	{
		return false;
	}

	const UStaticMesh* EdgeStaticMesh =
		EdgeSplineMeshComponentClass->GetDefaultObject<USplineMeshComponent>()->GetStaticMesh();

#if DO_ENSURE
	// Make sure the StaticMesh is set
	if (!ensure(IsValid(EdgeStaticMesh)))
	{
		return false;
	}
#endif

	const float EdgeSplineMeshComponentLength = GetEdgeSplineMeshComponentLength(EdgeStaticMesh);

	// Create the start and end EdgeSplineMeshComponents and add them to the MeshesAlongSpline array as required
	MeshesAlongSpline.Add(ConstructEdgeSplineMeshComponent(true, EdgeSplineMeshComponentLength));
	MeshesAlongSpline.Add(ConstructEdgeSplineMeshComponent(false, EdgeSplineMeshComponentLength));

	return true;
}

float USplineMeshesWithEdgesComponent::GetEdgeSplineMeshComponentLength(const UStaticMesh* EdgeStaticMesh) const
{
	// Return the size of the EdgeStaticMesh's bounding box along the specified axis
	switch (EdgeSplineMeshComponentLengthAxis)
	{
	case EAxis::X:
		return EdgeStaticMesh->GetBoundingBox().GetSize().X;

	case EAxis::Y:
		return EdgeStaticMesh->GetBoundingBox().GetSize().Y;

	case EAxis::Z:
		return EdgeStaticMesh->GetBoundingBox().GetSize().Z;

	default:
		return 1;
	}
}

USplineMeshComponent* USplineMeshesWithEdgesComponent::ConstructEdgeSplineMeshComponent(const bool bStartEdge,
	const float EdgeSplineMeshComponentLength)
{
	USplineMeshComponent* EdgeSplineMeshComponent = NewObject<USplineMeshComponent>(this,
		EdgeSplineMeshComponentClass);

	EdgeSplineMeshComponent->RegisterComponent();

	// Get the first or last point's index of the spline depending on whether it's the start or end edge
	const int32 SplinePointIndex = bStartEdge ? 0 : GetNumberOfSplinePoints() - 1;

	// Get the direction of the spline's point (it looks in the direction of the next point (even if it's the last one))
	const FVector SplinePointDirection = GetDirectionAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::Local);

	// Get the transform of the spline's point to transform the offset from local to world space
	const FTransform SplinePointTransform = GetTransformAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::Local);

	/**
	 * Calculate the offset of the edge depending on whether it's the start or end edge. We use an offset for the start
	 * edge by default, so we need to negate the X value for the end edge.
	 */
	const FVector StartPosOffset = bStartEdge ? EdgeSplineMeshComponentOffset :
		FVector(-EdgeSplineMeshComponentOffset.X, EdgeSplineMeshComponentOffset.Y, EdgeSplineMeshComponentOffset.Z);

	// Transform the Offset to world space
	FVector StartPos = SplinePointTransform.TransformPosition(StartPosOffset);

	// Distance from the start position to the end position of the edge considering the direction
	const FVector EndPosOffset = SplinePointDirection * EdgeSplineMeshComponentLength;

	/**
	 * Adjust the start position if it's the start edge because the SplinePointDirection points to the next point while
	 * we want to point to the "previous" point.
	 */
	if (bStartEdge)
	{
		StartPos -= EndPosOffset;
	}

	// Calculate the end position of the edge
	const FVector EndPos = StartPos + EndPosOffset;

	// Apply the calculations to the EdgeSplineMeshComponent
	EdgeSplineMeshComponent->SetStartAndEnd(StartPos, SplinePointDirection, EndPos,
		SplinePointDirection);

	// Finally, attach the EdgeSplineMeshComponent to this spline
	EdgeSplineMeshComponent->AttachToComponent(this,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	return EdgeSplineMeshComponent;
}