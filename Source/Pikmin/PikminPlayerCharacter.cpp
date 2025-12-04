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
#include "AI/PikminState.h"
#include "AI/PikminAIController.h"

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
            AICon->SetLeaderFollowTarget(FollowLocationComponent);
            AICon->SetState(EPikminState::Following);
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
            AICon->SetLeaderFollowTarget(nullptr);
            AICon->SetState(EPikminState::Idle);
        }
    }
}
