// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/Savable.h"
#include "BuoyancyTypes.h"
#include "BallPawn.generated.h"

class UShapeShifterSaveGame;
class UInputAction;
class USoundCue;
class UNiagaraSystem;

struct FInputActionValue;

enum class EBallPawnForm;

UCLASS()
class SHAPESHIFTER_API ABallPawn : public APawn, public ISavable
{
	GENERATED_BODY()

public:
	ABallPawn();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaSeconds) override;

	UStaticMeshComponent* GetMesh() const;

	bool IsFalling() const;

	EBallPawnForm GetForm() const;
	void SetForm(const EBallPawnForm NewForm);

	// Change form to the next one. Usually called from ChangeFormAction InputAction.
	void ChangeForm();

	// Spawn Clone in CreateCloneRate seconds and kill old clone if it exists
	void CreateClone();

	// Cancel CreateCloneTimer and speed up CreateCloneNiagaraComponent in CreateCloneVfxSpeedOnCancel times
	void CancelCloneCreation();

	/**
	 * @return true if bOverlappingWaterJumpZone is true and CurrentForm is Rubber.
	 */
	bool IsSwimmingOnWaterSurface() const;

	void SetOverlappingWaterJumpZone(const bool bNewOverlappingWaterJumpZone);

	/**
	 * Destroy the BallPawn if it's a clone. Kills the BallPawn if it's a player. On kill the camera will be faded to
	 * black and last save will be loaded.
	 * @note This BallPawn will be destroyed after calling this function if it's a clone. So make sure you don't use
	 * this BallPawn instance after calling this function.
	 */
	void Die();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBuoyancyComponent* BuoyancyComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* RollingAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AirSlicingAudioComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* DefaultMappingContext;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Call AddMappingContext to LocalPlayerSubsystem if it was not added before
	void InitDefaultMappingContext() const;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* ChangeFormAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* CreateCloneAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* SaveGameAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* LoadGameAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clone")
	USoundCue* SpawnCloneSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clone")
	USoundCue* CreateCloneSound; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clone")
	USoundCue* CancelSpawnCloneSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Form")
	USoundCue* ChangeFormSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	USoundCue* DieSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	USoundCue* JumpSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	USoundCue* RubberHitSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	USoundCue* MetalHitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	USoundCue* RubberRollingSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	USoundCue* MetalRollingSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0), Category = "Movement")
	float MaxVelocityRollingSound = 500;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0), Category = "Movement")
	float MinPitchRollingSound = 0.75;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0), Category = "Movement")
	float MaxPitchRollingSound = 1.25;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 360), Category = "Movement")
	float AngelSound = 80;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0), Category = "Movement")
	float MinVelocityHitSound = 60;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0), Category = "Movement")
	float MaxVelocityHitSound = 900;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 1), Category = "Movement")
	float MinPitchAirSlicingSound = 0.5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 1), Category = "Movement")
	float MaxPitchAirSlicingSound = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0), Category = "Movement")
	float MinVelocityAirSlicingSound = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0), Category = "Movement")
	float MaxVelocityAirSlicingSound = 500;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void Jump(const FInputActionValue& Value);

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
		FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	// Remove old Clone and spawn the new one if it won't collide anything
	void SpawnClone();

	virtual void OnSaveGame(UShapeShifterSaveGame* SaveGameObject) override;
	virtual void OnLoadGame(UShapeShifterSaveGame* SaveGameObject) override;

	void SaveGame();
	void LoadGame();

	// Call RegisterDynamicForce for every FluidSim Actor on scene
	void InitWaterFluidSimulation();

	// Call RegisterDynamicForce with parameters for FluidSim. This needed to register components for fluid simulation.
	UFUNCTION(BlueprintImplementableEvent, Category = "Water Fluid Simulation")
	void RegisterDynamicForce(AActor* FluidSim, USceneComponent* ForceComponent, const float ForceRadius,
		const float ForceStrength);

	// Revive the player if he was dead. The camera will fade back from black. Doesn't work for clones.
	void Revive();

private:
	void SetupComponents();

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = 0))
	float MovementSpeed = 150;

	/**
	 * How fast player can turn the other direction.
	 * 0 - no effect;
	 * 1 - immediately.
	 */
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = 0, ClampMax = 1))
	float MovementAmortizationFactor = 0.75;

	// Whether player can jump or not
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bCanEverJump = true;

	bool bCanJump = true;

	/**
	 * You must set your own custom Collision Trace Channel with "Block" default response to this variable to make
	 * Jump and IsFalling functions work properly!
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	TEnumAsByte<ECollisionChannel> FallingCollisionTraceChanel = ECC_GameTraceChannel1;

	// Velocity from previous tick
	FVector LastUpdateVelocity;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = 0, EditCondition = "bCanEverJump"))
	float JumpImpulse = 500;

	bool bOverlappingWaterJumpZone = false;

	FTimerHandle JumpOnWaterSurfaceTimer;

	// Delay between jumping on water surface
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = 0.1, ClampMax = 1, EditCondition = "bCanEverJump"))
	float JumpOnWaterSurfaceDelay = 0.5;

	// Enable jumping in JumpOnWaterSurfaceDelay if swimming on water surface when execute
	void EnableJumpIfSwimmingWithDelay();

	// Whether player can change form or not
	UPROPERTY(EditAnywhere, Category = "Form")
	bool bCanEverChangeForm = true; 

	EBallPawnForm CurrentForm;

	// Niagara VFX which will be spawned on form changing
	UPROPERTY(EditDefaultsOnly, Category = "Form")
	UNiagaraSystem* ChangeFormNiagaraSystemTemplate; 

	UPROPERTY(EditAnywhere, Category = "Materials")
	TMap<EBallPawnForm, UMaterialInterface*> FormMaterials;

	UPROPERTY(EditAnywhere, Category = "Collision")
	TMap<EBallPawnForm, UPhysicalMaterial*> FormPhysicalMaterials;

	UPROPERTY(EditAnywhere, Category = "Physics")
	TMap<EBallPawnForm, float> FormMasses;

	// Whether player can create clone or not
	UPROPERTY(EditAnywhere, Category = "Clone")
	bool bCanEverCreateClone = true;

	TWeakObjectPtr<ABallPawn> Clone;

	FTimerHandle CreateCloneTimer;

	// A delay before Clone will be created
	UPROPERTY(EditAnywhere, Category = "Clone", meta = (EditCondition = "bCanEverCreateClone"))
	float CreateCloneRate = 3;

	FTransform CloneSpawnTransform;

	/**
	 * Check if Clone will collide with player on spawn.
	 * @return True if not colliding.
	 */
	bool CanSpawnClone() const;

	/**
	 * Spawn the Clone without any checks.
	 * @note Be careful with calling that because it will spawn the Clone even it's already exists or if it will collide
	 * something when spawned.
	 */
	void SpawnCloneObject();

	/**
	 * You must set your own custom Collision Trace Channel which will Block only pawn to make Clone spawning work
	 * properly!
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	TEnumAsByte<ECollisionChannel> SpawnCloneCheckTraceChanel = ECC_GameTraceChannel3;

	// If true than Clone will be destroyed when changing Form
	UPROPERTY(EditAnywhere, Category = "Clone", meta = (EditCondition = "bCanEverCreateClone"))
	bool bDestroyCloneOnChangeForm = true;

	// Niagara VFX which will be spawned on Clone creation start
	UPROPERTY(EditDefaultsOnly, Category = "Clone", meta = (EditCondition = "bCanEverCreateClone"))
	UNiagaraSystem* CreateCloneNiagaraSystemTemplate;

	TWeakObjectPtr<class UNiagaraComponent> CreateCloneNiagaraComponent;

	// How much faster compared with the default speed CreateCloneNiagaraComponent will be on Clone creation cancel 
	UPROPERTY(EditDefaultsOnly, Category = "Clone", meta = (ClampMin = 1, EditCondition = "bCanEverCreateClone"))
	float CreateCloneVfxSpeedOnCancel = 10;

	// Niagara VFX which will be spawned on Clone creation end
	UPROPERTY(EditDefaultsOnly, Category = "Clone", meta = (EditCondition = "bCanEverCreateClone"))
	UNiagaraSystem* SpawnCloneNiagaraSystemTemplate;

	// This tag will work only if CurrentForm is Metal
	UPROPERTY(EditAnywhere, Category = "Laser")
	FName ReflectLaserTagName = TEXT("ReflectLaser");

	// This tag will work only if BallPawn is dead
	UPROPERTY(EditAnywhere, Category = "Laser")
	FName IgnoreLaserTagName = TEXT("IgnoreLaser");

	/**
	 * BuoyancyData assigned to different BallPawnForms to swim on water.
	 * The default values are same, so they all must be changed in BP except Pontoons.
	 */
	UPROPERTY(EditAnywhere, Category = "Water Buoyancy Data")
	TMap<EBallPawnForm, FBuoyancyData> FormBuoyancyData;

	UPROPERTY(EditDefaultsOnly, Category = "Water Fluid Simulation")
	TSubclassOf<AActor> WaterFluidSimulationClass;

	UPROPERTY(EditDefaultsOnly, Category = "Water Fluid Simulation", meta = (ClampMin = 0))
	float WaterFluidForceStrength = 1;

	bool bDead = false;

	UPROPERTY(EditAnywhere, Category = "Death", meta = (ClampMin = 0))
	float DeathCameraFadeDuration = 2;

	FTimerHandle LoadAfterDeathTimer;

	// Niagara VFX which will be spawned on BallPawn death
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UNiagaraSystem* DeathNiagaraSystemTemplate;

	TWeakObjectPtr<USoundCue> CurrentHitSound;

	// This is needed to have control over CreateCloneSound from any part of this class code
	TWeakObjectPtr<UAudioComponent> CreateCloneAudioComponent;
};