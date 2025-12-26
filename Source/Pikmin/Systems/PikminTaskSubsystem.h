// Copyright 2025, Rolling Pixels. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/PikminTaskInteractable.h"
#include "Types/ItemTypes.h"
#include "PikminTaskSubsystem.generated.h"

class ACarryableObject;

USTRUCT()
struct FCarryableObjectArrayWrapper
{
    GENERATED_BODY()

    TArray<TWeakObjectPtr<ACarryableObject>> CarryableObjects;
};

/**
 * 
 */
UCLASS()
class PIKMIN_API UPikminTaskSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void RegisterTask(TScriptInterface<IPikminTaskInteractable> Task) { Tasks.Add(Task); }

    UFUNCTION(BlueprintCallable)
    void UnregisterTask(TScriptInterface<IPikminTaskInteractable> Task) { Tasks.Remove(Task); }

    TScriptInterface<IPikminTaskInteractable> GetNearestAvailableTask(const FVector& Location);

    UPROPERTY()
    TArray<TScriptInterface<IPikminTaskInteractable>> Tasks;

public:
    UPROPERTY()
    TMap<EItemType, AActor*> DropOffLocations;

    UPROPERTY()
    TMap<EItemType, FCarryableObjectArrayWrapper> PendingAssignments;

public:
    UFUNCTION(BlueprintCallable)
    void RegisterDropOff(EItemType Type, AActor* DropOff);

    UFUNCTION(BlueprintCallable)
    void RequestDropOff(EItemType Type, ACarryableObject* CarryObject);
};
