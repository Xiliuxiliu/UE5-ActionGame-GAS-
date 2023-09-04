// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/InventoryComponent.h"

#include "ActionGas/ActionGameTypes.h"
#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"


static TAutoConsoleVariable<int32> CVarShowInventory(TEXT("ShowDebugInventory"),
	0,
	TEXT("Draw debug info about inventory/n 0:off/n 1:on/n"),
	ECVF_Cheat);

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//设置为true，则会调用InitializeComponent
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	//默认装备第一个物品
	if (InventoryList.GetItemRef().Num())
	{
		EquipItem(InventoryList.GetItemRef()[0].ItemInstance->ItemStaticDataClass);
	}

	// UnEquipItem();
	DropItem();
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();
	if (GetOwner()->GetLocalRole()==ROLE_Authority)
	{
		for (auto ItemClass : DefaultItems)
		{
			InventoryList.AddItem(ItemClass);
		}
	}
	
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FInventoryListItem Item :InventoryList.GetItemRef())
	{
		UInventoryItemInstance* ItemInstance = Item.ItemInstance;
		if (IsValid(ItemInstance))
		{
			WroteSomething |= Channel->ReplicateSubobject(ItemInstance,*Bunch,*RepFlags);
		}
	}
	return WroteSomething;
}

void UInventoryComponent::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	if (GetOwner()->HasAuthority())
	{
		InventoryList.AddItem(InItemStaticDataClass);
	}
}

void UInventoryComponent::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	if (GetOwner()->HasAuthority())
	{
		InventoryList.RemoveItem(InItemStaticDataClass);
	}
}

UInventoryItemInstance* UInventoryComponent::GetEquippedItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	return CurrentItem;
}

void UInventoryComponent::EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	if (GetOwner()->HasAuthority())
	{
		for (FInventoryListItem Item:InventoryList.GetItemRef())
		{
			UInventoryItemInstance* ItemInstance = Item.ItemInstance;
			if (IsValid(ItemInstance) && ItemInstance->ItemStaticDataClass == InItemStaticDataClass)
			{
				Item.ItemInstance->OnEquipped(GetOwner());
				CurrentItem = Item.ItemInstance;
				break;
			}
		}
	}
}

void UInventoryComponent::UnEquipItem()
{
	if (GetOwner()->HasAuthority())
	{
		if (IsValid(CurrentItem))
		{
			CurrentItem->OnUnEquipped();
			CurrentItem = nullptr;
		}
	}
}

void UInventoryComponent::DropItem()
{
	if (GetOwner()->HasAuthority())
	{
		if (IsValid(CurrentItem))
		{
			CurrentItem->OnDropped();
			RemoveItem(CurrentItem->ItemStaticDataClass);
			CurrentItem = nullptr;
		}
	}
}


void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	const bool bShowDebug = CVarShowInventory.GetValueOnGameThread()!=0;
	if (bShowDebug)
	{
		for (FInventoryListItem Item :InventoryList.GetItemRef())
		{
			UInventoryItemInstance* ItemInstance = Item.ItemInstance;
			if (IsValid(ItemInstance))
			{
				const UItemStaticData* ItemStaticData = ItemInstance->GetItemStaticData();
				if (GetOwner()->GetLocalRole()==ROLE_Authority)
				{
					GEngine->AddOnScreenDebugMessage(-1,0,FColor::Blue,FString::Printf(TEXT("Server item:%s"),*ItemStaticData->Name.ToString()));
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1,0,FColor::Blue,FString::Printf(TEXT("Client item:%s"),*ItemStaticData->Name.ToString()));
				}
			}
		}
	}
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent,InventoryList);
	DOREPLIFETIME(UInventoryComponent,CurrentItem);
}

