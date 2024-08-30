// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MeshesAlongSplineComponent.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

void UMeshesAlongSplineComponent::UpdateSpline()
{
	Super::UpdateSpline();
	ReconstructMeshesAlongSpline();
}

void UMeshesAlongSplineComponent::OnRegister()
{
	Super::OnRegister();
	ReconstructMeshesAlongSpline();
}

void UMeshesAlongSplineComponent::ConstructMeshesAlongSpline()
{
	// Build new meshes
	const int32 NumSegments = this->GetNumberOfSplinePoints() - 1;

	// Create a mesh for each segment on the spline
	for (int32 i = 0; i < NumSegments; ++i)
	{
		if (USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this))
		{
			SplineMeshComponent->SetMobility(EComponentMobility::Stationary);
			SplineMeshComponent->SetStaticMesh(StaticMesh);

			// Stretch the mouse to the entire segment
			SplineMeshComponent->SetStartAndEnd(
				this->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Type::Local),
				this->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Type::Local),
				this->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Type::Local),
				this->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Type::Local)
			);

			SplineMeshComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
			SplineMeshComponent->RegisterComponent();

			MeshesAlongSpline.Add(SplineMeshComponent);
		}
	}
}

void UMeshesAlongSplineComponent::ReconstructMeshesAlongSpline()
{
	// Clean old meshes
	for (USplineMeshComponent* Rail : MeshesAlongSpline)
	{
		Rail->DestroyComponent();
	}
	MeshesAlongSpline.Reset();

	// Build new meshes
	ConstructMeshesAlongSpline();
}
