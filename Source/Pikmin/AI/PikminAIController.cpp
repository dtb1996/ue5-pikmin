// Fill out your copyright notice in the Description page of Project Settings.


#include "PikminAIController.h"
#include "PikminCharacter.h"
#include "PikminPlayerCharacter.h"
#include "Systems/PikminTaskSubsystem.h"

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

void APikminAIController::RequestFollow(AActor* Caller)
{
    if (!Caller)
    {
        return;
    }

    if (IsBusy())
    {
        return;
    }

    if (ActiveTask)
    {
        ActiveTask->Execute_UnassignPikmin(ActiveTask.GetObject(), ControlledPikmin);
        ActiveTask = nullptr;
    }

    if (APikminPlayerCharacter* Player = Cast<APikminPlayerCharacter>(Caller))
    {
        FollowTarget = Player->FollowLocationComponent;
        CurrentState = EPikminState::Following;
    }
}

void APikminAIController::RequestIdle()
{
    if (IsBusy())
    {
        return;
    }

    CurrentState = EPikminState::Idle;
}

void APikminAIController::OnThrownLanded()
{
    CurrentState = EPikminState::Idle;

    TryFindTask();
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

    case EPikminState::Thrown:
        ThrownState(DeltaTime);
        break;

    case EPikminState::Working:
        WorkingState(DeltaTime);
        break;

    default:
        IdleState(DeltaTime);
        break;
    }
}

bool APikminAIController::IsBusy() const
{
    return CurrentState == EPikminState::Carrying || CurrentState == EPikminState::Thrown;
}

void APikminAIController::IdleState(float DeltaTime)
{
    StopMovement();

    TimeSinceLastScan += DeltaTime;
    if (TimeSinceLastScan >= ScanCooldown)
    {
        TimeSinceLastScan = 0.f;
        TryFindTask();
    }
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

void APikminAIController::ThrownState(float DeltaTime)
{

}

void APikminAIController::WorkingState(float DeltaTime)
{
    if (!ActiveTask)
    {
        SetState(EPikminState::Idle);
        return;
    }

    FVector Target = ActiveTask->Execute_GetTaskLocation(ActiveTask.GetObject());
    float Distance = FVector::Dist2D(Target, ControlledPikmin->GetActorLocation());

    if (Distance > 50.0f)
    {
        MoveToLocation(Target, 50.0f);
    }
    else
    {
        StopMovement();

        // TODO: attach to task actor socket
    }
}

void APikminAIController::TryFindTask()
{
    if (!ControlledPikmin || CurrentState != EPikminState::Idle)
    {
        return;
    }

    auto TaskSubsystem = GetGameInstance()->GetSubsystem<UPikminTaskSubsystem>();
    if (!TaskSubsystem)
    {
        return;
    }

    const FVector PikminLocation = ControlledPikmin->GetActorLocation();

    TScriptInterface<IPikminTaskInteractable> BestTask;
    float BestDistance = FLT_MAX;

    for (auto Task : TaskSubsystem->Tasks)
    {
        if (!Task)
        {
            continue;
        }

        FVector TaskLocation = Task->Execute_GetTaskLocation(Task.GetObject());
        float DistanceSquared = FVector::DistSquared(TaskLocation, PikminLocation);

        DrawDebugSphere(GetWorld(), ControlledPikmin->GetActorLocation(), TaskSearchRadius, 24, FColor::Green, false, 0.05f);

        if (DistanceSquared > TaskSearchRadius * TaskSearchRadius)
        {
            continue;
        }

        if (!Task->Execute_CanAcceptPikmin(Task.GetObject(), ControlledPikmin))
        {
            continue;
        }

        // Pick closest
        if (DistanceSquared < BestDistance)
        {
            BestDistance = DistanceSquared;
            BestTask = Task;
        }
    }

    if (BestTask)
    {
        ActiveTask = BestTask;
        BestTask->Execute_AssignPikmin(BestTask.GetObject(), ControlledPikmin);
        SetState(EPikminState::Working);
    }
}
