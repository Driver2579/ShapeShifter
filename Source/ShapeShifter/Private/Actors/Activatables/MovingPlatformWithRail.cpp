// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Activatables/MovingPlatformWithRail.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

void AMovingPlatformWithRail::BeginPlay()
{
	Super::BeginPlay();

	ConstructRail();
}

void AMovingPlatformWithRail::OnConstruction(const FTransform& Transform)
{
	// Clean old rails
	for (USplineMeshComponent* Rail : Rails)
	{
		Rail->DestroyComponent();
	}
	Rails.Reset(); 
	
	// Build new rail
	ConstructRail();
}

void AMovingPlatformWithRail::ConstructRail()
{
	USplineComponent* Spline = MovementDirectionSplineComponent;
	
	const int32 NumSegments = Spline->GetNumberOfSplinePoints() - 1;

	// Create a mesh for each segment on the spline
	for (int32 i = 0; i < NumSegments; ++i)
	{
		if (USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this))
		{
			SplineMeshComponent->SetMobility(EComponentMobility::Stationary);

			SplineMeshComponent->SetStaticMesh(RailStaticMesh);

			// Stretch the mouse to the entire segment
			SplineMeshComponent->SetStartAndEnd(
				Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Type::Local),
				Spline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Type::Local),
				Spline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Type::Local),
				Spline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Type::Local)
			);

			SplineMeshComponent->AttachToComponent(Spline, FAttachmentTransformRules::KeepRelativeTransform);
			SplineMeshComponent->RegisterComponent();

			Rails.Add(SplineMeshComponent);
		}
	}

	//RailStartStaticMesh->SetStaticMesh(RailStartAndEndStaticMesh);
	//RailEndStaticMesh->SetStaticMesh(RailStartAndEndStaticMesh);

	//RailStartStaticMesh->SetRelativeLocationAndRotation(
	//	Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Type::Local) -
	//	RailStartScene->GetRelativeLocation() + RailStartStaticMesh->GetRelativeLocation(),

	//	Spline->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::Type::Local) -
	//	RailStartScene->GetRelativeRotation() + RailStartStaticMesh->GetRelativeRotation()
	//	);

	//RailStartScene->SetRelativeLocationAndRotation(
	//	Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Type::Local),
	//	Spline->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::Type::Local)
	//	);
}