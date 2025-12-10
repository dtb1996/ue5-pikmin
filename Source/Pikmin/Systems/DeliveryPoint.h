// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemTypes.h"
#include "DeliveryPoint.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class PIKMIN_API ADeliveryPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ADeliveryPoint();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
	EItemType ItemTypeToDeliver;

	UFUNCTION(BlueprintCallable)
	virtual void HandleItemDelivered(EItemType ItemType, int32 PikminYield) { };

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
