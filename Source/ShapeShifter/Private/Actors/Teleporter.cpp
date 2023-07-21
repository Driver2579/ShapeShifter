// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Teleporter.h"

#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Pawns/BallPawn.h"

ATeleporter::ATeleporter()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

	TeleportTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Teleport Trigger"));
	TeleportTrigger->SetupAttachment(RootComponent);

	NiagaraParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara Particles"));
	NiagaraParticles->SetupAttachment(RootComponent);

	TeleportPoint = CreateDefaultSubobject<USceneComponent>(TEXT("TeleportPoint"));
	TeleportPoint->SetupAttachment(RootComponent);
}

void ATeleporter::BeginPlay()
{
	Super::BeginPlay();

	// Set default Active state
	SetActive(bActive);

	TeleportTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATeleporter::OnTeleportTriggerBeginOverlap);
	TeleportTrigger->OnComponentEndOverlap.AddDynamic(this, &ATeleporter::OnTeleportTriggerEndOverlap);
}

void ATeleporter::OnTeleportTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// We can't teleport if Teleporter isn't active
	if (!IsActive())
	{
		return;
	}

	// Cast OtherActor to ABallPawn
	ABallPawn* BallPawn = Cast<ABallPawn>(OtherActor);

	// Return if BallPawn isn't valid or if BallPawn was just teleported here
	if (!IsValid(BallPawn) || JustTeleportedBallPawns.Contains(BallPawn) || !IsOtherTeleporterValid())
	{
		return;
	}

	// Add BallPawn to JustTeleportedBallPawns in OtherTeleporter to avoid infinite teleporting
	OtherTeleporter->JustTeleportedBallPawns.Add(BallPawn);

	// Clear BallPawn velocity
	BallPawn->GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

	// Teleport BallPawn to TeleportPoint of OtherTeleporter
	BallPawn->SetActorLocation(OtherTeleporter->TeleportPoint->GetComponentLocation());
}

void ATeleporter::OnTeleportTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Cast OtherActor to ABallPawn
	ABallPawn* BallPawn = Cast<ABallPawn>(OtherActor);

	// Remove BallPawn from JustTeleportedBallPawns to allow him teleport through this Teleporter
	if (IsValid(BallPawn))
	{
		JustTeleportedBallPawns.Remove(BallPawn);
	}
}

bool ATeleporter::IsOtherTeleporterValid() const
{
	if (!IsValid(OtherTeleporter))
	{
		UE_LOG(LogTemp, Error, TEXT("ATeleporter::IsOtherTeleporterValid: OtherTeleporter is invalid!"));

		return false;
	}

	if (OtherTeleporter == this)
	{
		UE_LOG(LogTemp, Error, TEXT("ATeleporter::IsOtherTeleporterValid: OtherTeleporter can't equal this pointer!"));

		return false;
	}

	return true;
}

void ATeleporter::Activate()
{
	bActive = true;

	NiagaraParticles->Activate();

	TArray<USceneComponent*> ChildrenComponents;
	NiagaraParticles->GetChildrenComponents(true, ChildrenComponents);

	for (USceneComponent* It : ChildrenComponents)
	{
		if (IsValid(It) && It->IsA<UNiagaraComponent>())
		{
			It->Activate();
		}
	}
}

void ATeleporter::Deactivate()
{
	bActive = false;

	NiagaraParticles->Deactivate();

	TArray<USceneComponent*> ChildrenComponents;
	NiagaraParticles->GetChildrenComponents(true, ChildrenComponents);

	for (USceneComponent* It : ChildrenComponents)
	{
		if (IsValid(It) && It->IsA<UNiagaraComponent>())
		{
			It->Deactivate();
		}
	}
}

bool ATeleporter::IsActive() const
{
	return bActive;
}