// Copyright 2025, Rolling Pixels. All Rights Reserved

#include "PikminCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/PikminAIController.h"
#include "Components/CapsuleComponent.h"

APikminCharacter::APikminCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	AIControllerClass = APikminAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::Spawned;
}

void APikminCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle throw motion if active
	if (ThrowMotion.bActive)
	{
		ThrowMotion.Time += DeltaTime;
		float Alpha = ThrowMotion.Duration > 0.0f ? (ThrowMotion.Time / ThrowMotion.Duration) : 1.0f;
		if (Alpha >= 1.0f)
		{
			// Finalize
			SetActorLocation(ThrowMotion.End);
			ThrowMotion.Reset();
			OnThrowFinish();
		}
		else
		{
			// Linear horizontal lerp
			FVector Pos = FMath::Lerp(ThrowMotion.Start, ThrowMotion.End, Alpha);
			// Parabolic height
			float H = ThrowMotion.Height * (1.0f - FMath::Pow((Alpha * 2.0f - 1.0f), 2.0f));
			Pos.Z += H;
			SetActorLocation(Pos);
		}
	}

	if (APikminAIController* AI = Cast<APikminAIController>(GetController()))
	{
		if (AI->GetState() == EPikminState::Working && CurrentTaskActor.IsValid())
		{
			CarryAnimSpeed = CurrentTaskActor->GetVelocity().Size2D();
		}
		else
		{
			CarryAnimSpeed = GetVelocity().Size2D();
		}
	}
}

void APikminCharacter::OnWhistleSelect_Implementation(AActor* Caller)
{
	if (APikminAIController* AI = Cast<APikminAIController>(GetController()))
	{
		AI->RequestFollow(Caller);
	}
}

void APikminCharacter::OnWhistleDeselect_Implementation(AActor* Caller)
{
	if (APikminAIController* AI = Cast<APikminAIController>(GetController()))
	{
		AI->RequestIdle();
	}
}

void APikminCharacter::RequestMoveTo(const FVector& Location, float AcceptanceRadius)
{
	if (APikminAIController* AI = Cast<APikminAIController>(GetController()))
	{
		AI->MoveToLocation(Location, AcceptanceRadius);
	}
}

void APikminCharacter::RequestStop()
{
	if (APikminAIController* AI = Cast<APikminAIController>(GetController()))
	{
		AI->StopMovement();
	}
}

void APikminCharacter::HandleAttachToTaskActor(AActor* TaskActor, const FVector& WorldAttachLocation)
{
	if (!TaskActor)
	{
		return;
	}

	if (CurrentTaskActor != TaskActor)
	{
		CurrentTaskActor = TaskActor;
		return;
	}
}

void APikminCharacter::HandleDetachFromTaskActor()
{
	if (!CurrentTaskActor.IsValid())
	{
		return;
	}

	CurrentTaskActor = nullptr;
}

void APikminCharacter::BeginThrowTo(const FVector& Target, float Duration, float Height)
{
	// Setup throw
	ThrowMotion.bActive = true;
	ThrowMotion.Start = GetActorLocation();
	ThrowMotion.End = Target;
	ThrowMotion.Duration = Duration;
	ThrowMotion.Time = 0.0f;
	ThrowMotion.Height = Height;

	// Disable collisions while flying
	SetActorEnableCollision(false);
	GetCharacterMovement()->DisableMovement();

	// Tell AI controller
	if (APikminAIController* AI = Cast<APikminAIController>(GetController()))
	{
		AI->SetState(EPikminState::Thrown);
	}
}

void APikminCharacter::OnThrowFinish()
{
	// Re-enable physics/character movement
	SetActorEnableCollision(true);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// Inform controller
	if (APikminAIController* AI = Cast<APikminAIController>(GetController()))
	{
		AI->OnThrownLanded();
	}
}

bool APikminCharacter::IsBusy() const
{
	if (APikminAIController* AI = Cast<APikminAIController>(GetController()))
	{
		return AI->IsBusy();
	}

	return false;
}

EPikminState APikminCharacter::GetState() const
{
	if (APikminAIController* AI = Cast<APikminAIController>(GetController()))
	{
		return AI->GetState();
	}

	return EPikminState::Idle;
}

void APikminCharacter::OnTaskCompleted()
{
	if (APikminAIController* AI = Cast<APikminAIController>(GetController()))
	{
		AI->SetState(EPikminState::Idle);
	}

	// Detach if attached and notify controller
	HandleDetachFromTaskActor();
}
