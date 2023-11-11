// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Jumppad.generated.h"

UCLASS()
class SHAPESHIFTER_API AJumppad : public AActor
{
	GENERATED_BODY()

public:	
	AJumppad();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BaseMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PadMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* TriggerComponent;

	// Displays where the player should jump in the level.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* TargetLocation;

	// Delay before jump
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0))
	float JumpDelay = 3;

	// The maximum height at which the player will be during the flight
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0))
	float JumpHeight = 200;

	UFUNCTION()
	void OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

private:
	FTimerHandle JumpTimer;
	FVector InitialVelocity = FVector::ZeroVector;
};