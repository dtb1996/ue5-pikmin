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

	APikminCharacter* Pikmin = World->SpawnActor<APikminCharacter>(PikminClass, Location, FRotator::ZeroRotator);
	if (Pikmin)
	{
		PikminArray.Add(Pikmin);
		Pikmin->PikminType = PikminType;
	}

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

	APikminSprout* Sprout = World->SpawnActor<APikminSprout>(SproutClass, Location, FRotator::ZeroRotator);
	if (Sprout)
	{
		Sprout->PikminType = PikminType;
		SproutArray.Add(Sprout);
	}

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
