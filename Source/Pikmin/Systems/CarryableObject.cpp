// Fill out your copyright notice in the Description page of Project Settings.

#include "CarryableObject.h"
#include "CarryableAIController.h"
#include "AI/PikminCharacter.h"
#include "AI/PikminAIController.h"
#include "Systems/PikminTaskSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"

ACarryableObject::ACarryableObject()
{
	PrimaryActorTick.bCanEverTick = true;

	// Character Movement
	GetCharacterMovement()->MaxWalkSpeed = BaseMoveSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCanEverAffectNavigation(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// AI Controller
	AIControllerClass = ACarryableAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ACarryableObject::BeginPlay()
{
	Super::BeginPlay();
	
	if (bAutoRegisterToSubsystem)
	{
		if (auto TaskSubsystem = GetGameInstance()->GetSubsystem<UPikminTaskSubsystem>())
		{
			TaskSubsystem->RegisterTask(this);
			TaskSubsystem->RequestDropOff(ItemType, this);
		}
	}
}

void ACarryableObject::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (bAutoRegisterToSubsystem)
	{
		if (auto TaskSubsystem = GetGameInstance()->GetSubsystem<UPikminTaskSubsystem>())
		{
			TaskSubsystem->UnregisterTask(this);
		}
	}
}

void ACarryableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePikminPositions();
}

// -------- Interface Logic --------

bool ACarryableObject::CanAcceptPikmin_Implementation(APikminCharacter* Pikmin)
{
	return AssignedPikmin.Num() < MaxPikmin;
}

void ACarryableObject::AssignPikmin_Implementation(APikminCharacter* Pikmin)
{
	if (!Pikmin || AssignedPikmin.Contains(Pikmin))
	{
		return;
	}

	AssignedPikmin.Add(Pikmin);

	Pikmin->HandleAttachToTaskActor(this, FVector::ZeroVector);
	
	UpdateMovement();

	// Check if we can start movement
	if (!bIsMoving && AssignedPikmin.Num() >= RequiredPikmin)
	{
		bIsMoving = true;

		if (ACarryableAIController* AI = Cast<ACarryableAIController>(GetController()))
		{
			AI->SetMoveTarget(DeliveryTarget);
		}
	}
}

void ACarryableObject::UnassignPikmin_Implementation(APikminCharacter* Pikmin)
{
	AssignedPikmin.Remove(Pikmin);

	Pikmin->HandleDetachFromTaskActor();
	
	UpdateMovement();

	if (AssignedPikmin.Num() < RequiredPikmin)
	{
		bIsMoving = false;

		if (ACarryableAIController* AI = Cast<ACarryableAIController>(GetController()))
		{
			AI->StopMovement();
		}
	}
}

FVector ACarryableObject::GetTaskLocation_Implementation() const
{
	return GetActorLocation();
}

void ACarryableObject::HandleTaskCompleted_Implementation()
{
	if (bAutoRegisterToSubsystem)
	{
		if (auto TaskSubsystem = GetGameInstance()->GetSubsystem<UPikminTaskSubsystem>())
		{
			TaskSubsystem->UnregisterTask(this);
		}
	}

	// Copy the array to safely iterate
	TArray<TWeakObjectPtr<APikminCharacter>> PikminCopy = AssignedPikmin;

	for (auto& PikminPtr : PikminCopy)
	{
		if (!PikminPtr.IsValid())
		{
			continue;
		}

		APikminCharacter* Pikmin = PikminPtr.Get();

		// Remove from the original array
		IPikminTaskInteractable::Execute_UnassignPikmin(this, Pikmin);

		Pikmin->OnTaskCompleted();
	}

	Destroy();
}

EItemType ACarryableObject::GetItemType_Implementation() const
{
	return ItemType;
}

int32 ACarryableObject::GetPikminYield_Implementation() const
{
	return PikminYield;
}

// -------- Pikmin Arrange Logic --------

FVector ACarryableObject::GetPikminAttachPoint(int32 Index) const
{
	float Angle = (360.0f / FMath::Max(1, AssignedPikmin.Num())) * Index;

	FVector HorizontalOffset =
		FVector(
			FMath::Cos(FMath::DegreesToRadians(Angle)),
			FMath::Sin(FMath::DegreesToRadians(Angle)),
			0.0f
		) * AttachRadius;

	FVector BaseLocation = GetActorLocation();
	BaseLocation.Z += PikminAttachHeightOffset;

	return BaseLocation + HorizontalOffset;
}

void ACarryableObject::UpdatePikminPositions()
{
	if (AssignedPikmin.Num() <= 0)
	{
		return;
	}

	for (int32 i = 0; i < AssignedPikmin.Num(); i++)
	{
		if (!AssignedPikmin[i].IsValid())
		{
			continue;
		}

		APikminCharacter* Pikmin = AssignedPikmin[i].Get();
		APikminAIController* AI = Cast<APikminAIController>(Pikmin->GetController());

		if (!AI)
		{
			continue;
		}

		// Generate the personal snap location for THIS pikmin
		FVector SnapPosition = GetPikminAttachPoint(i);

		// If Pikmin is already in Carrying state, force snap to exact position
		if (AI->GetState() == EPikminState::Working)
		{
			Pikmin->SetActorLocation(SnapPosition);
		}
		else
		{
			// Move AI toward the attach point
			AI->MoveToLocation(SnapPosition, 15.0f);

			// Check if we are close enough to start carrying behavior
			float Dist = FVector::Dist2D(SnapPosition, Pikmin->GetActorLocation());
			if (Dist <= 20.0f)
			{
				AI->SetState(EPikminState::Working);
				Pikmin->SetActorLocation(SnapPosition);
			}
		}
	}
}

// -------- Movement --------

void ACarryableObject::UpdateMovement()
{
	//if (!bIsMoving)
	//	return;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		float SpeedMultiplier = FMath::Clamp((float)AssignedPikmin.Num() / MaxPikmin, 0.3f, 1.0f);
		MovementComponent->MaxWalkSpeed = BaseMoveSpeed * SpeedMultiplier;
	}
}
