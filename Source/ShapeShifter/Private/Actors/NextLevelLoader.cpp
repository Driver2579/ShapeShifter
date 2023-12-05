// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/NextLevelLoader.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

ANextLevelLoader::ANextLevelLoader()
{
 	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	LoadLevelTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Load Level Trigger"));
	LoadLevelTriggerComponent->SetupAttachment(RootComponent);

	LoadLevelTriggerComponent->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
}

void ANextLevelLoader::BeginPlay()
{
	Super::BeginPlay();

	LoadLevelTriggerComponent->OnComponentBeginOverlap.AddDynamic(this,
		&ANextLevelLoader::OnLoadLevelTriggerBeginOverlap);
}

void ANextLevelLoader::OnLoadLevelTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!LevelToLoad.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, LevelToLoad);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ANextLevelLoader::OnLoadLevelTriggerBeginOverlap: LevelToLoad is invalid!"));
	}
}