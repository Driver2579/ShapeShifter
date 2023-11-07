// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Activatables/AutoDoor.h"

#include "Components/BoxComponent.h"

AAutoDoor::AAutoDoor()
{
	TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	TriggerComponent->SetupAttachment(RootComponent);

	TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AAutoDoor::OnTriggerOverlapBegin);
	TriggerComponent->OnComponentEndOverlap.AddDynamic(this, &AAutoDoor::OnTriggerEndBegin);
}

void AAutoDoor::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Activate();
}

void AAutoDoor::OnTriggerEndBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Deactivate();
}
