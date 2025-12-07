// Fill out your copyright notice in the Description page of Project Settings.

#include "CarryableObject.h"
#include "AI/PikminCharacter.h"
#include "AI/PikminAIController.h"
#include "Systems/PikminTaskSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ACarryableObject::ACarryableObject()
{
	PrimaryActorTick.bCanEverTick = true;
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

	//DeliveryTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
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
	UpdateMovement(DeltaTime);
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
	//UpdatePikminPositions();

	// Check if we can start movement
	if (!bIsMoving && AssignedPikmin.Num() >= RequiredPikmin)
	{
		bIsMoving = true;
	}
}

void ACarryableObject::UnassignPikmin_Implementation(APikminCharacter* Pikmin)
{
	AssignedPikmin.Remove(Pikmin);
	//UpdatePikminPositions();

	if (AssignedPikmin.Num() < RequiredPikmin)
	{
		bIsMoving = false;
	}
}

FVector ACarryableObject::GetTaskLocation_Implementation() const
{
	return GetActorLocation();
}

void ACarryableObject::HandleDelivered()
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

// -------- Pikmin Arrange Logic --------

FVector ACarryableObject::GetPikminAttachPoint(int32 Index) const
{
	float Angle = (360.0f / FMath::Max(1, AssignedPikmin.Num())) * Index;
	FVector Offset = FVector(FMath::Cos(FMath::DegreesToRadians(Angle)), FMath::Sin(FMath::DegreesToRadians(Angle)), 0) * AttachRadius;
	
	return GetActorLocation() + Offset;
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
			AI->MoveToLocation(SnapPosition, 15.f);

			// Check if we are close enough to start carrying behavior
			float Dist = FVector::Dist2D(SnapPosition, Pikmin->GetActorLocation());
			if (Dist <= 20.f)
			{
				AI->SetState(EPikminState::Working);
				Pikmin->SetActorLocation(SnapPosition);
			}
		}
	}
}

// -------- Movement --------

void ACarryableObject::UpdateMovement(float DeltaTime)
{
	if (!bIsMoving || !DeliveryTarget)
	{
		return;
	}

	float SpeedMultiplier = (float)AssignedPikmin.Num() / MaxPikmin;
	float MoveSpeed = BaseMoveSpeed * FMath::Max(0.2f, SpeedMultiplier);

	FVector Direction = (DeliveryTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	SetActorLocation(GetActorLocation() + Direction * MoveSpeed * DeltaTime);

	float Distance = FVector::Dist2D(GetActorLocation(), DeliveryTarget->GetActorLocation());

	if (Distance < 100.f)
	{
		// Arrived at zone
		bIsMoving = false;

		// TODO: trigger scoring, destroy, spawn seeds, etc
	}
}
