// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemTypes.h"
#include "DeliveryPoint.generated.h"

UCLASS()
class PIKMIN_API ADeliveryPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ADeliveryPoint();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = "Task")
	EItemType ItemTypeToDeliver;

};
