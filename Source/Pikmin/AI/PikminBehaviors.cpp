// Copyright 2025, Rolling Pixels. All Rights Reserved

#include "PikminBehaviors.h"
#include "PikminAIController.h"
#include "Pikmin/PikminCharacter.h"
#include "Systems/PikminTaskSubsystem.h"

namespace PikminBehaviors
{
	void Idle_Update(APikminAIController* Controller, float DeltaTime)
	{
		if (!Controller)
		{
			return;
		}

		// Keep movement stopped while idle
		APikminCharacter* Pikmin = Controller->GetControlledPikmin();
		if (!Pikmin)
		{
			return;
		}

		Pikmin->RequestStop();

		// Throttled scan for nearby tasks
		Controller->TimeSinceLastScan += DeltaTime;
		if (Controller->TimeSinceLastScan >= Controller->ScanCooldown)
		{
			Controller->TimeSinceLastScan = 0.0f;

			if (auto TaskSubsystem = Controller->GetWorld()->GetGameInstance()->GetSubsystem<UPikminTaskSubsystem>())
			{
				const FVector MyLoc = Pikmin->GetActorLocation();

				TScriptInterface<IPikminTaskInteractable> BestTask;
				float BestDistSq = FLT_MAX;

				for (auto& Task : TaskSubsystem->Tasks)
				{
					if (!Task) continue;

					FVector TaskLoc = Task->Execute_GetTaskLocation(Task.GetObject());
					float DistSq = FVector::DistSquared(MyLoc, TaskLoc);

					// Within search radius?
					if (DistSq > Controller->TaskSearchRadius * Controller->TaskSearchRadius) continue;

					// Can accept?
					if (!Task->Execute_CanAcceptPikmin(Task.GetObject(), Pikmin)) continue;

					if (DistSq < BestDistSq)
					{
						BestDistSq = DistSq;
						BestTask = Task;
					}
				}

				if (BestTask)
				{
					Controller->AssignToTask(BestTask);
					Controller->SetState(EPikminState::Working);
				}
			}
		}
	}

	void Follow_Update(APikminAIController* Controller, float DeltaTime)
	{
		if (!Controller)
		{
			return;
		}

		APikminCharacter* Pikmin = Controller->GetControlledPikmin();
		if (!Pikmin || !Controller->FollowTarget) return;

		const FVector Target = Controller->FollowTarget->GetComponentLocation();
		const float Dist = FVector::Dist2D(Target, Pikmin->GetActorLocation());
		if (Dist > Pikmin->MoveAcceptanceRadius)
		{
			Pikmin->RequestMoveTo(Target, Pikmin->MoveAcceptanceRadius);
		}
		else
		{
			Pikmin->RequestStop();
		}
	}

	void Working_Update(APikminAIController* Controller, float DeltaTime)
	{
		if (!Controller)
		{
			return;
		}

		APikminCharacter* Pikmin = Controller->GetControlledPikmin();
		if (!Pikmin || !Controller->GetActiveTask()) return;

		const FVector TaskLoc = Controller->GetActiveTask()->Execute_GetTaskLocation(Controller->GetActiveTask().GetObject());
		const float Dist = FVector::Dist2D(TaskLoc, Pikmin->GetActorLocation());

		// Move to the task's attach area center. Task will provide finer attach-slot when Pikmin arrives.
		if (Dist > Pikmin->MoveAcceptanceRadius)
		{
			Pikmin->RequestMoveTo(TaskLoc, Pikmin->MoveAcceptanceRadius);
		}
		else
		{
			// Arrived at task region — tell the task actor to accept/slot this pikmin.
			// The Task's AssignPikmin should set up slot index and the Pikmin will request the precise slot location.
			// Here we call the interface to confirm/push the pikmin into the task's internal state machine.
			IPikminTaskInteractable::Execute_AssignPikmin(Controller->GetActiveTask().GetObject(), Pikmin);
			// Expect the Task to respond by providing slot location via its own API (e.g., SetSlotForPikmin)
		}
	}

	void Thrown_Update(APikminAIController* Controller, float DeltaTime)
	{
		// Thrown state is handled by the character's throw motion; controller stays idle or monitors landing.
		if (!Controller)
		{
			return;
		}

		APikminCharacter* Pikmin = Controller->GetControlledPikmin();
		if (!Pikmin)
		{
			return;
		}

		// Nothing for now. When character calls OnThrownLanded(), controller resets to Idle and will scan.
	}
}
