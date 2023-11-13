// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/ActivationSwitchers/Lever.h"

#include "Actors/SaveGameManager.h"
#include "Components/BoxComponent.h"
#include "GameModes/ShapeShifterGameMode.h"
#include "Objects/ShapeShifterSaveGame.h"

ALever::ALever()
{
 	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMeshComponent->SetupAttachment(RootComponent);

	LeverMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lever Mesh"));
	LeverMeshComponent->SetupAttachment(RootComponent);

	// Enable Hit Events
	LeverMeshComponent->SetNotifyRigidBodyCollision(true);

	ActivateZoneComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Activate Zone"));
	ActivateZoneComponent->SetupAttachment(LeverMeshComponent);

	DeactivateZoneComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Deactivate Zone"));
	DeactivateZoneComponent->SetupAttachment(LeverMeshComponent);
}

void ALever::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetLeverMeshRotationByActive();
}

void ALever::BeginPlay()
{
	Super::BeginPlay();

	SetupSaveLoadDelegates();

	LeverMeshComponent->OnComponentHit.AddDynamic(this, &ALever::OnLeverMeshHit);
}

void ALever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Return to safe performance if we don't need to rotate LeverMeshComponent
	if (!bRotateLeverMesh)
	{
		return;
	}

	// Get LeverMeshTargetRotation by bActive state
	const FRotator& LeverMeshTargetRotation = bActive ? LeverMeshActiveRotation : LeverMeshInactiveRotation;

	const FRotator& NewRotation = FMath::RInterpTo(LeverMeshComponent->GetRelativeRotation(),
		LeverMeshTargetRotation, DeltaTime, RotationSpeed);

	LeverMeshComponent->SetRelativeRotation(NewRotation);

	// We need this because the default Tolerance in Equals function is too small and it could fail sometimes
	const float Tolerance = 0.001;

	if (LeverMeshComponent->GetRelativeRotation().Equals(LeverMeshTargetRotation, Tolerance))
	{
		bRotateLeverMesh = false;
	}
}

void ALever::SetupSaveLoadDelegates()
{
	const AShapeShifterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShapeShifterGameMode>();

	if (!IsValid(GameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("ALever::SetupSaveLoadDelegates: Failed to get GameMode!"));

		return;
	}

	ASaveGameManager* SaveGameManager = GameMode->GetSaveGameManager();

	if (!IsValid(SaveGameManager))
	{
		UE_LOG(LogTemp, Error, TEXT("ALever::SetupSaveLoadDelegates: SaveGameManager is invalid!"));

		return;
	}

	SaveGameManager->OnSaveGame.AddDynamic(this, &ALever::OnSaveGame);
	SaveGameManager->OnLoadGame.AddDynamic(this, &ALever::OnLoadGame);
}

void ALever::OnSaveGame(UShapeShifterSaveGame* SaveGameObject)
{
	if (!IsValid(SaveGameObject))
	{
		UE_LOG(LogTemp, Error, TEXT("ALever::OnSaveGame: SaveGameObject is invalid!"));

		return;
	}

	SaveGameObject->LeverSaveData.Add(GetName(), bActive);
}

void ALever::OnLoadGame(UShapeShifterSaveGame* SaveGameObject)
{
	if (!IsValid(SaveGameObject))
	{
		UE_LOG(LogTemp, Error, TEXT("ALever::OnSaveGame: SaveGameObject is invalid!"));

		return;
	}

	// Get saved bActive by Lever unique name
	const bool* SavedActive = SaveGameObject->LeverSaveData.Find(GetName());

	if (SavedActive == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ALever::OnLoadGame: SavedActive is invalid for %s"), *GetName());

		return;
	}

	// SetActive using SavedActive
	SetActive(*SavedActive);

	// Skip rotation animation
	SetLeverMeshRotationByActive();
}

void ALever::SetLeverMeshRotationByActive() const
{
	if (bActive)
	{
		LeverMeshComponent->SetRelativeRotation(LeverMeshActiveRotation);
	}
	else
	{
		LeverMeshComponent->SetRelativeRotation(LeverMeshInactiveRotation);
	}
}

void ALever::OnLeverMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// We don't need to handle OtherComps without physics
	if (!IsValid(OtherComp) || !OtherComp->IsSimulatingPhysics())
	{
		return;
	}

	// Get ZVelocity of OtherComp
	const float ZVelocity = OtherComp->GetComponentVelocity().Z;

	// Activate the Lever if OtherComp hit the top of it
	if (ZVelocity > VelocityToSwitchActivation && OtherComp->IsOverlappingComponent(ActivateZoneComponent))
	{
		Activate();
	}
	// Deactivate the Lever if OtherComp hit the bottom of it
	else if (ZVelocity < -VelocityToSwitchActivation && OtherComp->IsOverlappingComponent(DeactivateZoneComponent))
	{
		Deactivate();
	}
}

bool ALever::IsActive() const
{
	return bActive;
}

void ALever::Activate()
{
	// We don't need to do anything if Lever is already active
	if (bActive)
	{
		return;
	}

	bActive = true;
	OnActiveSwitch();
}

void ALever::Deactivate()
{
	// We don't need to do anything if Lever is already inactive
	if (!bActive)
	{
		return;
	}

	bActive = false;
	OnActiveSwitch();
}

void ALever::OnActiveSwitch()
{
	// Turn on LeverMeshComponent rotation because activation was switched
	bRotateLeverMesh = true;

	SwitchActorsActivation();
}