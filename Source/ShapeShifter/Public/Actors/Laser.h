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

private:
	UPROPERTY(EditAnywhere, Category = "Activation")
	bool bActive = true;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* NiagaraSystemTemplate;

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

	void DrawLaserBeams();

	/**
	 * @brief Enables or disables all beams in Beams array from FirstBeamIndex.
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