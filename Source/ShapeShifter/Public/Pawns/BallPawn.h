// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/Enums/BallPawnForm.h"
#include "GameFramework/Pawn.h"
#include "BallPawn.generated.h"

class UInputAction;

struct FInputActionValue;

UCLASS()
class SHAPESHIFTER_API ABallPawn : public APawn
{
	GENERATED_BODY()

public:
	ABallPawn();

	// Limits PlayerCameraManager ViewPitch
	void LimitViewPitch(const float MinViewPitch, const float MaxViewPitch) const;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void SetForm(const EBallPawnForm NewForm);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enhanced Input")
	class UInputMappingContext* DefaultMappingContext;

	// Call AddMappingContext to LocalPlayerSubsystem if it was not added before
	void InitDefaultMappingContext() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* JumpAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* ChangeFormAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void Jump(const FInputActionValue& Value);

	bool IsZVelocityClear(const float Tolerance = 0.1) const;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
		FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	// Change form to the next one
	void ChangeForm();

private:
	void SetupComponents();

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = 0))
	float MovementSpeed = 150;

	bool bCanJump = true;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = 0))
	float JumpImpulse = 500;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMax = 0))
	float DefaultMinViewPitch = -80;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = 0))
	float DefaultMaxViewPitch = 30;

	EBallPawnForm CurrentForm = EBallPawnForm::Rubber;

	UPROPERTY(EditAnywhere, Category = "Materials")
	TMap<EBallPawnForm, UMaterial*> FormMaterials;

	UPROPERTY(EditAnywhere, Category = "Physical Materials")
	TMap<EBallPawnForm, UPhysicalMaterial*> FormPhysicalMaterials;

	UPROPERTY(EditAnywhere, Category = "Physics")
	TMap<EBallPawnForm, float> FormMasses;
};