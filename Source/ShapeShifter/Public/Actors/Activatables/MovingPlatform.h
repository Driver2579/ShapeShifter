// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Activatable.h"
#include "Components/TimelineComponent.h"
#include "MovingPlatform.generated.h"

UCLASS()
class SHAPESHIFTER_API AMovingPlatform : public AActor, public IActivatable
{
	GENERATED_BODY()
	
public:
	AMovingPlatform();

	virtual void Tick(float DeltaTime) override;


	virtual bool IsActive() const override final;

	virtual void Activate() override;
	virtual void Deactivate() override;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	USceneComponent* SceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USplineComponent* MovementDirectionSplineComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;
	
	// Should display the dependence of location on the route on time
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	UCurveFloat* MovementCurve;

	// Time it takes the platform to complete the route
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.01"))
	float MoveTime = 1;

	// Delay before starting to move
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0"))
	float StartDelay = 0;

	// Delay before starting to move back
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0"))
	float EndDelay = 0;

	// Whether the platform will turn towards the route or not
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRotate = false;

	// Infinite movement when activated and stop in place when deactivated
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bLoop = false;
	
	UFUNCTION()
	void ProcessMovementTimeline(const float Value);

private:
	UPROPERTY(EditAnywhere, Category = "Activation")
	bool bActive = false;
	
	FTimeline MovementTimeline;
	FTimerHandle MoveTimer;
};