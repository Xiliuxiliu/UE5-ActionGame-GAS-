// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemActor.h"

#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"


AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AItemActor::Init(UInventoryItemInstance* InInstance)
{
	if (HasAuthority())
	{
		ItemInstance = InInstance;
	}
}

bool AItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSometing =  Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	if (IsValid(ItemInstance))
	{
		WroteSometing |= Channel->ReplicateSubobject(ItemInstance,*Bunch,*RepFlags);
	}
	return  WroteSometing;
}

void AItemActor::OnEquipped()
{
}

void AItemActor::OnUnEquipped()
{
}

void AItemActor::OnDropped()
{
	//物品分离
	GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	
	if (AActor* ActorOwner = GetOwner())
	{
		const FVector Location = GetActorLocation();
		const FVector Forward = ActorOwner->GetActorForwardVector();

		const float DropItemDist = 100.f;
		const float DropItemTraceDist = 10000.f;

		const FVector TraceStart = Location + Forward*DropItemDist;
		const FVector TracEnd = TraceStart  - FVector::UpVector*DropItemTraceDist;

		TArray<AActor*> ActorsToIgnore = {ActorOwner};
		FHitResult TraceHit;

		static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugInventory"));
		const bool bShowTraversal = CVar->GetInt() > 0;
		EDrawDebugTrace::Type DebugDrawType = bShowTraversal?EDrawDebugTrace::ForDuration:EDrawDebugTrace::None;

		if (UKismetSystemLibrary::LineTraceSingleByProfile(this,
			TraceStart,
			TracEnd,
			TEXT("WorldStatic"),
			true,
			ActorsToIgnore,
			DebugDrawType,
			TraceHit,
			true))
		{
			if (TraceHit.bBlockingHit)
			{
				SetActorLocation(TraceHit.Location+FVector::UpVector*50);
				return;
			}
			SetActorLocation(TraceHit.Location+FVector::UpVector*50);
			
		}
	}
}


void AItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}


void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemActor,ItemInstance);
}

