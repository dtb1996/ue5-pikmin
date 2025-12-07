// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PikminState.h"
//#include "Systems/PikminTaskSubsystem.h"
#include "Interfaces/PikminTaskInteractable.h"
#include "PikminAIController.generated.h"

class APikminCharacter;

/**
 * 
 */
UCLASS()
class PIKMIN_API APikminAIController : public AAIController
{
	GENERATED_BODY()

public:
    APikminAIController();

    virtual void Tick(float DeltaTime) override;
    virtual void OnPossess(APawn* InPawn) override;

    // State management
    void SetState(EPikminState NewState);
    EPikminState GetState() const { return CurrentState; };

    // High-level requests
    void RequestFollow(AActor* Caller);
    void RequestIdle();

    // Called when thrown finishes
    void OnThrownLanded();

    // Called by Pikmin character to notify task completion/detach
    void HandleTaskComplete();

    // Query
    bool IsBusy() const;

    // Task assignment helpers
    void AssignToTask(TScriptInterface<IPikminTaskInteractable> NewTask);
    void ClearActiveTask();

    TScriptInterface<IPikminTaskInteractable> GetActiveTask() const { return ActiveTask; };

    // Expose controlled pawn safely
    APikminCharacter* GetControlledPikmin() const { return ControlledPikmin; }

protected:
    // State handlers
    void UpdateState(float DeltaTime);

private:
    // States
    EPikminState CurrentState = EPikminState::Idle;

    // Pointer to character
    APikminCharacter* ControlledPikmin = nullptr;

    // Current task object (if any)
    TScriptInterface<IPikminTaskInteractable> ActiveTask;

public:
    // Search radius for local tasks
    UPROPERTY(EditAnywhere, Category = "Tasks")
    float TaskSearchRadius = 300.0f;

    // Task scanning
    float TimeSinceLastScan = 0.0f;
    float ScanCooldown = 0.5f;
    
    // Follow target
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USceneComponent* FollowTarget;
};
