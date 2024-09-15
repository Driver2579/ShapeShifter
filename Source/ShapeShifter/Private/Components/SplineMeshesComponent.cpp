// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SplineMeshesComponent.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

USplineMeshesComponent::USplineMeshesComponent()
{
	SplineMeshComponentClass = USplineMeshComponent::StaticClass();

	Mobility = EComponentMobility::Static;
}

void USplineMeshesComponent::OnRegister()
{
	Super::OnRegister();

	ConstructMeshesAlongSpline();
}

void USplineMeshesComponent::OnUnregister()
{
	Super::OnUnregister();

	DestroyMeshesAlongSpline();
}

void USplineMeshesComponent::UpdateSpline()
{
	Super::UpdateSpline();

	ReconstructMeshesAlongSpline();
}

void USplineMeshesComponent::ConstructMeshesAlongSpline()
{
	// Don't do anything if SplineMeshComponentClass isn't set
	if (!IsValid(SplineMeshComponentClass))
	{
		return;
	}

#if DO_ENSURE
	// Make sure StaticMesh is set
	if (!ensure(SplineMeshComponentClass->GetDefaultObject<USplineMeshComponent>()->GetStaticMesh()))
	{
		return;
	}
#endif

	// The number of segments on the spline excluding the last one because of "i + 1" in the next loop
	const int32 NumSegments = GetNumberOfSplinePoints() - 1;

	// Build new meshes by creating a mesh for each segment on the spline
	for (int32 i = 0; i < NumSegments; ++i)
	{
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this,
			SplineMeshComponentClass);

		SplineMeshComponent->RegisterComponent();

		// Stretch the mesh to the entire segment
		SplineMeshComponent->SetStartAndEnd(
			GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local),
			GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local),
			GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local),
			GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local)
		);

		SplineMeshComponent->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		MeshesAlongSpline.Add(SplineMeshComponent);
	}
}

void USplineMeshesComponent::DestroyMeshesAlongSpline()
{
	// Destroy old meshes
	for (USplineMeshComponent* Rail : MeshesAlongSpline)
	{
		if (Rail)
		{
			Rail->DestroyComponent();
		}
	}
	
	// Clear an array of old meshes
	MeshesAlongSpline.Reset();
}

void USplineMeshesComponent::ReconstructMeshesAlongSpline()
{
	DestroyMeshesAlongSpline();
	ConstructMeshesAlongSpline();
}