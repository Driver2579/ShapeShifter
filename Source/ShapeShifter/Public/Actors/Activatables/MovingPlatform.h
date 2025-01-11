// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Activatable.h"
#include "Interfaces/Savable.h"
#include "Components/TimelineComponent.h"
#include "MovingPlatform.generated.h"

class UShapeShifterSaveGame;

UCLASS()
class SHAPESHIFTER_API AMovingPlatform : public AActor, public IActivatable, public ISavable
{
	GENERATED_BODY()
	
public:
	AMovingPlatform();

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	virtual void Tick(float DeltaTime) override;

	virtual bool IsActive() const override final;

	virtual void Activate() override;
	virtual void Deactivate() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USplineMeshesWithEdgesComponent* MovementDirectionSplineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/**
	 * Attach collision components to this one. They have to repeat collision of MeshComponent but a little smaller than
	 * the original one. This is needed to check if BallPawn is colliding with MovingPlatform, and if yes than an
	 * overlapping BallPawn will be killed. You can attach to this component as many collisions as you want.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* CollisionAttachPointComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* ActivateAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* DeactivateAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AmbientAudioComponent;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	virtual void OnCollisionComponentsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnSaveGame(UShapeShifterSaveGame* SaveGameObject) override;
	virtual void OnLoadGame(UShapeShifterSaveGame* SaveGameObject) override;

	/**
	 * Handles the movement of the platform along the spline.
	 * @param Value Position of the platform on the spline (from 0 to 1).
	 */
	UFUNCTION()
	void ProcessMovementTimeline(const float Value);

private:
	// Add OnComponentBeginOverlap delegate subscription to all attached components of CollisionAttachPointComponent
	void SetupCollisionComponents() const;

	UFUNCTION()
	void OnMovementTimelineEvent();
	
	UPROPERTY(EditAnywhere, Category = "Activation")
	bool bActive = false;

	bool bHasEverSwitchedActivation = false;

	// Stores the initial relative mesh's position 
	FVector MeshComponentInitialRelativeLocation;

	// Stores the initial relative mesh's rotation
	FRotator MeshComponentInitialRelativeRotation;

#if WITH_EDITOR
	// Rotation that was set before bRotate was enabled
	FRotator* MeshComponentRelativeRotationBeforeRotateEnabled;
#endif

	// Time it takes the platform to complete the route
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.01))
	float MoveTime = 1;

	/**
	 * Map where:
	 *	The key is the time when the delay starts;
	 *	The value is how long the delay lasts.
	 */
	UPROPERTY(EditAnywhere)
	TMap<float, float> DelaysMap;

	// Whether the platform will turn towards the route or not
	UPROPERTY(EditAnywhere)
	bool bRotate = false;

	// Infinite movement when activated and stop in place when deactivated
	UPROPERTY(EditAnywhere)
	bool bLoop = false;

	// Should display the time dependence of the location on the route
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<UCurveFloat> MovementCurve;

	FTimeline MovementTimeline;

	FTimerHandle MoveTimer;
};