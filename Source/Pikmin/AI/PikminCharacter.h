// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Systems/PikminSelectable.h"
#include "PikminState.h"
#include "PikminCharacter.generated.h"

USTRUCT()
struct FThrowMotion
{
    GENERATED_BODY()

    UPROPERTY()
    bool bActive = false;

    UPROPERTY()
    FVector Start = FVector::ZeroVector;

    UPROPERTY()
    FVector End = FVector::ZeroVector;

    UPROPERTY()
    float Time = 0.0f;

    UPROPERTY()
    float Duration = 0.6f;

    UPROPERTY()
    float Height = 350.0f;

    void Reset() { bActive = false; Time = 0.0f; }
};

UCLASS()
class APikminCharacter : public ACharacter, public IPikminSelectable
{
    GENERATED_BODY()

public:
    APikminCharacter();

    virtual void Tick(float DeltaTime) override;

    // Interface functions
    virtual void OnWhistleSelect_Implementation(AActor* Caller) override;
    virtual void OnWhistleDeselect_Implementation(AActor* Caller) override;

    // High-level actions the controller calls
    void RequestMoveTo(const FVector& Location, float AcceptanceRadius = 50.0f);
    void RequestStop();
    void AttachToTaskActor(AActor* TaskActor, const FVector& WorldAttachLocation);
    void DetachFromTaskActor();

    // Throw helpers
    void BeginThrowTo(const FVector& Target, float Duration = 0.6f, float Height = 350.0f);
    bool IsThrown() const { return ThrowMotion.bActive; }

    // Queries
    bool IsBusy() const;
    EPikminState GetState() const;

    // Called by carryable object when task completes
    UFUNCTION(BlueprintCallable)
    void OnTaskCompleted();

protected:
    // Called internally when throw finishes
    void OnThrowFinish();

    // Encapsulated throw state
    UPROPERTY(VisibleAnywhere, Category = "Throw")
    FThrowMotion ThrowMotion;

    // Track current assigned task actor (weak)
    UPROPERTY()
    TWeakObjectPtr<AActor> CurrentTaskActor;

public:
    // Animation / gameplay tuning
    UPROPERTY(EditAnywhere, Category = "Movement")
    float MoveAcceptanceRadius = 50.0f;
};
