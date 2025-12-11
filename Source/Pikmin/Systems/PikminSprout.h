// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PikminTypes.h"
#include "PikminSelectable.h"
#include "PikminSprout.generated.h"

UCLASS()
class PIKMIN_API APikminSprout : public AActor, public IPikminSelectable
{
	GENERATED_BODY()
	
public:	
	APikminSprout();

protected:
	virtual void BeginPlay() override;

public:
	// --- Settings ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprout")
	EPikminType PikminType = EPikminType::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprout")
	float GrowTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprout")
	bool bAutoAlignToGround = true;

	// --- State ---
	bool bIsPluckable;

	// --- Interface functions ---
	virtual bool IsPluckable_Implementation(AActor* Caller) override { return bIsPluckable; };
	virtual void OnPluck_Implementation(AActor* Caller) override;

protected:
	void BecomePluckable();

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SproutMesh;

	FTimerHandle SproutTimerHandle;
};
