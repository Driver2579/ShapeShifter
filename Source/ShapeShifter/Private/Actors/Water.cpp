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

#if WITH_EDITOR
	if (!IsValid(EnterSound))
	{
		UE_LOG(LogTemp, Warning, TEXT("AWater::OnJumpZoneBeginOverlap: EnterSound is invalid!"));
	}
#endif

	if (!IsValid(OtherComp))
	{
		UE_LOG(LogTemp, Warning, TEXT("AWater::OnJumpZoneBeginOverlap: OtherComp is invalid!"));

		return;
	}
	
	// Calculate the volume and pitch of sound and then play it at the location of contact
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), EnterSound, OtherComp->GetComponentLocation(),
		FRotator::ZeroRotator, FMath::Min(OtherComp->GetComponentVelocity().Length() /
		MaxVelocitySound, MaxVelocitySound));
}

void AWater::OnJumpZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABallPawn* BallPawn = Cast<ABallPawn>(OtherActor);

	if (IsValid(BallPawn))
	{
		BallPawn->SetOverlappingWaterJumpZone(false);
	}

#if WITH_EDITOR
	if (!IsValid(LeaveSound))
	{
		UE_LOG(LogTemp, Warning, TEXT("AWater::OnJumpZoneEndOverlap: LeaveSound is invalid!"));
	}
#endif

	if (!IsValid(OtherComp))
	{
		UE_LOG(LogTemp, Warning, TEXT("AWater::OnJumpZoneEndOverlap: OtherComp is invalid!"));

		return;
	}

	// Calculate the volume and pitch of sound and then play it at the location of contact
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), LeaveSound, OtherComp->GetComponentLocation(),
		FRotator::ZeroRotator, FMath::Min(OtherComp->GetComponentVelocity().Length() /
		MaxVelocitySound, MaxVelocitySound));
}