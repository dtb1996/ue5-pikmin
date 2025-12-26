// Copyright 2025, Rolling Pixels. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CarryableAIController.generated.h"

UCLASS()
class PIKMIN_API ACarryableAIController : public AAIController
{
    GENERATED_BODY()

public:
    virtual void Tick(float DeltaSeconds) override;

    virtual void StopMovement() override;

    UFUNCTION(BlueprintCallable)
    void SetMoveTarget(AActor* Target);

private:
    AActor* TargetActor = nullptr;
    float RepathInterval = 0.5f;
    float TimeSinceRepath = 0.0f;
};
