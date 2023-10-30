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

	virtual void Activate() override;
	virtual void Deactivate() override;
	virtual bool IsActive() const override;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	USceneComponent* SceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovingPlatform")
	class USplineComponent* MovementDirectionSplineComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MovingPlatform")
	UStaticMeshComponent* MeshComponent;
	
	/**
	 * Should display the dependence of location on the route on time
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovingPlatform")
	UCurveFloat* MovementCurve;

	// Time it takes the platform to complete the route
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovingPlatform", meta = (ClampMin = "0"))
	float MoveTime = 1;

	// Time it takes the platform to complete the route
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovingPlatform", meta = (ClampMin = "0"))
	float StartDelay = 0;

	// Delay before starting to move back
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovingPlatform", meta = (ClampMin = "0"))
	float EndDelay = 0;

	// Should the platform be turned towards the route
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovingPlatform")
	bool bRotate = false;

	// Infinite movement when activated and stop in place when deactivated
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovingPlatform")
	bool bLoop = false;

	UFUNCTION()
	void ProcessMovementTimeline(const float Value);

private:
	bool bIsActive = false;
	FTimeline MovementTimeline;
	FTimerHandle MoveTimer;
};