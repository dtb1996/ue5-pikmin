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
