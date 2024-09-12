// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SplineMeshesComponent.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

void USplineMeshesComponent::OnRegister()
{
	Super::OnRegister();

	ReconstructMeshesAlongSpline();
}

void USplineMeshesComponent::UpdateSpline()
{
	Super::UpdateSpline();

	ReconstructMeshesAlongSpline();
}

void USplineMeshesComponent::ConstructMeshesAlongSpline()
{
	// The number of segments on the spline excluding the last one because of "i + 1" in the next loop
	const int32 NumSegments = GetNumberOfSplinePoints() - 1;

	// Build new meshes by creating a mesh for each segment on the spline
	for (int32 i = 0; i < NumSegments; ++i)
	{
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this);

		SplineMeshComponent->RegisterComponent();

		SplineMeshComponent->SetMobility(Mobility);
		SplineMeshComponent->SetStaticMesh(StaticMesh);

		// Stretch the mesh to the entire segment
		SplineMeshComponent->SetStartAndEnd(
			GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Type::Local),
			GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Type::Local),
			GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Type::Local),
			GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Type::Local)
		);

		SplineMeshComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

		MeshesAlongSpline.Add(SplineMeshComponent);
	}
}

void USplineMeshesComponent::ReconstructMeshesAlongSpline()
{
	// Destroy old meshes
	for (USplineMeshComponent* Rail : MeshesAlongSpline)
	{
		Rail->DestroyComponent();
	}

	// Clear an array of old meshes
	MeshesAlongSpline.Reset();

	// Build new meshes
	ConstructMeshesAlongSpline();
}