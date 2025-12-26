// Fill out your copyright notice in the Description page of Project Settings.


#include "PikminAIController.h"
#include "Pikmin/PikminCharacter.h"
#include "Player/PikminPlayerCharacter.h"
#include "Systems/PikminTaskSubsystem.h"
#include "PikminBehaviors.h"

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

void APikminAIController::RequestFollow(AActor* Caller)
{
    if (!Caller || IsBusy())
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
        SetState(EPikminState::Following);
    }
}

void APikminAIController::RequestIdle()
{
    SetState(EPikminState::Idle);
}

void APikminAIController::OnThrownLanded()
{
    // Become idle and immediately scan
    SetState(EPikminState::Idle);
    TimeSinceLastScan = ScanCooldown;
}

void APikminAIController::HandleTaskComplete()
{
    ActiveTask = nullptr;
    SetState(EPikminState::Idle);
}

bool APikminAIController::IsBusy() const
{
    return CurrentState == EPikminState::Carrying || CurrentState == EPikminState::Thrown;
}

void APikminAIController::AssignToTask(TScriptInterface<IPikminTaskInteractable> NewTask)
{
    if (!ControlledPikmin || !NewTask)
    {
        return;
    }

    // Unassign previous
    if (ActiveTask)
    {
        IPikminTaskInteractable::Execute_UnassignPikmin(ActiveTask.GetObject(), ControlledPikmin);
    }

    ActiveTask = NewTask;

    // Tell the task to register this pikmin (task stores weak refs)
    IPikminTaskInteractable::Execute_AssignPikmin(ActiveTask.GetObject(), ControlledPikmin);

    // Controller state will be changed by behavior when appropriate
}

void APikminAIController::ClearActiveTask()
{
    if (ActiveTask)
    {
        IPikminTaskInteractable::Execute_UnassignPikmin(ActiveTask.GetObject(), ControlledPikmin);
        ActiveTask = nullptr;
    }
}

void APikminAIController::UpdateState(float DeltaTime)
{
    // Small dispatcher - actual logic lives in PikminBehaviors
    switch (CurrentState)
    {
    case EPikminState::Idle:
        PikminBehaviors::Idle_Update(this, DeltaTime);
        break;

    case EPikminState::Following:
        PikminBehaviors::Follow_Update(this, DeltaTime);
        break;

    case EPikminState::Working:
        PikminBehaviors::Working_Update(this, DeltaTime);
        break;

    case EPikminState::Thrown:
        PikminBehaviors::Thrown_Update(this, DeltaTime);
        break;

    default:
        PikminBehaviors::Idle_Update(this, DeltaTime);
        break;
    }
}
