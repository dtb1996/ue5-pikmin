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
		FVector SpawnLocation = GetRandomSpawnPointOnGround();
		Manager->SpawnSprout(this, SpawnLocation, PikminType);

		if (bDrawDebug)
		{
			DrawDebugSphere(GetWorld(), SpawnLocation, 15.f, 12, FColor::Green, false, 2.0f);
		}
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
		FVector SpawnLocation = GetRandomSpawnPointOnGround();
		Manager->SpawnSprout(this, SpawnLocation, PikminType);

		if (bDrawDebug)
		{
			DrawDebugSphere(GetWorld(), SpawnLocation, 15.f, 12, FColor::Green, false, 2.0f);
		}
	}

	StoredPikmin -= ToSpawn;
}

FVector AOnionDeliveryPoint::GetRandomSpawnPointOnGround() const
{
	const FVector Origin = GetActorLocation();

	const float Angle = FMath::FRandRange(0.0f, 2.0f * PI);

	const float Radius = FMath::Sqrt(
		FMath::FRandRange(
			FMath::Square(SpawnRadiusInner),
			FMath::Square(SpawnRadius)
		)
	);

	const FVector Offset(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.0f);

	const FVector TraceStart = Origin + Offset + FVector(0, 0, GroundTraceHeight);
	const FVector TraceEnd = Origin + Offset - FVector(0, 0, GroundTraceHeight);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		Params
	))
	{
		return Hit.ImpactPoint;
	}

	return Origin + Offset;
}
