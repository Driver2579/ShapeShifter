// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/ActivationSwitchers/FloorButton.h"

#include "Components/AudioComponent.h"

AFloorButton::AFloorButton()
{
 	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	SetupComponents();
}

void AFloorButton::SetupComponents()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMeshComponent->SetupAttachment(RootComponent);

	ButtonMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button Mesh"));
	ButtonMeshComponent->SetupAttachment(RootComponent);

	ButtonTriggerComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button Trigger"));
	ButtonTriggerComponent->SetupAttachment(ButtonMeshComponent);

	ActivateAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Activate Audio"));
	ActivateAudioComponent->SetupAttachment(RootComponent);

	DeactivateAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Deactivate Audio"));
	DeactivateAudioComponent->SetupAttachment(RootComponent);

	// Set ButtonTriggerComponent collision profile to TriggerOnlyPawn
	ButtonTriggerComponent->SetCollisionProfileName(TEXT("TriggerOnlyPawn"));

	// Enable ButtonTriggerComponent MultiBodyOverlap
	ButtonTriggerComponent->bMultiBodyOverlap = true;

	// Make ButtonTriggerComponent invisible in game
	ButtonTriggerComponent->SetHiddenInGame(true);
}

void AFloorButton::BeginPlay()
{
	Super::BeginPlay();

	ButtonMeshInactiveLocation = ButtonMeshComponent->GetRelativeLocation();
	ButtonMeshActiveLocation = ButtonMeshInactiveLocation - FVector(0, 0, MoveOffset);

	ButtonTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AFloorButton::OnButtonTriggerBeginOverlap);
	ButtonTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &AFloorButton::OnButtonTriggerEndOverlap);
}

void AFloorButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Get ButtonMeshTargetLocation by bActive state
	const FVector& ButtonMeshTargetLocation = bActive ? ButtonMeshActiveLocation : ButtonMeshInactiveLocation;

	if (ButtonMeshComponent->GetRelativeLocation().Equals(ButtonMeshTargetLocation))
	{
		return;
	}

	const FVector& NewLocation = FMath::VInterpConstantTo(ButtonMeshComponent->GetRelativeLocation(),
		ButtonMeshTargetLocation, DeltaTime, MoveSpeed);

	ButtonMeshComponent->SetRelativeLocation(NewLocation);
}

void AFloorButton::OnButtonTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor != this)
	{
		Activate();
	}
}

void AFloorButton::OnButtonTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// We use TSet here instead of TArray because GetOverlappingActors with TSet overload is better for performance
	TSet<AActor*> OverlappingActors;
	ButtonTriggerComponent->GetOverlappingActors(OverlappingActors);

	OverlappingActors.Remove(this);

	if (OverlappingActors.Num() == 0)
	{
		Deactivate();
	}
}

bool AFloorButton::IsActive() const
{
	return bActive;
}

void AFloorButton::Activate()
{
	// We don't need to do anything if it's already active
	if (bActive)
	{
		return;
	}

	bActive = true;

	SwitchActorsActivation();

	ActivateAudioComponent->Play();
	DeactivateAudioComponent->Stop();
}

void AFloorButton::Deactivate()
{
	// We don't need to do anything if it's already inactive
	if (!bActive)
	{
		return;
	}

	bActive = false;

	SwitchActorsActivation();

	ActivateAudioComponent->Stop();
	DeactivateAudioComponent->Play();
}