// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/PikminTypes.h"
#include "PikminManagerSubsystem.generated.h"

class APikminCharacter;
class APikminSprout;

/**
 * 
 */
UCLASS()
class PIKMIN_API UPikminManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // Max Pikmin + Sprouts simultaneously in the world
    UPROPERTY(EditAnywhere, Category = "Pikmin")
    int32 MaxPikminInWorld = 100;

    // --- Spawn Functions ---
    UFUNCTION(BlueprintCallable)
    APikminCharacter* SpawnPikmin(UObject* WorldContextObject, const FVector& Location, EPikminType PikminType, bool bIsSpawningFromSprout = false);

    UFUNCTION(BlueprintCallable)
    APikminSprout* SpawnSprout(UObject* WorldContextObject, const FVector& Location, EPikminType PikminType);

    // --- Array getters ---
    UFUNCTION(BlueprintCallable)
    const TArray<APikminCharacter*>& GetPikmin() const { return PikminArray; }
    const TArray<APikminSprout*>& GetSprouts() const { return SproutArray; }

    // --- Counts ---
    int32 GetCurrentPikminCount() const { return PikminArray.Num(); }
    int32 GetCurrentSproutCount() const { return SproutArray.Num(); }

    UFUNCTION(BlueprintCallable)
    int32 GetTotalCreatureCount() const { return PikminArray.Num() + SproutArray.Num(); };

    int32 GetFreeSlots() const;

    // --- Utility ---
    UFUNCTION(BlueprintCallable)
    APikminCharacter* GetNextThrowablePikmin(AActor* Player);

private:
    //UPROPERTY()
    TArray<APikminCharacter*> PikminArray;
    TArray<APikminSprout*> SproutArray;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<APikminCharacter> PikminClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<APikminSprout> SproutClass;

    bool FindValidSpawnLocation(UWorld* World, TSubclassOf<AActor> ActorClass, const FVector& DesiredLocation, FVector& OutLocation) const;
};
