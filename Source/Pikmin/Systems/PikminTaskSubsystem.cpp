// Copyright 2025, Rolling Pixels. All Rights Reserved


#include "PikminTaskSubsystem.h"
#include "Tasks/CarryableObject.h"

TScriptInterface<IPikminTaskInteractable> UPikminTaskSubsystem::GetNearestAvailableTask(const FVector& Location)
{
	TScriptInterface<IPikminTaskInteractable> BestTask;
	float BestDistance = FLT_MAX;

	for (auto Task : Tasks)
	{
		if (!Task || !Task->Execute_CanAcceptPikmin(Task.GetObject(), nullptr))
		{
			continue;
		}

		float Distance = FVector::DistSquared(Location, Task->Execute_GetTaskLocation(Task.GetObject()));

		if (Distance < BestDistance)
		{
			BestDistance = Distance;
			BestTask = Task;
		}
	}

	return BestTask;
}

void UPikminTaskSubsystem::RegisterDropOff(EItemType Type, AActor* DropOff)
{
	DropOffLocations.Add(Type, DropOff);

	// Assign to any pending objects
	if (PendingAssignments.Contains(Type))
	{
		FCarryableObjectArrayWrapper& Pending = PendingAssignments[Type];

		for (auto CarryObject : Pending.CarryableObjects)
		{
			if (CarryObject.IsValid())
			{
				CarryObject->SetDeliveryTarget(DropOff);
			}
		}
	}
}

void UPikminTaskSubsystem::RequestDropOff(EItemType Type, ACarryableObject* CarryObject)
{
	if (DropOffLocations.Contains(Type))
	{
		CarryObject->SetDeliveryTarget(DropOffLocations[Type]);
	}
	else
	{
		PendingAssignments.FindOrAdd(Type).CarryableObjects.Add(CarryObject);
	}
}
