// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/Laser.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Pawns/BallPawn.h"

ALaser::ALaser()
{
 	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

	LaserSpawnPointComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Laser spawn point"));
	LaserSpawnPointComponent->SetupAttachment(RootComponent);

	/**
	 * We need it for a proper render when interacting with physics object.
	 * For some reason it's starting to work properly only with this component...
	 */
	LaserSpawnPointComponent->SetTickGroup(TG_PostUpdateWork);

	LaserDirectionComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Laser direction"));
	LaserDirectionComponent->SetupAttachment(LaserSpawnPointComponent);
}

void ALaser::BeginPlay()
{
	Super::BeginPlay();

	SpawnLaserBeams();

	// DrawLaserBeams for the first time to initialize Beams location
	DrawLaserBeams();

	// Set default Active state
	SetActive(bActive);
}

void ALaser::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(KillBallPawnTimer);
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

		// Continue if Beam isn't valid
		if (!IsValid(Beam))
		{
			continue;
		}

		// Initialize beam custom colors
		Beam->SetVariableLinearColor(BeamColorVariableName, BeamColor);
		Beam->SetVariableLinearColor(FireColorVariableName, FireColor);
		Beam->SetVariableLinearColor(RingsColorVariableName, RingsColor);
		Beam->SetVariableLinearColor(HitRingColorVariableName, HitRingColor);

		// Add Beam to Beams array
		Beams.Add(Beam);
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
	// Initialize first BeamStartLocation and BeamDirection
	FVector BeamStartLocation = LaserSpawnPointComponent->GetComponentLocation();
	FVector BeamDirection = LaserDirectionComponent->GetForwardVector();

	// Iterate through all beams in Beams array
	for (int32 i = 0; i < Beams.Num(); ++i)
	{
		// Draw beams until DrawLaserBeamSingle won't return false
		if (!DrawLaserBeamSingle(i, BeamStartLocation, BeamDirection))
		{
			break;
		}
	}
}

bool ALaser::DrawLaserBeamSingle(const int32 CurrentBeamIndex, FVector& BeamStartLocation, FVector& BeamDirection)
{
	// Send error message and return false if CurrentBeamIndex isn't valid
	if (!Beams.IsValidIndex(CurrentBeamIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("ALaser::DrawLaserBeamSingle: CurrentBeamIndex isn't valid!"));

		return false;
	}

	// We should never hit this
	if (!IsValid(Beams[CurrentBeamIndex]))
	{
		UE_LOG(LogTemp, Error, TEXT("ALaser::DrawLaserBeamSingle: Beams array has an invalid element!"));

		return false;
	}

	// Enable current Beam
	Beams[CurrentBeamIndex]->SetVariableBool(SpawnBeamVariableName, true);

	// Set StartLocation of current Beam
	Beams[CurrentBeamIndex]->SetVariableVec3(BeamStartLocationVariableName, BeamStartLocation);

	// Calculate TraceEnd
	const FVector TraceEnd = BeamStartLocation + BeamDirection * MaxBeamLength;

	// Create TraceParams
	FCollisionQueryParams TraceParams;

	// Turn on TraceComplex to use complex collision
	TraceParams.bTraceComplex = true;

	// Get all Actors we need to ignore using tag
	TArray<AActor*> IgnoredActors;
	UGameplayStatics::GetAllActorsWithTag(this, IgnoreActorTagName, IgnoredActors);

	// Add IgnoredActors
	TraceParams.AddIgnoredActors(IgnoredActors);

	// LineTrace to set current beam variables
	FHitResult HitResult;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, BeamStartLocation, TraceEnd,
		LaserCollisionTraceChanel, TraceParams);

	// Calculate NextBeamIndex
	const int NextBeamIndex = CurrentBeamIndex + 1;

	// Stop drawing next reflected beams if we didn't hit anything
	if (!bHit)
	{
		// Set current beam EndLocation
		Beams[CurrentBeamIndex]->SetVariableVec3(BeamEndLocationVariableName, HitResult.TraceEnd);

		// Disable all next beams if NextBeamIndex is valid
		if (Beams.IsValidIndex(NextBeamIndex))
		{
			SetBeamsActive(false, NextBeamIndex);
		}

		// Return false as we don't need to draw next reflected beam
		return false;
	}

	// Set NextBeamDirection with ReflectionVector between BeamDirection and ImpactPoint
	const FVector NextBeamDirection = FMath::GetReflectionVector(BeamDirection, HitResult.ImpactNormal);

	// Calculate BeamsBisectorDirection
	const FVector BeamsBisectorDirection = -(BeamDirection - NextBeamDirection);

	// Set BeamsBisectorDirection to current beam
	Beams[CurrentBeamIndex]->SetVariableVec3(BeamsBisectorDirectionVariableName, BeamsBisectorDirection);

	// Set BeamDirection for next beam
	BeamDirection = NextBeamDirection;

	// Calculate next BeamStartLocation
	BeamStartLocation = HitResult.Location + NextBeamDirection * BeamDistanceToImpactPoint;

	// Set current beam EndLocation with next beam StartLocation
	Beams[CurrentBeamIndex]->SetVariableVec3(BeamEndLocationVariableName, BeamStartLocation);

	// Get HitActor from HitResult
	AActor* HitActor = HitResult.GetActor();

	// Remember if we can reflect HitActor or not
	const bool bReflectActor = IsValid(HitActor) && HitActor->ActorHasTag(ReflectActorTagName);

	/**
	 * Disable BeamEnd in current beam if we have to reflect all or we hit an Actor which can reflect laser and return
	 * true to draw next reflected beams
	 */
	if (bReflectAll || bReflectActor)
	{
		Beams[CurrentBeamIndex]->SetVariableBool(SpawnBeamEndVariableName, false);

		// Call OnLaserHit with bReflected true as HitActor reflected Laser
		OnLaserHit(HitActor, true);

		return true;
	}

	// Enable BeamEnd in current beam because there won't be any next reflected beams 
	Beams[CurrentBeamIndex]->SetVariableBool(SpawnBeamEndVariableName, true);

	// Disable all next beams if NextBeamIndex is valid
	if (Beams.IsValidIndex(NextBeamIndex))
	{
		SetBeamsActive(false, NextBeamIndex);
	}

	// Call OnLaserHit with bReflected false as this HitActor blocked Laser
	OnLaserHit(HitActor, false);

	// Return false to stop drawing next reflected beams
	return false;
}

void ALaser::OnLaserHit(AActor* HitActor, const bool bReflected)
{
	// We can't hit anything if Laser isn't active or HitActor isn't valid
	if (!bActive || !IsValid(HitActor))
	{
		return;
	}

	ABallPawn* BallPawn = Cast<ABallPawn>(HitActor);

	// Kill BallPawn only if it don't reflects or ignores the Laser
	if (!IsValid(BallPawn) || BallPawn->ActorHasTag(ReflectActorTagName) || BallPawn->ActorHasTag(IgnoreActorTagName))
	{
		return;
	}

	// Kill BallPawn immediately if delay is 0
	if (KillBallPawnTime == 0)
	{
		BallPawn->Die();
	}
	// Kill BallPawn with delay in another case
	else if (!GetWorldTimerManager().IsTimerActive(KillBallPawnTimer))
	{
		GetWorldTimerManager().SetTimer(KillBallPawnTimer, BallPawn, &ABallPawn::Die, KillBallPawnTime,
			false);
	}
}

void ALaser::SetBeamsActive(const bool bNewActive, const int32 FirstBeamIndex)
{
	if (!HasActorBegunPlay())
	{
		return;
	}

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

	SetBeamsActive(bActive);
}

void ALaser::Deactivate()
{
	bActive = false;

	SetBeamsActive(bActive);
}

void ALaser::KillBallPawn(ABallPawn* BallPawnToKill)
{
	if (IsValid(BallPawnToKill))
	{
		BallPawnToKill->Die();
	}
}
