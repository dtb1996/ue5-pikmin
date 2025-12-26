// Fill out your copyright notice in the Description page of Project Settings.

#include "CarryableAIController.h"

void ACarryableAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!TargetActor) return;

    TimeSinceRepath += DeltaSeconds;
    if (TimeSinceRepath >= RepathInterval)
    {
        TimeSinceRepath = 0.0f;
        MoveToActor(TargetActor, 50.0f);
    }
}

void ACarryableAIController::StopMovement()
{
    Super::StopMovement();

    TargetActor = nullptr;
}

void ACarryableAIController::SetMoveTarget(AActor* Target)
{
    TargetActor = Target;
}