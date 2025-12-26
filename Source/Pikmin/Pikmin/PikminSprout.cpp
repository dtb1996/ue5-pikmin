// Copyright 2025, Rolling Pixels. All Rights Reserved

#include "PikminSprout.h"
#include "Systems/PikminManagerSubsystem.h"
#include "Player/PikminPlayerCharacter.h"
#include "Systems/PikminInteractionComponent.h"

APikminSprout::APikminSprout()
{
	PrimaryActorTick.bCanEverTick = true;

    SproutMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SproutMesh"));
    RootComponent = SproutMesh;

    SproutMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SproutMesh->SetCollisionObjectType(ECC_WorldDynamic);
    SproutMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    SproutMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void APikminSprout::BeginPlay()
{
	Super::BeginPlay();
	
    // Auto-ground snapping
    if (bAutoAlignToGround)
    {
        FVector Start = GetActorLocation() + FVector(0, 0, 200);
        FVector End = GetActorLocation() - FVector(0, 0, 500);

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
        {
            SetActorLocation(Hit.ImpactPoint);
        }
    }

    // Wait GrowTime seconds, then the sprout can be plucked
    GetWorldTimerManager().SetTimer(SproutTimerHandle, this, &APikminSprout::BecomePluckable, GrowTime, false);
}

void APikminSprout::OnPluck_Implementation(AActor* Caller)
{
    if (!bIsPluckable)
    {
        return;
    }

    // Spawn Pikmin
    UPikminManagerSubsystem* Manager = GetGameInstance()->GetSubsystem<UPikminManagerSubsystem>();
    if (Manager)
    {
        FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 50);
        Manager->SpawnPikmin(this, SpawnLocation, PikminType, true);
    }

    // Play animation, emit particles, etc.

    Destroy();
}

void APikminSprout::BecomePluckable()
{
    bIsPluckable = true;

    // Change mesh, color, emit particles, etc.

   // Notify any nearby player interaction components that this object is now pluckable
    TArray<AActor*> OverlappingActors;
    GetOverlappingActors(OverlappingActors, APikminPlayerCharacter::StaticClass());

    for (AActor* Actor : OverlappingActors)
    {
        APikminPlayerCharacter* Player = Cast<APikminPlayerCharacter>(Actor);
        if (!Player)
        {
            continue;
        }

        if (UPikminInteractionComponent* Interaction = Player->FindComponentByClass<UPikminInteractionComponent>())
        {
            Interaction->OnSelectableBecamePluckable(this);
        }
    }
}
