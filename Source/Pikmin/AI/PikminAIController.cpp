// Fill out your copyright notice in the Description page of Project Settings.


#include "PikminAIController.h"
#include "PikminCharacter.h"

APikminAIController::APikminAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void APikminAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ControlledPikmin = Cast<APikminCharacter>(InPawn);
}

void APikminAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateState(DeltaTime);
}

void APikminAIController::SetState(EPikminState NewState)
{
    CurrentState = NewState;
}

void APikminAIController::SetLeaderFollowTarget(USceneComponent* NewTarget)
{
    FollowTarget = NewTarget;
}

void APikminAIController::UpdateState(float DeltaTime)
{
    switch (CurrentState)
    {
    case EPikminState::Idle:
        IdleState(DeltaTime);
        break;

    case EPikminState::Following:
        FollowState(DeltaTime);
        break;

    default:
        IdleState(DeltaTime);
        break;
    }
}

void APikminAIController::IdleState(float DeltaTime)
{
    StopMovement();
}

void APikminAIController::FollowState(float DeltaTime)
{
    if (!ControlledPikmin || !FollowTarget)
    {
        return;
    }

    FVector Target = FollowTarget->GetComponentLocation();
    float Dist = FVector::Dist2D(Target, ControlledPikmin->GetActorLocation());

    if (Dist > 50.f)
    {
        MoveToLocation(Target, 50.f);
    }
    else
    {
        StopMovement();
    }
}
