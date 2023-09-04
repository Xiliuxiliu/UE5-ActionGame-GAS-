// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_Step.h"

#include "ActionGas/ActionGasCharacter.h"
#include "ActorComponent/FootStepsComponent.h"

void UAnimNotify_Step::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AActionGasCharacter* Character = Cast<AActionGasCharacter>(MeshComp->GetOwner()))
	{
		if (UFootStepsComponent* FootStepsComponent = Character->GetFootStepsComponent())
		{
			FootStepsComponent->HandleFootstep(Foot);
		}
	}
}
