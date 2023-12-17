// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Water.h"

#include "Components/BoxComponent.h"
#include "Pawns/BallPawn.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AWater::AWater()
{
	WaterZoneComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Water Zone"));
	WaterZoneComponent->SetupAttachment(RootComponent);

	JumpZoneComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Jump Zone"));
	JumpZoneComponent->SetupAttachment(RootComponent);

	AmbientAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Ambient Audio"));
	AmbientAudioComponent->SetupAttachment(RootComponent);
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

	if (IsValid(BallPawn))
	{
		BallPawn->SetOverlappingWaterJumpZone(true);
	}

	const UAudioComponent* SpawnedSound = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), EnterSound,
		OtherComp->GetComponentLocation(), FRotator::ZeroRotator, FMath::Min(
		OtherComp->GetComponentVelocity().Length() / MaxVelocitySound, MaxVelocitySound));

	if (!IsValid(SpawnedSound))
	{
		UE_LOG(LogTemp, Warning, TEXT("AWater::OnJumpZoneBeginOverlap: SpawnedSound is invalid!"));
	}
}

void AWater::OnJumpZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABallPawn* BallPawn = Cast<ABallPawn>(OtherActor);

	if (IsValid(BallPawn))
	{
		BallPawn->SetOverlappingWaterJumpZone(false);
	}

	const UAudioComponent* SpawnedSound = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), LeaveSound,
		OtherComp->GetComponentLocation(), FRotator::ZeroRotator, FMath::Min(
		OtherComp->GetComponentVelocity().Length() / MaxVelocitySound, MaxVelocitySound));
	
	if (!IsValid(SpawnedSound))
	{
		UE_LOG(LogTemp, Warning, TEXT("AWater::OnJumpZoneEndOverlap: SpawnedSound is invalid!"));
	}
}