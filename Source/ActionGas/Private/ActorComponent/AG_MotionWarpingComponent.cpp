// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/AG_MotionWarpingComponent.h"

#include "ActionGas/ActionGameTypes.h"

UAG_MotionWarpingComponent::UAG_MotionWarpingComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UAG_MotionWarpingComponent::SendWarpPointsToClients()
{
	if (GetOwnerRole()==ROLE_Authority)
	{
		TArray<FMotionWarpingTargetByLocationAndRotation> WapTargets;
		for (auto Target : WapTargets)//WarpTargets
		{
			FMotionWarpingTargetByLocationAndRotation MotionWarpingTarget(Target.Name,Target.Location,Target.Rotation);

			WapTargets.Add(MotionWarpingTarget);
		}
		MulticastSyncWarpPoints(WapTargets);
	}
}

void UAG_MotionWarpingComponent::MulticastSyncWarpPoints_Implementation(
	const TArray<FMotionWarpingTargetByLocationAndRotation>& Targets)
{
	for (const FMotionWarpingTargetByLocationAndRotation& Warp:Targets)
	{
		AddOrUpdateWarpTargetFromLocationAndRotation(Warp.Name,Warp.Location,FRotator(Warp.Rotation));
	}
}
