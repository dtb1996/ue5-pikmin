// Copyright 2025, Rolling Pixels. All Rights Reserved

#include "DeliveryPoint.h"
#include "Systems/PikminTaskSubsystem.h"
#include "Components/SphereComponent.h"
#include "Interfaces/PikminTaskInteractable.h"

ADeliveryPoint::ADeliveryPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));

	SphereCollision->InitSphereRadius(200.0f);
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SphereCollision->SetGenerateOverlapEvents(true);
	RootComponent = SphereCollision;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	StaticMesh->SetupAttachment(RootComponent);
}

void ADeliveryPoint::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto Subsystem = GetGameInstance()->GetSubsystem<UPikminTaskSubsystem>())
	{
		Subsystem->RegisterDropOff(ItemTypeToDeliver, this);
	}

	if (SphereCollision)
	{
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ADeliveryPoint::OnOverlapBegin);
	}
}

void ADeliveryPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (SphereCollision)
	{
		SphereCollision->OnComponentBeginOverlap.RemoveDynamic(this, &ADeliveryPoint::OnOverlapBegin);
	}
}

void ADeliveryPoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UPikminTaskInteractable::StaticClass()))
	{
		EItemType ItemType = IPikminTaskInteractable::Execute_GetItemType(OtherActor);
		int32 PikminYield = IPikminTaskInteractable::Execute_GetPikminYield(OtherActor);
		
		if (ItemType == ItemTypeToDeliver)
		{
			IPikminTaskInteractable::Execute_HandleTaskCompleted(OtherActor);
			HandleItemDelivered(ItemType, PikminYield);
		}
	}
}
