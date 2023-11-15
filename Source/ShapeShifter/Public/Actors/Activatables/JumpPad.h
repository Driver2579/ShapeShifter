// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "JumpPad.generated.h"

class UJumpPadTargetComponent;

UCLASS()
class SHAPESHIFTER_API AJumpPad : public AActor
{
	GENERATED_BODY()

public:
	AJumpPad();

	virtual void Tick(float DeltaTime) override;

	// Get the base mesh
	UStaticMeshComponent* GetMesh() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BaseMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PadMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RotationAxisComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* JumpTriggerComponent;

	// Indicates where the player should land in the level.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UJumpPadTargetComponent* TargetLocationComponent;

private:
	FTimeline AnimationTimeline;

	FTimerHandle JumpTimer;

	FRotator StartRotation;
	FRotator TargetRotation;

	// The velocity that will be added to the required jump 
	FVector ThrowVelocity;

	// Delay before jump
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
	float JumpDelay = 3;

	// The maximum height at which the player will be during the flight
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
	float JumpHeight = 200;

	UPROPERTY(EditAnywhere)
	FRotator RotationOffset;

	// Should display the time dependence of the location on the route
	UPROPERTY(EditAnywhere)
	UCurveFloat* AnimationCurve;

	UFUNCTION()
	void OnJumpTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnJumpTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	void ProgressAnimateTimeline(const float Value) const;
	void OnEndAnimation();

	// Throws an object on ThrowVelocity
	void ThrowObject(UPrimitiveComponent* Object);
};