// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Activatable.h"
#include "Teleporter.generated.h"

class ABallPawn;

UCLASS()
class SHAPESHIFTER_API ATeleporter : public AActor, public IActivatable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATeleporter();

	virtual bool IsActive() const override final;

	virtual void Activate() override;
	virtual void Deactivate() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// Triggers teleportation to connected teleporter
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TeleportTriggerComponent;

	// Attach all other NiagaraParticlesComponent to this if you need more
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* NiagaraParticlesComponent;

	// A point where Pawn will be teleported to
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* TeleportPointComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UAudioComponent* ActivateAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UAudioComponent* DeactivateAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UAudioComponent* AmbientAudioComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds")
	USoundCue* TeleportSound;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnTeleportTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnTeleportTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(EditAnywhere, Category = "Activation")
	bool bActive = true;

	UPROPERTY(EditInstanceOnly)
	ATeleporter* OtherTeleporter;

	void TeleportBallPawn(ABallPawn* BallPawnToTeleport) const;

	TArray<ABallPawn*> JustTeleportedBallPawns;

	// Check if we can use OtherTeleporter for teleportation
	bool IsOtherTeleporterValid() const;
};