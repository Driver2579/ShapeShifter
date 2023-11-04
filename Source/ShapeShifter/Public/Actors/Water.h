// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WaterBodyCustomActor.h"
#include "Water.generated.h"

class UBoxComponent;

UCLASS()
class SHAPESHIFTER_API AWater : public AWaterBodyCustom
{
	GENERATED_BODY()

public:
	AWater();

protected:
	// Zone where water work
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* WaterZoneComponent;

	// Zone where player is allowed to jump
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* JumpZoneComponent;

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnJumpZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnJumpZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};