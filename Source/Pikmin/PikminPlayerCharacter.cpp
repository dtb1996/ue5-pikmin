// Copyright Epic Games, Inc. All Rights Reserved.

#include "PikminPlayerCharacter.h"
#include "Camera/CameraRig.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Systems/PikminManagerSubsystem.h"
#include "AI/PikminCharacter.h"
#include "AI/PikminAIController.h"
#include "Systems/PikminWhistleComponent.h"
#include "Systems/PikminThrowTargetComponent.h"
#include "Systems/PikminInteractionComponent.h"
#include "Player/PikminPlayerAnimInstance.h"

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

void APikminPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    //// Optionally spawn and attach a camera rig automatically
    //if (CameraRigClass)
    //{
    //    SpawnedCameraRig = SpawnAndAttachCameraRig();
    //    if (SpawnedCameraRig)
    //    {
    //        SpawnedCameraRig->SetFollowTarget(this);
    //        

    //        if (APlayerController* PC = Cast<APlayerController>(GetController()))
    //        {
    //            // Switch the active camera to the camera rig
    //            PC->SetViewTarget(SpawnedCameraRig);
    //        }
    //    }
    //}
}

ACameraRig* APikminPlayerCharacter::SpawnAndAttachCameraRig()
{
    if (!CameraRigClass) return nullptr;
    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = GetInstigator();
    ACameraRig* Rig = GetWorld()->SpawnActor<ACameraRig>(CameraRigClass, GetActorLocation(), GetActorRotation(), Params);
    if (Rig)
    {
        // Attach so it moves with pawn if desired, or leave separate if you want independent smoothing
        Rig->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
        Rig->SetFollowTarget(this);
    }
    return Rig;
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

void APikminPlayerCharacter::CommandThrow()
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

    if (UPikminPlayerAnimInstance* Anim = GetPikminAnimInstance())
    {
        Anim->PlayThrowMontage();
    }

    // Throw Pikmin
    UPikminManagerSubsystem* PikminManager = GetGameInstance()->GetSubsystem<UPikminManagerSubsystem>();
    APikminCharacter* Pikmin = PikminManager->GetNextThrowablePikmin(this);

    if (!Pikmin)
    {
        return;
    }

    const FVector TargetLocation = ThrowTargetComponent->GetTargetLocation();
    Pikmin->BeginThrowTo(TargetLocation);
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

UPikminPlayerAnimInstance* APikminPlayerCharacter::GetPikminAnimInstance() const
{
    if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
    {
        return Cast<UPikminPlayerAnimInstance>(SkeletalMesh->GetAnimInstance());
    }
    
    return nullptr;
}
