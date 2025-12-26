// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/DeliveryPoint.h"
#include "Types/PikminTypes.h"
#include "OnionDeliveryPoint.generated.h"

/**
 * 
 */
UCLASS()
class PIKMIN_API AOnionDeliveryPoint : public ADeliveryPoint
{
	GENERATED_BODY()
	
public:
	AOnionDeliveryPoint();

    // Type of Pikmin associated with this onion
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Onion")
    EPikminType PikminType = EPikminType::Red;

    // Stored Pikmin waiting to be spawned later
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Onion")
    int32 StoredPikmin = 0;

    UPROPERTY(EditAnywhere, Category = "Onion|Spawn")
    float SpawnRadius = 250.0f;

    UPROPERTY(EditAnywhere, Category = "Onion|Spawn")
    float SpawnRadiusInner = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Onion|Spawn")
    float GroundTraceHeight = 500.0f;

    // Called when food is delivered
    virtual void HandleItemDelivered(EItemType ItemType, int32 PikminYield) override;

    UFUNCTION(BlueprintCallable)
    void ReleaseStoredPikmin(int32 NumRequested);

protected:
    /** Debug */
    UPROPERTY(EditAnywhere, Category = "Onion|Debug")
    bool bDrawDebug = false;

private:
    FVector GetRandomSpawnPointOnGround() const;
};
