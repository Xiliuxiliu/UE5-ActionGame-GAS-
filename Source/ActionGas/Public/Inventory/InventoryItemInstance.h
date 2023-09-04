// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemInstance.generated.h"


class UItemStaticData;
class AItemActor;

UCLASS()
class ACTIONGAS_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(Replicated)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;

	UPROPERTY(ReplicatedUsing = OnRep_Equipped)
	bool bEquipped = false;

	UPROPERTY(Replicated)
	AItemActor* ItemActor = nullptr;
	
public:
	virtual void Init(TSubclassOf<UItemStaticData> InitemStaticDataClass);

	virtual bool IsSupportedForNetworking() const override{return  true;}

	UFUNCTION(BlueprintCallable,BlueprintPure)
	const UItemStaticData* GetItemStaticData()const;

	UFUNCTION()
	void OnRep_Equipped();

	virtual void OnEquipped(AActor* InOwner = nullptr);
	virtual void OnUnEquipped(AActor* InOwner = nullptr);
	virtual void OnDropped(AActor* InOwner = nullptr);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifeTimeProps) const override;

	
	
};
