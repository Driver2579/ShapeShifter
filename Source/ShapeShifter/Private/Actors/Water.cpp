// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Water.h"

#include "Components/BoxComponent.h"
#include "Pawns/BallPawn.h"

AWater::AWater()
{
	WaterZoneComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Water Zone"));
	WaterZoneComponent->SetupAttachment(RootComponent);

	JumpZoneComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Jump Zone"));
	JumpZoneComponent->SetupAttachment(RootComponent);
}

void AWater::BeginPlay()
{
	Super::BeginPlay();

	JumpZoneComponent->OnComponentBeginOverlap.AddDynamic(this, &AWater::OnJumpZoneBeginOverlap);
	JumpZoneComponent->OnComponentEndOverlap.AddDynamic(this, &AWater::OnJumpZoneEndOverlap);
}

void AWater::OnJumpZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABallPawn* BallPawn = Cast<ABallPawn>(OtherActor);

	// We enable BallPawn jumping only if he is in Rubber form now
	if (IsValid(BallPawn) && BallPawn->GetForm() == EBallPawnForm::Rubber)
	{
		BallPawn->SetSwimmingOnWaterSurface(true);
	}
}

void AWater::OnJumpZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABallPawn* BallPawn = Cast<ABallPawn>(OtherActor);

	if (IsValid(BallPawn))
	{
		BallPawn->SetSwimmingOnWaterSurface(false);
	}
}