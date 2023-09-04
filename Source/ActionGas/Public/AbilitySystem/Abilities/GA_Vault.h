// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GA_GameplayAbility.h"
#include "GA_Vault.generated.h"


class UAbilityTask_PlayMontageAndWait;
UCLASS()
class ACTIONGAS_API UGA_Vault : public UGA_GameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Vault();
	
	//检查玩家是否处于可以释放技能的状态，通过检查，玩法能力将被应用
	virtual bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags) override;

	//激活能力
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	//结束能力
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	//用于检测
	//水平检测识别障碍物
	UPROPERTY(EditDefaultsOnly,Category = HorizontalTrace)
	float HorizontalTraceRadius = 30.f;
	
	UPROPERTY(EditDefaultsOnly,Category = HorizontalTrace)
	float HorizontalTraceLength = 500.f;

	//执行次数
	UPROPERTY(EditDefaultsOnly,Category = HorizontalTrace)
	float HorizontalTraceCount = 5.f;

	//水平射线之间的间隔
	UPROPERTY(EditDefaultsOnly,Category = HorizontalTrace)
	float HorizontalTraceStep = 30.f;

	//垂直识别
	UPROPERTY(EditDefaultsOnly,Category = HorizontalTrace)
	float VerticalTraceStep = 30.f;

	//垂直识别
	UPROPERTY(EditDefaultsOnly,Category = HorizontalTrace)
	float VerticalTraceRadius = 30.f;

	//指定碰撞通道
	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	//指定蒙太奇
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* VaultMontage = nullptr;

	//缓存目标点
	FVector JumpToLocation;
	FVector JumpOverLocation;

	//忽略一些碰撞，防止跳跃过程中，产生的不协调动作问题，比如卡进墙
	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannelsToIgnore;

	//创建蒙太奇动画，引擎提供好的
	UAbilityTask_PlayMontageAndWait* MontageTask = nullptr;
};
