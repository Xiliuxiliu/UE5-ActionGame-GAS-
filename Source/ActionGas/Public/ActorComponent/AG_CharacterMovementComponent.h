// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AG_CharacterMovementComponent.generated.h"


class UAbilitySystemComponent;

UCLASS()
class ACTIONGAS_API UAG_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	//能力列表
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> TravelsAbilitiesOrdered;

	bool TryTravelsal(UAbilitySystemComponent* ASC);
	
};
