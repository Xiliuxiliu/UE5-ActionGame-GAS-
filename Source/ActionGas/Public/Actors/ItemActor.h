// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

class UInventoryItemInstance;

UCLASS()
class ACTIONGAS_API AItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemActor();

	void Init(UInventoryItemInstance* InInstance);
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	virtual void OnEquipped();
	virtual void OnUnEquipped();
	virtual void OnDropped();
	
protected:
	UPROPERTY(Replicated)
	UInventoryItemInstance* ItemInstance = nullptr;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
};
