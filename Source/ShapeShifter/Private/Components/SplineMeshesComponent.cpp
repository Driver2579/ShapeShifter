// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SplineMeshesComponent.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

USplineMeshesComponent::USplineMeshesComponent()
{
	SplineMeshComponentClass = USplineMeshComponent::StaticClass();

	// Set all points to a curve type by default
	for (auto& SplinePoint : SplineCurves.Position.Points)
	{
		SplinePoint.InterpMode = ConvertSplinePointTypeToInterpCurveMode(ESplinePointType::Curve);
	}

	// Update the spline to reflect the changes but call a Super function to avoid constructing meshes at the stage
	Super::UpdateSpline();

	// Set the mobility to static by default because the meshes are static
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

bool USplineMeshesComponent::ConstructMeshesAlongSpline()
{
	// Don't do anything if SplineMeshComponentClass isn't set
	if (!IsValid(SplineMeshComponentClass) || !SplineMeshComponentClass->IsAsset())
	{
		return false;
	}

#if DO_ENSURE
	// Make sure the StaticMesh is set
	if (!ensure(SplineMeshComponentClass->GetDefaultObject<USplineMeshComponent>()->GetStaticMesh()))
	{
		return false;
	}
#endif

	// The number of segments on the spline
	const int32 NumSegments = GetNumberOfSplineSegments();

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

	return MeshesAlongSpline.Num() > 0;
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