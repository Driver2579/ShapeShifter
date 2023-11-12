#pragma once

#include "BallPawnSaveData.generated.h"

enum class EBallPawnForm;

USTRUCT()
struct FBallPawnSaveData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FTransform PlayerTransform;

	UPROPERTY()
	FVector PlayerVelocity;

	UPROPERTY()
	EBallPawnForm PlayerForm;

	// Whether Clone was spawned in saved game or not
	UPROPERTY()
	bool bHasPlayerClone;

	UPROPERTY()
	FTransform CloneTransform;

	UPROPERTY()
	FVector CloneVelocity;
};