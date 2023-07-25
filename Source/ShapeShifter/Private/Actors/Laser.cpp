// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Laser.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/ArrowComponent.h"
#include "ShapeShifter/ShapeShifter.h"

ALaser::ALaser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

	LaserSpawnPointComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Laser spawn point"));
	LaserSpawnPointComponent->SetupAttachment(RootComponent);

	LaserDirectionComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Laser direction"));
	LaserDirectionComponent->SetupAttachment(LaserSpawnPointComponent);
}

void ALaser::BeginPlay()
{
	Super::BeginPlay();

	// Set default Active state
	SetActive(bActive);

	SpawnLaserBeams();
}

void ALaser::SpawnLaserBeams()
{
	if (!IsValid(NiagaraSystemTemplate))
	{
		UE_LOG(LogTemp, Error, TEXT("ALaser::SpawnLaserBeams: NiagaraSystemTemplate isn't valid!"));

		return;
	}

	// Create Beams SpawnParameters
	FFXSystemSpawnParameters SpawnParameters;

	// Initialize Beams SpawnParameters
	SpawnParameters.SystemTemplate = NiagaraSystemTemplate;
	SpawnParameters.AttachToComponent = LaserSpawnPointComponent;
	SpawnParameters.Location = LaserSpawnPointComponent->GetComponentLocation();
	SpawnParameters.LocationType = EAttachLocation::SnapToTarget;
	SpawnParameters.bAutoDestroy = false;

	for (int i = 0; i < BeamsCount; ++i)
	{
		// Spawn Beam
		UNiagaraComponent* Beam = UNiagaraFunctionLibrary::SpawnSystemAttachedWithParams(SpawnParameters);

		// Add Beam to Beams array if it's valid
		if (IsValid(Beam))
		{
			Beams.Add(Beam);
		}
	}
}

void ALaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bActive)
	{
		DrawLaserBeams();
	}
}

void ALaser::DrawLaserBeams()
{
	FVector BeamStartLocation = LaserSpawnPointComponent->GetComponentLocation();
	FVector BeamDirection = LaserDirectionComponent->GetForwardVector();

	bool bHit = true;

	for (int i = 0; i < Beams.Num(); ++i)
	{
		// We should never hit this
		if (!IsValid(Beams[i]))
		{
			UE_LOG(LogTemp, Error, TEXT("ALaser::DrawLaserBeams: Beams array has an invalid element!"));

			return;
		}

		if (!bHit)
		{
			break;
		}

		// Enable current Beam
		Beams[i]->SetVariableBool(SpawnBeamVariableName, true);

		// Set StartLocation of current Beam
		Beams[i]->SetVariableVec3(BeamStartLocationVariableName, BeamStartLocation);

		// Calculate TraceEnd
		const FVector TraceEnd = BeamStartLocation + BeamDirection * MaxBeamLength;

		// Create TraceParams and turn on TraceComplex
		FCollisionQueryParams TraceParams;
		TraceParams.bTraceComplex = true;

		// LineTrace to set current beam variables
		FHitResult HitResult;
		bHit = GetWorld()->LineTraceSingleByChannel(HitResult, BeamStartLocation, TraceEnd, ECC_LASER_BEAM,
			TraceParams);

		if (bHit)
		{
			// Set NextBeamDirection with ReflectionVector between BeamDirection and ImpactPoint
			const FVector NextBeamDirection = FMath::GetReflectionVector(BeamDirection, HitResult.ImpactNormal);

			// Calculate BeamsBisectorDirection
			const FVector BeamsBisectorDirection = -(BeamDirection - NextBeamDirection);

			// Set BeamsBisectorDirection to current beam
			Beams[i]->SetVariableVec3(BeamsBisectorDirectionVariableName, BeamsBisectorDirection);

			// Set BeamDirection for next beam
			BeamDirection = NextBeamDirection;

			// Calculate next BeamStartLocation
			BeamStartLocation = HitResult.Location + NextBeamDirection * BeamDistanceToImpactPoint;

			// Set current beam EndLocation with next beam StartLocation
			Beams[i]->SetVariableVec3(BeamEndLocationVariableName, BeamStartLocation);
		}
		else
		{
			// Set current beam EndLocation
			Beams[i]->SetVariableVec3(BeamEndLocationVariableName, HitResult.TraceEnd);

			// Calculate NextBeamIndex
			const int NextBeamIndex = i + 1;

			// Disable all next beams if NextBeamIndex is valid
			if (Beams.IsValidIndex(NextBeamIndex))
			{
				SetBeamsActive(false, i + 1);
			}
		}
	}
}

void ALaser::SetBeamsActive(const bool bNewActive, const int32 FirstBeamIndex)
{
	if (!Beams.IsValidIndex(FirstBeamIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("ALaser::SetBeamsActive: FirstBeamIndex isn't valid!"));

		return;
	}

	for (int i = FirstBeamIndex; i < Beams.Num(); ++i)
	{
		// We should never hit this
		if (!IsValid(Beams[i]))
		{
			UE_LOG(LogTemp, Error, TEXT("ALaser::SetBeamsActive: Beams array has an invalid element!"));

			return;
		}

		// Set new active state to current beam
		Beams[i]->SetVariableBool(SpawnBeamVariableName, bNewActive);
		Beams[i]->SetVariableBool(SpawnBeamEndVariableName, bNewActive);
	}
}

bool ALaser::IsActive() const
{
	return bActive;
}

void ALaser::Activate()
{
	bActive = true;
}

void ALaser::Deactivate()
{
	bActive = false;
}