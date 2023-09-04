// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/AG_AnimInstance.h"
#include "ActionGas/ActionGasCharacter.h"
#include "DataAssets/CharacterAnimDataAsset.h"

UBlendSpace* UAG_AnimInstance::GetLocomotionBlendSpace() const
{
	if (AActionGasCharacter*  ActionGasCharacter=Cast<AActionGasCharacter>(GetOwningActor()))
	{
		FCharacterData Data = ActionGasCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.MovementBlendSpace;
		}
	}

	return DefaultCharacterAnimDataAsset?DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBlendSpace:nullptr;
}

UAnimSequence* UAG_AnimInstance::GetIdleAnimation() const
{
	if (AActionGasCharacter* ActionGasCharacter = Cast<AActionGasCharacter>(GetOwningActor()))
	{
		FCharacterData Data = ActionGasCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimAsset;
		}
	}
	return DefaultCharacterAnimDataAsset?DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimAsset:nullptr;
}

UBlendSpace* UAG_AnimInstance::GetCrouchLocomotionBlendSpace() const
{
	if (AActionGasCharacter*  ActionGasCharacter=Cast<AActionGasCharacter>(GetOwningActor()))
	{
		FCharacterData Data = ActionGasCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendSpace;
		}
	}

	return DefaultCharacterAnimDataAsset?DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendSpace:nullptr;
}

UAnimSequence* UAG_AnimInstance::GetCrouchAnimation() const
{
	if (AActionGasCharacter* ActionGasCharacter = Cast<AActionGasCharacter>(GetOwningActor()))
	{
		FCharacterData Data = ActionGasCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimAsset;
		}
	}
	return DefaultCharacterAnimDataAsset?DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimAsset:nullptr;
}
