// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Activatable.h"
#include "Laser.generated.h"

UCLASS()
class SHAPESHIFTER_API ALaser : public AActor, public IActivatable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALaser();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool IsActive() const override;

	virtual void Activate() override;
	virtual void Deactivate() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* LaserSpawnPointComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UArrowComponent* LaserDirectionComponent;

	/**
	 * Called when Laser beam hit any not ignored Actor. This function called every tick while hitting Actor.
	 * @param HitActor Hit Actor by Laser beam.
	 * @param bReflected Whether HitActor reflected Laser beam or not.
	 */
	virtual void OnLaserHit(AActor* HitActor, const bool bReflected);

private:
	UPROPERTY(EditAnywhere, Category = "Activation")
	bool bActive = true;

	UPROPERTY(EditAnywhere, Category = "Laser")
	class UNiagaraSystem* NiagaraSystemTemplate;

	UPROPERTY(EditAnywhere, Category = "Laser color")
	FLinearColor BeamColor = FLinearColor(80.000000, 0.000000, 0.000000);

	UPROPERTY(EditAnywhere, Category = "Laser color")
	FLinearColor FireColor = FLinearColor(100.000000, 0.000000, 0.000000);

	UPROPERTY(EditAnywhere, Category = "Laser color")
	FLinearColor RingsColor = FLinearColor(20.000000, 0.000000, 0.000000, 1.000000);

	UPROPERTY(EditAnywhere, Category = "Laser color")
	FLinearColor HitRingColor = FLinearColor(20.000000, 0.000000, 0.000000, 1.000000);

	UPROPERTY(EditDefaultsOnly, Category = "Laser Niagara variables")
	FName BeamColorVariableName = TEXT("BeamColor");

	UPROPERTY(EditDefaultsOnly, Category = "Laser Niagara variables")
	FName FireColorVariableName = TEXT("FireColor");

	UPROPERTY(EditDefaultsOnly, Category = "Laser Niagara variables")
	FName RingsColorVariableName = TEXT("RingsColor");

	UPROPERTY(EditDefaultsOnly, Category = "Laser Niagara variables")
	FName HitRingColorVariableName = TEXT("HitRingColor");

	void SpawnLaserBeams();

	// Amount of beams to spawn and the maximum of reflections
	UPROPERTY(EditAnywhere, Category = "Reflections", meta = (ClampMin = 0))
	int32 BeamsCount = 10;

	UPROPERTY(EditAnywhere, Category = "Reflections", meta = (ClampMin = 1))
	float MaxBeamLength = 100000;

	// If true laser will be reflected from all surfaces. If false laser will be reflected only from BallPawn Metal form
	UPROPERTY(EditAnywhere, Category = "Reflections")
	bool bReflectAll = false;

	// Distance between beam end location and impact point
	UPROPERTY(EditAnywhere, Category = "Reflections", meta = (ClampMin = 0.1, ClampMax = 1))
	float BeamDistanceToImpactPoint = 0.2;

	TArray<class UNiagaraComponent*> Beams;

	/**
	 * Draws as many reflected beams as need to. This function calls DrawLaserBeamSingle function in Beams array
	 * iterating for loop. Should be called in tick to recalculate beams dynamically.
	 */
	void DrawLaserBeams();

	/**
	 * Draw one beam and calculate start location and direction of next beam. This should be called while
	 * iterating through Beams array.
	 * @param CurrentBeamIndex Current index of Beams array iteration.
	 * @param BeamStartLocation Should be initialized with current beam start location. Out is next beam start location.
	 * @param BeamDirection Should be initialized with current beam direction. Out is next beam direction.
	 * @return True if there should be next reflected beam. You should keep calling this in Beams array iteration in
	 * this case.
	 * @return False if there can't be next reflected beam. You should stop calling this in Beams array iteration in
	 * this case.
	 */
	bool DrawLaserBeamSingle(const int32 CurrentBeamIndex, FVector& BeamStartLocation, FVector& BeamDirection);

	/**
	 * You must set your own custom Collision Trace Channel with "Block" default response to this variable to make laser
	 * work properly!
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	TEnumAsByte<ECollisionChannel> LaserCollisionTraceChanel = ECC_GameTraceChannel2;

	// If ReflectAll is false than only Actors with this tag will reflect beams
	UPROPERTY(EditAnywhere, Category = "Tags")
	FName ReflectActorTagName = TEXT("ReflectLaser");

	// Actors with this tag will be ignored by Laser
	UPROPERTY(EditAnywhere, Category = "Tags")
	FName IgnoreActorTagName = TEXT("IgnoreLaser");

	/**
	 * Enables or disables all beams in Beams array from FirstBeamIndex.
	 * @param bNewActive Whether we should enable or disable beams.
	 * @param FirstBeamIndex First beam index which will be enabled/disabled. All previous indexes will not be hit.
	 * You have to keep default 0 value if you need to enable/disable all beams.
	 */
	void SetBeamsActive(const bool bNewActive, const int32 FirstBeamIndex = 0);

	UPROPERTY(EditDefaultsOnly, Category = "Laser Niagara variables")
	FName SpawnBeamVariableName = TEXT("SpawnBeam");

	UPROPERTY(EditDefaultsOnly, Category = "Laser Niagara variables")
	FName SpawnBeamEndVariableName = TEXT("SpawnBeamEnd");

	UPROPERTY(EditDefaultsOnly, Category = "Laser Niagara variables")
	FName BeamStartLocationVariableName = TEXT("BeamStartLocation");

	UPROPERTY(EditDefaultsOnly, Category = "Laser Niagara variables")
	FName BeamEndLocationVariableName = TEXT("BeamEndLocation");

	UPROPERTY(EditDefaultsOnly, Category = "Laser Niagara variables")
	FName BeamsBisectorDirectionVariableName = TEXT("BeamsBisectorDirection");
};