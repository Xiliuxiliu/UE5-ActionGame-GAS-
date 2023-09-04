// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_GameplayAbility.h"

#include "ActionGas/ActionGasCharacter.h"

void UGA_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	FGameplayEffectContextHandle EffectContextHandle = ActorInfo->AbilitySystemComponent->MakeEffectContext();
	
	for (auto Effect:OngoingEffectsJustApplyOnStart)
	{
		if (!Effect.Get())
		{
			continue;
		}
		if (UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect,1,EffectContextHandle);

			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				if (!ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					UE_LOG(LogTemp,Warning,TEXT("Ability %s failed to apply startup effect %s"),*GetName(),*GetNameSafe(Effect));
				}
				else
				{
					UE_LOG(LogTemp,Warning,TEXT("Ability %s succeed to apply startup effect %s"),*GetName(),*GetNameSafe(Effect));
				}
			}
		}
	}

	//判断是否已经实例化，如真为true，对于蓝图始终为true
	if (IsInstantiated())
	{
		UE_LOG(LogTemp,Warning,TEXT("stage 1 Instantiated,Loop for add Effect"));
		for (auto GameplayEffect:OngoingEffectsToRemoveOnEnd)
		{
			if (!GameplayEffect.Get())
			{
				UE_LOG(LogTemp,Warning,TEXT("Get GameplayEffect failed"));
				continue;
			}

			UE_LOG(LogTemp,Warning,TEXT("stage 2 Instantiated,Loop for add Effect"));
			if (UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
			{
				FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect,1,EffectContextHandle);
				//FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName());
				//AbilitySystemComponent->AddReplicatedLooseGameplayTag(Tag);

				if (SpecHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					if (!ActiveGameplayEffectHandle.WasSuccessfullyApplied())
					{
						UE_LOG(LogTemp,Warning,TEXT("Ability %s failed to apply startup effect %s"),*GetName(),*GetNameSafe(GameplayEffect));
					}
					else
					{
						RemoveOnEffectHandles.Add(ActiveGameplayEffectHandle);
						UE_LOG(LogTemp,Warning,TEXT("Ability %s add to apply startup effect %s success"),*GetName(),*GetNameSafe(GameplayEffect));
					}
				}
			}
			
		}
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("in ActivateAbility,IsInstantiated failed"));
	}
	
}

void UGA_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsInstantiated())
	{
		for (FActiveGameplayEffectHandle ActiveGEHandle : RemoveOnEffectHandles)
		{
			if (ActiveGEHandle.IsValid())
			{
				ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGEHandle);
			}
		}
		RemoveOnEffectHandles.Empty();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AActionGasCharacter* UGA_GameplayAbility::GetActionGameCharacterFromActorInfo() const
{
	return Cast<AActionGasCharacter>(GetAvatarActorFromActorInfo());
}
