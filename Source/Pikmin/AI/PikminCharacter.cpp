// Fill out your copyright notice in the Description page of Project Settings.

#include "PikminCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PikminAIController.h"
#include "Components/CapsuleComponent.h"

APikminCharacter::APikminCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	AIControllerClass = APikminAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::Spawned;
}

void APikminCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsThrown)
	{
		return;
	}

	ThrowTime += DeltaTime;

	float Alpha = ThrowTime / ThrowDuration;
	if (Alpha >= 1.0f)
	{
		bIsThrown = false;
		OnThrowLanded();
		return;
	}

	// Horizontal Lerp
	FVector Pos = FMath::Lerp(ThrowStart, ThrowEnd, Alpha);

	// Add arc height using parabola
	float Height = ThrowHeight * (1 - FMath::Pow((Alpha * 2 - 1), 2));
	Pos.Z += Height;

	SetActorLocation(Pos);
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

void APikminCharacter::BeginThrow(const FVector& Target, AActor* Thrower)
{
	ThrowStart = GetActorLocation();
	ThrowEnd = Target;
	ThrowTime = 0.f;
	bIsThrown = true;

	// Tell AI it is thrown
	if (APikminAIController* AI = Cast<APikminAIController>(GetController()))
	{
		AI->SetState(EPikminState::Thrown);
	}

	// Disable movement controller
	GetCharacterMovement()->DisableMovement();
	SetActorEnableCollision(false);
}

void APikminCharacter::OnThrowLanded()
{
	SetActorEnableCollision(true);

	// Re-enable movement
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// Tell AI it’s free again
	if (APikminAIController* AI = Cast<APikminAIController>(GetController()))
	{
		AI->OnThrownLanded();
	}
}
