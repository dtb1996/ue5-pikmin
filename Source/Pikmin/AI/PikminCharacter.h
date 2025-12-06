// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Systems/PikminSelectable.h"
#include "PikminState.h"
#include "PikminCharacter.generated.h"

UCLASS()
class APikminCharacter : public ACharacter, public IPikminSelectable
{
    GENERATED_BODY()

public:
    APikminCharacter();

    virtual void Tick(float DeltaTime) override;

    virtual void OnWhistleSelect_Implementation(AActor* Caller) override;

    virtual void OnWhistleDeselect_Implementation(AActor* Caller) override;

    bool IsBusy() const;

    EPikminState GetState() const;

    void BeginThrow(const FVector& Target, AActor* Thrower);

    // Thrown state variables
    UPROPERTY(EditAnywhere, Category = "Throw")
    float ThrowHeight = 350.f;

    UPROPERTY(EditAnywhere, Category = "Throw")
    float GravityScale = 1.0f;

    bool bIsThrown = false;
    FVector ThrowStart;
    FVector ThrowEnd;
    float ThrowTime;
    float ThrowDuration = 0.6f;

protected:
    void OnThrowLanded();
};
