// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Jumppad.generated.h"

UCLASS()
class SHAPESHIFTER_API AJumpPad : public AActor
{
	GENERATED_BODY()

public:	
	AJumpPad();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* BaseMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PadMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* JumpTriggerComponent;

	// Indicates where the player should land in the level.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* TargetLocationComponent;

private:
	FTimerHandle JumpTimer;

	// The velocity that will be added to the required jump 
	FVector InitialVelocity = FVector::ZeroVector;

	// Delay before jump
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
	float JumpDelay = 3;

	// The maximum height at which the player will be during the flight
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
	float JumpHeight = 200;

	UFUNCTION()
	void OnJumpBeginTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnJumpTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};