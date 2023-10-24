// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Common/Enums/BallPawnForm.h"
#include "BallPawn.generated.h"

class UInputAction;

struct FInputActionValue;

UCLASS()
class SHAPESHIFTER_API ABallPawn : public APawn
{
	GENERATED_BODY()

public:
	ABallPawn();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaSeconds) override;

	UStaticMeshComponent* GetMesh() const;

	bool IsFalling() const;

	void SetForm(const EBallPawnForm NewForm);

	// Change form to the next one. Usually called from ChangeFormAction InputAction.
	void ChangeForm();

	// Spawn Clone in CreateCloneRate seconds and destroy old clone if it exists
	void CreateClone();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enhanced Input")
	class UInputMappingContext* DefaultMappingContext;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Call AddMappingContext to LocalPlayerSubsystem if it was not added before
	void InitDefaultMappingContext() const;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* JumpAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* ChangeFormAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* CreateCloneAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void Jump(const FInputActionValue& Value);

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
		FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void SpawnClone();

private:
	void SetupComponents();

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = 0))
	float MovementSpeed = 150;

	bool bCanJump = true;

	/**
	 * You must set your own custom Collision Trace Channel with "Block" default response to this variable to make
	 * Jump and IsFalling functions work properly!
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	TEnumAsByte<ECollisionChannel> FallingCollisionTraceChanel = ECC_GameTraceChannel1;

	// Velocity from previous tick
	FVector LastUpdateVelocity;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = 0))
	float JumpImpulse = 500;

	EBallPawnForm CurrentForm = EBallPawnForm::Rubber;

	UPROPERTY(EditAnywhere, Category = "Materials")
	TMap<EBallPawnForm, UMaterial*> FormMaterials;

	UPROPERTY(EditAnywhere, Category = "Collision")
	TMap<EBallPawnForm, UPhysicalMaterial*> FormPhysicalMaterials;

	UPROPERTY(EditAnywhere, Category = "Physics")
	TMap<EBallPawnForm, float> FormMasses;

	TWeakObjectPtr<ABallPawn> Clone;

	FTimerHandle CreateCloneTimer;

	// A delay before Clone will be created
	UPROPERTY(EditAnywhere, Category = "Clone")
	float CreateCloneRate = 3;

	FTransform CloneSpawnTransform;

	/**
	 * Check if Clone will collide with player on spawn.
	 * @return True if not colliding.
	 */
	bool CanSpawnClone() const;

	/**
	 * You must set your own custom Collision Trace Channel which will Block only pawn to make Clone spawning work
	 * properly!
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	TEnumAsByte<ECollisionChannel> SpawnCloneCheckTraceChanel = ECC_GameTraceChannel3;

	// If true than Clone will be destroyed when changing Form
	UPROPERTY(EditAnywhere, Category = "Clone")
	bool bDestroyCloneOnChangeForm = true;

	// This tag will work only if CurrentForm is Metal
	UPROPERTY(EditAnywhere, Category = "Clone")
	FName ReflectLaserTagName = TEXT("ReflectLaser");
};