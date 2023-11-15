// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NextLevelLoader.generated.h"

UCLASS()
class SHAPESHIFTER_API ANextLevelLoader : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANextLevelLoader();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* LoadLevelTriggerComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnLoadLevelTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditInstanceOnly)
	TSoftObjectPtr<UWorld> LevelToLoad;
};