// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/FloorButton.h"

AFloorButton::AFloorButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMeshComponent->SetupAttachment(RootComponent);

	ButtonMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button Mesh"));
	ButtonMeshComponent->SetupAttachment(RootComponent);

	ButtonTriggerComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button Trigger"));
	ButtonTriggerComponent->SetupAttachment(ButtonMeshComponent);

	// Set ButtonTriggerComponent collision profile to OverlapAllDynamic
	ButtonTriggerComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// Enable ButtonTriggerComponent MultiBodyOverlap
	ButtonTriggerComponent->bMultiBodyOverlap = true;

	// Make ButtonTriggerComponent invisible in editor and game
	ButtonTriggerComponent->SetVisibility(false);
	ButtonTriggerComponent->SetHiddenInGame(true);
}

void AFloorButton::BeginPlay()
{
	Super::BeginPlay();

	/*// Set default Active state
	SetActive(bActive);*/

	StartButtonLocation = ButtonMeshComponent->GetComponentLocation();
	EndButtonLocation = StartButtonLocation - FVector(0, 0, MoveOffset);

	ButtonTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AFloorButton::OnBeginOverlap);
	ButtonTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &AFloorButton::OnEndOverlap);
}

void AFloorButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation;

	if (IsActive())
	{
		NewLocation = FMath::VInterpConstantTo(ButtonMeshComponent->GetComponentLocation(),
			EndButtonLocation, DeltaTime, MoveSpeed);
	}
	else
	{
		NewLocation = FMath::VInterpConstantTo(ButtonMeshComponent->GetComponentLocation(),
			StartButtonLocation, DeltaTime, MoveSpeed);
	}

	ButtonMeshComponent->SetWorldLocation(NewLocation);
}

void AFloorButton::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor != this)
	{
		Activate();
	}
}

void AFloorButton::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	OverlappingActors.Remove(this);

	if (OverlappingActors.Num() == 0)
	{
		Deactivate();
	}
}

void AFloorButton::Activate()
{
	bActive = true;

	SwitchActorsActivation();
}

void AFloorButton::Deactivate()
{
	bActive = false;

	SwitchActorsActivation();
}

bool AFloorButton::IsActive() const
{
	return bActive;
}

void AFloorButton::SwitchActorsActivation() const
{
	// Iterate through ActorsToSwitchActivation
	for (AActor* It : ActorsToSwitchActivation)
	{
		// Cast It to ActorToSwitchActivation to use Activatable interface functions
		IActivatable* ActorToSwitchActivation = Cast<IActivatable>(It);

		// Check if ActorToSwitchActivation is valid
		if (ActorToSwitchActivation)
		{
			// Switch Active state
			ActorToSwitchActivation->SetActive(!ActorToSwitchActivation->IsActive());
		}
	}
}