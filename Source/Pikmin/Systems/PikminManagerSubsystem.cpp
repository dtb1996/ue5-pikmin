// Fill out your copyright notice in the Description page of Project Settings.


#include "PikminManagerSubsystem.h"
#include "AI/PikminCharacter.h"
#include "PikminSprout.h"
#include "PikminGameInstance.h"

void UPikminManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (auto PikminGI = Cast<UPikminGameInstance>(GI))
		{
			PikminClass = PikminGI->DefaultPikminClass;
			SproutClass = PikminGI->DefaultSproutClass;
		}
	}
}

APikminCharacter* UPikminManagerSubsystem::SpawnPikmin(UObject* WorldContextObject, const FVector& Location, EPikminType PikminType, bool bIsSpawningFromSprout)
{
	if (!PikminClass || !WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FVector SpawnLocation = Location;

	// Adjust location if needed
	FindValidSpawnLocation(World, PikminClass, Location, SpawnLocation);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APikminCharacter* Pikmin = World->SpawnActor<APikminCharacter>(PikminClass, SpawnLocation, FRotator::ZeroRotator, Params);
	if (!Pikmin)
	{
		return nullptr;
	}

	PikminArray.Add(Pikmin);
	Pikmin->PikminType = PikminType;

	if (bIsSpawningFromSprout)
	{
		APikminSprout* PikminSprout = Cast<APikminSprout>(WorldContextObject);
		if (PikminSprout)
		{
			SproutArray.Remove(PikminSprout);
		}
	}

	return Pikmin;
}

APikminSprout* UPikminManagerSubsystem::SpawnSprout(UObject* WorldContextObject, const FVector& Location, EPikminType PikminType)
{
	if (!SproutClass || !WorldContextObject || GetFreeSlots() <= 0)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FVector SpawnLocation = Location;
	FindValidSpawnLocation(World, SproutClass, Location, SpawnLocation);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APikminSprout* Sprout = World->SpawnActor<APikminSprout>(SproutClass, Location, FRotator::ZeroRotator);
	if (!Sprout)
	{
		nullptr;
	}

	Sprout->PikminType = PikminType;
	SproutArray.Add(Sprout);

	return Sprout;
}

int32 UPikminManagerSubsystem::GetFreeSlots() const
{
	return FMath::Max(0, MaxPikminInWorld - GetTotalCreatureCount());
}

APikminCharacter* UPikminManagerSubsystem::GetNextThrowablePikmin(AActor* Player)
{
	APikminCharacter* Closest = nullptr;
	float BestDistance = FLT_MAX;

	for (APikminCharacter* Pikmin : PikminArray)
	{
		if (!Pikmin || Pikmin->IsBusy() || Pikmin->GetState() != EPikminState::Following)
		{
			continue;
		}

		float Distance = FVector::Dist(Player->GetActorLocation(), Pikmin->GetActorLocation());


		if (Distance < BestDistance)
		{
			BestDistance = Distance;
			Closest = Pikmin;
		}
	}

	return Closest;
}

bool UPikminManagerSubsystem::FindValidSpawnLocation(UWorld* World, TSubclassOf<AActor> ActorClass, const FVector& DesiredLocation, FVector& OutLocation) const
{
	if (!World || !*ActorClass)
	{
		return false;
	}

	// Temporary transform
	FTransform TestTransform(FRotator::ZeroRotator, DesiredLocation);

	// Spawn collision component size
	AActor* DefaultActor = ActorClass->GetDefaultObject<AActor>();
	if (!DefaultActor)
	{
		return false;
	}

	OutLocation = DesiredLocation;

	// Uses capsule / root collision to find a free spot
	return World->FindTeleportSpot(DefaultActor, OutLocation, FRotator::ZeroRotator);
}
