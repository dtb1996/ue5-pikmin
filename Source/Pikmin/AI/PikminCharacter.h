// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Systems/PikminSelectable.h"
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
};
