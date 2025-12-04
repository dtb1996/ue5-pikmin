// Fill out your copyright notice in the Description page of Project Settings.

#include "PikminCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PikminAIController.h"

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

}
