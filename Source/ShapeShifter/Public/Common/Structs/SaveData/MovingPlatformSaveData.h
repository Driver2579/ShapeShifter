#pragma once

#include "MovingPlatformSaveData.generated.h"

USTRUCT()
struct FMovingPlatformSaveData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    float PlaybackPosition;

    UPROPERTY()
    bool bActive;
};