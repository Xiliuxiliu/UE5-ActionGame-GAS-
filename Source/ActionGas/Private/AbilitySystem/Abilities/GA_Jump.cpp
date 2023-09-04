// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Jump.h"
#include "ActionGas/ActionGasCharacter.h"

UGA_Jump::UGA_Jump()
{
	//实时同步
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	//不创建新实例，所有实例都是用同一份，适合不需要创建特定的ability
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
                                  FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	const AActionGasCharacter* Character = CastChecked<AActionGasCharacter>(ActorInfo->AvatarActor.Get(),ECastCheckedType::NullAllowed);
	return Character->CanJump();
}

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(OwnerInfo,&ActivationInfo))
	{
		if (!CommitAbility(Handle,OwnerInfo,ActivationInfo))
		{
			return;
		}
		Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);
		
		AActionGasCharacter* Character = CastChecked<AActionGasCharacter>(OwnerInfo->AvatarActor.Get(),ECastCheckedType::NullAllowed);
		if (Character)
		{
			UE_LOG(LogTemp,Warning,TEXT("ready to ActivateAbility jump"));
			Character->Jump();
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("in ActivateAbility, Character is null"));
		}
	
	}
	
	
}
