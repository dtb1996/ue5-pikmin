// Fill out your copyright notice in the Description page of Project Settings.


#include "PikminTaskSubsystem.h"

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
