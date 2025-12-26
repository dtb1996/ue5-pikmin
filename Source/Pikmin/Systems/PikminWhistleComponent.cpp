// Copyright 2025, Rolling Pixels. All Rights Reserved

#include "PikminWhistleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/PikminSelectable.h"

UPikminWhistleComponent::UPikminWhistleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPikminWhistleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTick)
{
	Super::TickComponent(DeltaTime, TickType, ThisTick);

	if (!bIsWhistling)
	{
		return;
	}

	const FVector Origin = WhistleOrigin;

	CurrentRadius = FMath::Clamp(CurrentRadius + ExpansionSpeed * DeltaTime, 0.f, MaxRadius);

	TArray<AActor*> Overlaps;
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		Origin,
		CurrentRadius,
		{ UEngineTypes::ConvertToObjectType(ECC_Pawn) },
		nullptr,
		TArray<AActor*>(),
		Overlaps
	);

	for (AActor* Actor : Overlaps)
	{
		if (Actor->GetClass()->ImplementsInterface(UPikminSelectable::StaticClass()))
		{
			IPikminSelectable::Execute_OnWhistleSelect(Actor, GetOwner());
		}
	}

	// Debug circle
	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), Origin, CurrentRadius, 24, FColor::Blue, false, 0.05f);
	}
}

void UPikminWhistleComponent::StartWhistle()
{
	bIsWhistling = true;
	CurrentRadius = StartRadius;

	if (WhistleOrigin.IsNearlyZero())
	{
		WhistleOrigin = GetOwner()->GetActorLocation();
	}
}

void UPikminWhistleComponent::EndWhistle()
{
	bIsWhistling = false;
	WhistleOrigin = FVector::ZeroVector;
}

void UPikminWhistleComponent::SetWhistleOrigin(const FVector& InOrigin)
{
	WhistleOrigin = InOrigin;
}
