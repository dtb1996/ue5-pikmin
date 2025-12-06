// Fill out your copyright notice in the Description page of Project Settings.


#include "PikminManagerSubsystem.h"
#include "AI/PikminCharacter.h"
#include "PikminGameInstance.h"

void UPikminManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (auto PikminGI = Cast<UPikminGameInstance>(GI))
		{
			PikminClass = PikminGI->DefaultPikminClass;
		}
	}
}

APikminCharacter* UPikminManagerSubsystem::SpawnPikmin(UObject* WorldContextObject, const FVector& Location)
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
		PikminArmy.Add(Pikmin);
	}

	return Pikmin;
}

APikminCharacter* UPikminManagerSubsystem::GetNextThrowablePikmin(AActor* Player)
{
	APikminCharacter* Closest = nullptr;
	float BestDistance = FLT_MAX;

	for (APikminCharacter* Pikmin : PikminArmy)
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
