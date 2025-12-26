// Copyright 2025, Rolling Pixels. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PikminInteractionComponent.generated.h"

class USphereComponent;
class APikminPlayerCharacter;

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent) )
class PIKMIN_API UPikminInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPikminInteractionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/** Called by player input */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryPluck();

	/** Allows UI queries */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetCurrentSelectable() const { return CurrentSelectable; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OnSelectableBecamePluckable(AActor* Selectable);

private:
	/** Automatically created interaction trigger */
	UPROPERTY()
	USphereComponent* InteractionSphere;

	/** Who owns this component */
	APikminPlayerCharacter* OwnerPlayer = nullptr;

	/** Current pluckable actor */
	UPROPERTY()
	AActor* CurrentSelectable = nullptr;

	/** Track all overlapped pluckable actors */
	UPROPERTY()
	TSet<AActor*> OverlappingSelectables;

	/** Re-evaluates Best Selectable */
	void UpdateBestSelectable();

	AActor* GetBestSelectable() const;

	UFUNCTION()
	void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
