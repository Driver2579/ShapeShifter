// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Activatables/AutoDoor.h"

#include "Components/BoxComponent.h"

AAutoDoor::AAutoDoor()
{
	OpenTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	OpenTriggerComponent->SetupAttachment(RootComponent);
}

void AAutoDoor::BeginPlay()
{
	Super::BeginPlay();

	OpenTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AAutoDoor::OnOpenTriggerOverlapBegin);
	OpenTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &AAutoDoor::OnOpenTriggerEndOverlap);
}

void AAutoDoor::OnOpenTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Activate();
}

void AAutoDoor::OnOpenTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Deactivate();
}
