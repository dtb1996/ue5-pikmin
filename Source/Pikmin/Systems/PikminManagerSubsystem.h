// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PikminManagerSubsystem.generated.h"

class APikminCharacter;

/**
 * 
 */
UCLASS()
class PIKMIN_API UPikminManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable)
    APikminCharacter* SpawnPikmin(UObject* WorldContextObject, const FVector& Location);

    UFUNCTION(BlueprintCallable)
    const TArray<APikminCharacter*>& GetPikmin() const { return PikminArmy; }

    UFUNCTION(BlueprintCallable)
    APikminCharacter* GetNextThrowablePikmin(AActor* Player);

private:
    UPROPERTY()
    TArray<APikminCharacter*> PikminArmy;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<APikminCharacter> PikminClass;
};
