// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/PikminTaskInteractable.h"
#include "PikminTaskSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PIKMIN_API UPikminTaskSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    void RegisterTask(TScriptInterface<IPikminTaskInteractable> Task) { Tasks.Add(Task); }
    void UnregisterTask(TScriptInterface<IPikminTaskInteractable> Task) { Tasks.Remove(Task); }

    TScriptInterface<IPikminTaskInteractable> GetNearestAvailableTask(const FVector& Location);

    UPROPERTY()
    TArray<TScriptInterface<IPikminTaskInteractable>> Tasks;
};
