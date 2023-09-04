// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGametypes.generated.h"

class AItemActor;

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="GAS")
	TArray<TSubclassOf<class UGameplayEffect>> Effects;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="GAS")
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;

	//动画资产
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Animation")
	class UCharacterAnimDataAsset* CharacterAnimDataAsset;
	
};

USTRUCT(BlueprintType)
struct FCharacterAnimationData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	class UBlendSpace* MovementBlendSpace=nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UAnimSequence* IdleAnimAsset = nullptr;

	//蹲伏
	UPROPERTY(EditDefaultsOnly)
	class UBlendSpace* CrouchMovementBlendSpace=nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UAnimSequence* CrouchIdleAnimAsset = nullptr;
	
};

UENUM(BlueprintType)
enum class EFoot:uint8
{
	LEFT UMETA(DisplayName = "Left"),
	RIGHT UMETA(DisplayName = "Right"),
};

USTRUCT()
struct FMotionWarpingTargetByLocationAndRotation
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	FName Name;

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	FMotionWarpingTargetByLocationAndRotation()=default;

	FMotionWarpingTargetByLocationAndRotation(FName InName,FVector InLocation,FRotator InRotation)
		:Name(InName)
	,Location(InLocation)
	,Rotation(InRotation)
	{
		
	};
};

UCLASS(Blueprintable,BlueprintType)
class UItemStaticData:public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FName Name;

	//指定道具蓝图
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<AItemActor> ItemActorClass;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FName AttachmentSocket = NAME_None;

public:
	UFUNCTION(BlueprintCallable)
	void SetItemName(FName InName){Name = InName;}
};

