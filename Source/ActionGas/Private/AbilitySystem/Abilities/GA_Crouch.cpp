// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Crouch.h"

#include "ActionGas/ActionGasCharacter.h"
#include "Animation/AnimAttributes.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Crouch::UGA_Crouch()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool UGA_Crouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if(const AActionGasCharacter* Character = CastChecked<AActionGasCharacter>(ActorInfo->AvatarActor.Get(),ECastCheckedType::NullAllowed))
	{
		return Character->CanCrouch();
	}
	
	return false;
}

void UGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if(AActionGasCharacter* Character = CastChecked<AActionGasCharacter>(ActorInfo->AvatarActor.Get(),ECastCheckedType::NullAllowed))
	{
		UCharacterMovementComponent* MovementComponent = CastChecked<UCharacterMovementComponent>(Character->GetCharacterMovement());

		if (MovementComponent)
		{
			MovementComponent->bUseControllerDesiredRotation = true;
			MovementComponent->bOrientRotationToMovement = false;
		}
		Character->Crouch();
	}
}

void UGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if(AActionGasCharacter* Character = CastChecked<AActionGasCharacter>(ActorInfo->AvatarActor.Get(),ECastCheckedType::NullAllowed))
	{
		UCharacterMovementComponent* MovementComponent = CastChecked<UCharacterMovementComponent>(Character->GetCharacterMovement());
		if (MovementComponent)
		{
			MovementComponent->bUseControllerDesiredRotation = false;
			MovementComponent->bOrientRotationToMovement = true;
		}
		Character->UnCrouch();
	}
}

bool UGA_Crouch::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
	{
		return false;
	}


	/*
	 *if (AActionGasCharacter* Character = Cast<AActionGasCharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (const UAG_AttributeSetBase* AttributeSetBase = Cast<UAG_AttributeSetBase>(Character->GetAttributeSetBase()))
		{
			const float CurrentStamina =AttributeSetBase->GetStamina();
			if (CurrentStamina < JumpStaminaCost)
			{
				UE_LOG(LogAbilitySystemComponent,Warning,TEXT("Stamina can't afford it "));
				return true;
			}
		}
	}
	UE_LOG(LogAbilitySystemComponent,Warning,TEXT("Stamina can afford it"));
	*/
	return true;
}

