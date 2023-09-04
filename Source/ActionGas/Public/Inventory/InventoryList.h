// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryList.generated.h"

class UInventoryItemInstance;
class UItemStaticData;

USTRUCT(BlueprintType)
struct FInventoryListItem : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()
	
public:
	UPROPERTY()
	UInventoryItemInstance* ItemInstance = nullptr;
};

USTRUCT()
struct FInventoryList: public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()
	
protected:
	UPROPERTY()
	TArray<FInventoryListItem>	Items;
	
public:
	/** Step 4: Copy this, replace example with your names */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryListItem, FInventoryList>( Items, DeltaParms, *this );
	}

	void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
	void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
	TArray<FInventoryListItem>& GetItemRef();
};

template<>
struct TStructOpsTypeTraits< FInventoryList > : public TStructOpsTypeTraitsBase2< FInventoryList >
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};


