// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/AG_CharacterMovementComponent.h"

#include "AbilitySystemComponent.h"

static TAutoConsoleVariable<int32> CVarShowTravelsal(TEXT("ShowDebugTravelsal"),
													 0,
													 TEXT("Draw debug info about Travelsal/n   0: off/n  1:on/n"),
													 ECVF_Cheat);

bool UAG_CharacterMovementComponent::TryTravelsal(UAbilitySystemComponent* ASC)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass:TravelsAbilitiesOrdered)
	{
		if (ASC->TryActivateAbilityByClass(AbilityClass,true))
		{
			FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityClass);
			if (Spec && Spec->IsActive())
			{
				return true;
			}
		}
	}
	return false;
}
