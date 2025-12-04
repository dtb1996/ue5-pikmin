// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PikminCharacter.generated.h"

UCLASS()
class APikminCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APikminCharacter();

    virtual void Tick(float DeltaTime) override;
};
