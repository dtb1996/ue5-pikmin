// Fill out your copyright notice in the Description page of Project Settings.

#include "PikminInteractionComponent.h"
#include "Components/SphereComponent.h"
#include "PikminSelectable.h"
#include "PikminPlayerCharacter.h"
//#include "GameFramework/Actor.h"
//#include "Engine/World.h"

UPikminInteractionComponent::UPikminInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPikminInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

    OwnerPlayer = Cast<APikminPlayerCharacter>(GetOwner());
    check(OwnerPlayer);

    // Create sphere and attach
    InteractionSphere = NewObject<USphereComponent>(OwnerPlayer, TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(OwnerPlayer->GetRootComponent());
    InteractionSphere->RegisterComponent();

    InteractionSphere->SetSphereRadius(120.f);
    InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    InteractionSphere->SetCollisionObjectType(ECC_Pawn);

    InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &UPikminInteractionComponent::OnSphereOverlapBegin);
    InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &UPikminInteractionComponent::OnSphereOverlapEnd);
}

void UPikminInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (InteractionSphere)
    {
        InteractionSphere->OnComponentBeginOverlap.RemoveDynamic(this, &UPikminInteractionComponent::OnSphereOverlapBegin);
        InteractionSphere->OnComponentEndOverlap.RemoveDynamic(this, &UPikminInteractionComponent::OnSphereOverlapEnd);
    }

    Super::EndPlay(EndPlayReason);
}

void UPikminInteractionComponent::TryPluck()
{
    if (CurrentSelectable)
    {
        IPikminSelectable::Execute_OnPluck(CurrentSelectable, OwnerPlayer);
    }
}

void UPikminInteractionComponent::OnSelectableBecamePluckable(AActor* Selectable)
{
    if (!Selectable || !InteractionSphere)
    {
        return;
    }

    // Only update if the sphere currently overlaps the sprout
    if (InteractionSphere->IsOverlappingActor(Selectable))
    {
        if (Selectable->GetClass()->ImplementsInterface(UPikminSelectable::StaticClass()) &&
            IPikminSelectable::Execute_IsPluckable(Selectable, OwnerPlayer))
        {
            CurrentSelectable = Selectable;
            // UpdateInteractionUI(true);
        }
    }
}

void UPikminInteractionComponent::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor)
    {
        return;
    }

    if (OtherActor->GetClass()->ImplementsInterface(UPikminSelectable::StaticClass()))
    {
        if (IPikminSelectable::Execute_IsPluckable(OtherActor, OwnerPlayer))
        {
            CurrentSelectable = OtherActor;
            //UpdateInteractionUI(true);
        }
    }
}

void UPikminInteractionComponent::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor == CurrentSelectable)
    {
        CurrentSelectable = nullptr;
        //UpdateInteractionUI(false);
    }
}
