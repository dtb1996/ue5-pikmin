// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Systems/DeliveryPoint.h"
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

protected:
	virtual void BeginPlay() override;

public:
    // Stored Pikmin waiting to be spawned later
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Onion")
    int32 StoredPikmin = 0;

    // Called when food is delivered
    virtual void HandleItemDelivered(EItemType ItemType, int32 PikminYield) override;

    UFUNCTION(BlueprintCallable)
    void ReleaseStoredPikmin(int32 NumRequested);
};
