// Fill out your copyright notice in the Description page of Project Settings.

#include "PikminCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PikminAIController.h"
//#include "Components/CapsuleComponent.h"

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

void APikminCharacter::AttachToTaskActor(AActor* TaskActor, const FVector& WorldAttachLocation)
{
	if (!TaskActor)
	{
		return;
	}

	// Ensure we stop AI movement and lock transform
	RequestStop();
	CurrentTaskActor = TaskActor;

	// Disable character movement and collision while attached
	GetCharacterMovement()->DisableMovement();
	SetActorLocation(WorldAttachLocation);
	AttachToActor(TaskActor, FAttachmentTransformRules::KeepWorldTransform);
}

void APikminCharacter::DetachFromTaskActor()
{
	// Restore
	if (CurrentTaskActor.IsValid())
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentTaskActor.Reset();
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		// Notify controller that we're idle again
		if (APikminAIController* AI = Cast<APikminAIController>(GetController()))
		{
			AI->HandleTaskComplete();
		}
	}
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
	DetachFromTaskActor();
}
