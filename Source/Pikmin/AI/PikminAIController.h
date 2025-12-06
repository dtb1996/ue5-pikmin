// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PikminState.h"
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

    void SetState(EPikminState NewState);
    EPikminState GetState() const { return CurrentState; };

    void SetLeaderFollowTarget(USceneComponent* NewTarget);

    void RequestFollow(AActor* Caller);
    void RequestIdle();

    bool IsBusy() const;

private:
    void UpdateState(float DeltaTime);

    void IdleState(float DeltaTime);
    void FollowState(float DeltaTime);
    void ThrownState(float DeltaTime);


    EPikminState CurrentState = EPikminState::Idle;

    APikminCharacter* ControlledPikmin;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* FollowTarget;
};
