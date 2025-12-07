// Fill out your copyright notice in the Description page of Project Settings.


#include "DeliveryPoint.h"
#include "PikminTaskSubsystem.h"

ADeliveryPoint::ADeliveryPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADeliveryPoint::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto Subsystem = GetGameInstance()->GetSubsystem<UPikminTaskSubsystem>())
	{
		Subsystem->RegisterDropOff(ItemTypeToDeliver, this);
	}
}
