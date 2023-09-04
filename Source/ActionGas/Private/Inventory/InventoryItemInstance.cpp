// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemInstance.h"

#include "ActionGameStatic.h"
#include "ActionGas/ActionGasCharacter.h"
#include "Actors/ItemActor.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	ItemStaticDataClass = InItemStaticDataClass;
}

const UItemStaticData* UInventoryItemInstance::GetItemStaticData() const
{
	return UActionGameStatic::GetItemStaticData(ItemStaticDataClass);
}

void UInventoryItemInstance::OnRep_Equipped()
{
}

void UInventoryItemInstance::OnEquipped(AActor* InOwner)
{
	if (InOwner->GetWorld())
	{
		const UItemStaticData* StaticData = GetItemStaticData();
		
		FTransform Transform ;
		//Transform.SetScale3D(FVector(0.2f));
		ItemActor = InOwner->GetWorld()->SpawnActorDeferred<AItemActor>(StaticData->ItemActorClass,Transform,InOwner);
		ItemActor->Init(this);
		ItemActor->OnEquipped();
		ItemActor->FinishSpawning(Transform);

		if (AActionGasCharacter* Character = Cast<AActionGasCharacter>(InOwner))
		{
			if (USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh())
			{
				ItemActor->AttachToComponent(SkeletalMeshComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale,StaticData->AttachmentSocket);
			}
		}

		bEquipped = true;
	}
}

void UInventoryItemInstance::OnUnEquipped(AActor* InOwner)
{
	if (ItemActor)
	{
		ItemActor->Destroy();
		ItemActor = nullptr;
	}
	bEquipped = false;
}

void UInventoryItemInstance::OnDropped(AActor* InOwner)
{
	if (ItemActor)
	{
		ItemActor->OnDropped();
	}
	bEquipped = false;
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass);
	DOREPLIFETIME(UInventoryItemInstance, ItemActor)
	DOREPLIFETIME_CONDITION_NOTIFY(UInventoryItemInstance,bEquipped,COND_None,REPNOTIFY_Always);
	
}
