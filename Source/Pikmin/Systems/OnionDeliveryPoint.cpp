// Fill out your copyright notice in the Description page of Project Settings.

#include "OnionDeliveryPoint.h"
#include "Systems/PikminManagerSubsystem.h"

AOnionDeliveryPoint::AOnionDeliveryPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemTypeToDeliver = EItemType::FoodRed;
}

void AOnionDeliveryPoint::HandleItemDelivered(EItemType ItemType, int32 PikminYield)
{
	if (ItemType != ItemTypeToDeliver)
	{
		return;
	}

	const int32 Yield = PikminYield;

	UPikminManagerSubsystem* Manager = GetGameInstance()->GetSubsystem<UPikminManagerSubsystem>();
	if (!Manager)
	{
		return;
	}

	int32 CurrentFieldCount = Manager->GetTotalCreatureCount();
	int32 FreeSlots = Manager->GetFreeSlots();
	int32 ToSpawn = FMath::Min(FreeSlots, Yield);
	int32 ToStore = Yield - ToSpawn;

	// Spawn Pikmin sprouts into world
	for (int i = 0; i < ToSpawn; i++)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 100);
		Manager->SpawnSprout(this, SpawnLocation, PikminType);
	}

	// Store overflow Pikmin
	if (ToStore > 0)
	{
		StoredPikmin += ToStore;
	}
}

void AOnionDeliveryPoint::ReleaseStoredPikmin(int32 NumRequested)
{
	UPikminManagerSubsystem* Manager = GetGameInstance()->GetSubsystem<UPikminManagerSubsystem>();
	if (!Manager)
	{
		return;
	}

	int32 CurrentFieldCount = Manager->GetTotalCreatureCount();
	int32 FreeSlots = Manager->GetFreeSlots();
	int32 ToSpawn = FMath::Min(FreeSlots, StoredPikmin);

	for (int i = 0; i < ToSpawn; i++)
	{
		Manager->SpawnPikmin(this, GetActorLocation() + FVector(0, 0, 100), PikminType);
	}

	StoredPikmin -= ToSpawn;
}
