// Copyright Epic Games, Inc. All Rights Reserved.

#include "PikminPlayerCharacter.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Systems/PikminManagerSubsystem.h"
#include "Pikmin/PikminCharacter.h"
#include "AI/PikminAIController.h"
#include "Systems/PikminWhistleComponent.h"
#include "Systems/PikminThrowTargetComponent.h"
#include "Systems/PikminInteractionComponent.h"
#include "Animation/PikminPlayerAnimInstance.h"

APikminPlayerCharacter::APikminPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Character movement defaults
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

    // Camera boom
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 600.0f;
    CameraBoom->SetRelativeRotation(FRotator(-40.0f, 0.0f, 0.0f));
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bInheritPitch = false;
    CameraBoom->bInheritRoll = false;

    // Camera
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(CameraBoom);
    Camera->bUsePawnControlRotation = false;
    
    // Follow location
    FollowLocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FollowLocation"));
    FollowLocationComponent->SetupAttachment(RootComponent);
    FollowLocationComponent->SetRelativeLocation(FVector(-100.0f, 0.0f, -90.0f));

    // Whistle component
    WhistleComponent = CreateDefaultSubobject<UPikminWhistleComponent>(TEXT("WhistleComponent"));

    // Throw target component
    ThrowTargetComponent = CreateDefaultSubobject<UPikminThrowTargetComponent>(TEXT("ThrowTargetComponent"));

    // Interaction component
    InteractionComponent = CreateDefaultSubobject<UPikminInteractionComponent>(TEXT("InteractionComponent"));
}

void APikminPlayerCharacter::CommandFollow()
{
    UPikminManagerSubsystem* PikminManager = GetGameInstance()->GetSubsystem<UPikminManagerSubsystem>();

    for (APikminCharacter* Pikmin : PikminManager->GetPikmin())
    {
        if (!Pikmin)
        {
            continue;
        }

        // Get AI controller and tell it to start following
        if (APikminAIController* AICon = Cast<APikminAIController>(Pikmin->GetController()))
        {
            AICon->RequestFollow(this);
        }
    }
}

void APikminPlayerCharacter::CommandDismiss()
{
    UPikminManagerSubsystem* PikminManager = GetGameInstance()->GetSubsystem<UPikminManagerSubsystem>();

    for (APikminCharacter* Pikmin : PikminManager->GetPikmin())
    {
        if (!Pikmin)
        {
            continue;
        }

        // Get AI controller and tell it to start following
        if (APikminAIController* AICon = Cast<APikminAIController>(Pikmin->GetController()))
        {
            AICon->RequestIdle();
        }
    }
}

void APikminPlayerCharacter::CommandAim()
{
    // If pluckable sprout in range then pluck instead of throwing
    if (InteractionComponent && InteractionComponent->GetCurrentSelectable() != nullptr)
    {
        TryPluck();
        return;
    }

    if (!ThrowTargetComponent)
    {
        return;
    }

    ThrowTargetComponent->BeginMouseAim();
}

void APikminPlayerCharacter::CommandThrow()
{
    if (!ThrowTargetComponent)
    {
        return;
    }

    if (UPikminPlayerAnimInstance* Anim = GetPikminAnimInstance())
    {
        Anim->PlayThrowMontage();
    }

    // Throw Pikmin
    UPikminManagerSubsystem* PikminManager = GetGameInstance()->GetSubsystem<UPikminManagerSubsystem>();
    APikminCharacter* Pikmin = PikminManager->GetNextThrowablePikmin(this);

    if (Pikmin)
    {
        const FVector TargetLocation = ThrowTargetComponent->GetTargetLocation();
        Pikmin->BeginThrowTo(TargetLocation);
    }

    ThrowTargetComponent->EndMouseAim();
}

void APikminPlayerCharacter::TryPluck()
{
    InteractionComponent->TryPluck();
}

void APikminPlayerCharacter::StartWhistle()
{
    if (UPikminPlayerAnimInstance* Anim = GetPikminAnimInstance())
    {
        Anim->bIsWhistling = true;
    }

    if (WhistleComponent)
    {
        FVector Origin = GetActorLocation();

        if (ThrowTargetComponent)
        {
            Origin = ThrowTargetComponent->GetTargetLocation();
        }

        WhistleComponent->SetWhistleOrigin(Origin);
        WhistleComponent->StartWhistle();
    }
}

void APikminPlayerCharacter::EndWhistle()
{
    if (UPikminPlayerAnimInstance* Anim = GetPikminAnimInstance())
    {
        Anim->bIsWhistling = false;
    }

    if (WhistleComponent)
    {
        WhistleComponent->EndWhistle();
    }
}

void APikminPlayerCharacter::SetMovementEnabled(bool bEnabled)
{
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        if (bEnabled)
        {
            MoveComp->SetMovementMode(MOVE_Walking);
        }
        else
        {
            MoveComp->StopMovementImmediately();
            MoveComp->SetMovementMode(MOVE_None);
        }
    }
}

UPikminPlayerAnimInstance* APikminPlayerCharacter::GetPikminAnimInstance() const
{
    if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
    {
        return Cast<UPikminPlayerAnimInstance>(SkeletalMesh->GetAnimInstance());
    }
    
    return nullptr;
}
