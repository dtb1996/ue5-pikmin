// Fill out your copyright notice in the Description page of Project Settings.

#include "PikminWhistleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PikminSelectable.h"

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

	CurrentRadius = FMath::Clamp(CurrentRadius + ExpansionSpeed * DeltaTime, 0.f, MaxRadius);

	TArray<AActor*> Overlaps;
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		GetOwner()->GetActorLocation(),
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
		DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), CurrentRadius, 24, FColor::Blue, false, 0.05f);
	}
}

void UPikminWhistleComponent::StartWhistle()
{
	bIsWhistling = true;
	CurrentRadius = StartRadius;
}

void UPikminWhistleComponent::EndWhistle()
{
	bIsWhistling = false;

	//for (AActor* Pikmin : SelectedPikmin)
	//{
	//	if (IPikminSelectable* Selectable = Cast<IPikminSelectable>(Pikmin))
	//	{
	//		Selectable->OnWhistleDeselect();
	//	}
	//}

	//SelectedPikmin.Empty();
}
