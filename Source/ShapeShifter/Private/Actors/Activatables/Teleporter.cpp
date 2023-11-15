// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/Teleporter.h"

#include "NiagaraComponent.h"
#include "Pawns/BallPawn.h"

ATeleporter::ATeleporter()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

	TeleportTriggerComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Teleport Trigger"));
	TeleportTriggerComponent->SetupAttachment(RootComponent);

	// Set TeleportTriggerComponent collision profile to OverlapOnlyPawn
	TeleportTriggerComponent->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	// Enable TeleportTriggerComponent MultiBodyOverlap
	TeleportTriggerComponent->bMultiBodyOverlap = true;

	// Make TeleportTriggerComponent invisible in editor and game
	TeleportTriggerComponent->SetVisibility(false);
	TeleportTriggerComponent->SetHiddenInGame(true);

	NiagaraParticlesComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara Particles"));
	NiagaraParticlesComponent->SetupAttachment(RootComponent);

	TeleportPointComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Teleport Point"));
	TeleportPointComponent->SetupAttachment(RootComponent);
}

void ATeleporter::BeginPlay()
{
	Super::BeginPlay();

	// Set default Active state
	SetActive(bActive);

	TeleportTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ATeleporter::OnTeleportTriggerBeginOverlap);
	TeleportTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &ATeleporter::OnTeleportTriggerEndOverlap);
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

	// Teleport BallPawn to TeleportPointComponent of OtherTeleporter
	BallPawn->SetActorLocation(OtherTeleporter->TeleportPointComponent->GetComponentLocation());
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

bool ATeleporter::IsActive() const
{
	return bActive;
}

void ATeleporter::Activate()
{
	bActive = true;

	// Activate NiagaraParticlesComponent
	NiagaraParticlesComponent->SetVisibility(true);

	// Get ChildrenComponents of NiagaraParticlesComponent
	TArray<USceneComponent*> ChildrenComponents;
	NiagaraParticlesComponent->GetChildrenComponents(true, ChildrenComponents);

	// Activate ChildrenComponents of NiagaraParticlesComponent
	for (USceneComponent* It : ChildrenComponents)
	{
		if (IsValid(It) && It->IsA<UNiagaraComponent>())
		{
			It->SetVisibility(true);
		}
	}
}

void ATeleporter::Deactivate()
{
	bActive = false;

	// Deactivate NiagaraParticlesComponent
	NiagaraParticlesComponent->SetVisibility(false);

	// Get ChildrenComponents of NiagaraParticlesComponent
	TArray<USceneComponent*> ChildrenComponents;
	NiagaraParticlesComponent->GetChildrenComponents(true, ChildrenComponents);

	// Deactivate ChildrenComponents of NiagaraParticlesComponent
	for (USceneComponent* It : ChildrenComponents)
	{
		if (IsValid(It) && It->IsA<UNiagaraComponent>())
		{
			It->SetVisibility(false);
		}
	}
}