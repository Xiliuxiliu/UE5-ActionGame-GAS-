// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Vault.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "ActionGas/ActionGasCharacter.h"
#include "ActorComponent/AG_MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Serialization/ArchiveReplaceObjectRef.h"

UGA_Vault::UGA_Vault()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

bool UGA_Vault::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	if (!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
	{
		return false;
	}
	
	AActionGasCharacter* Character = GetActionGameCharacterFromActorInfo();
	if (!IsValid(Character))
	{
		return false;
	}
	
	const FVector StartLocation = Character->GetActorLocation();
	const FVector ForwardVector = Character->GetActorForwardVector();
	const FVector UpVector = Character->GetActorUpVector();

	TArray<AActor*> ActorToIgnore = {Character};

	static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTravelsal")) ;
	const bool bShowDebugTravelsal = CVar->GetInt()>0;

	EDrawDebugTrace::Type DebugDrawType = bShowDebugTravelsal? EDrawDebugTrace::ForDuration:EDrawDebugTrace::None;
	
	int32 JumpToLocationIdx = INDEX_NONE;

	//跟踪索引
	int i = 0;
	FHitResult TraceHitResult;
	//跳跃的最大水平距离
	float MaxJumpDistance = HorizontalTraceLength;

	//开始横向走线，从地面开始每层发射一条
	// TODO：单纯寻找水平方向上有没有碰撞并记录到碰撞物的直线距离
	for (; i < HorizontalTraceCount;++i)
	{
		const FVector TraceStart = StartLocation + i * UpVector * HorizontalTraceStep;
		const FVector TraceEnd = TraceStart + ForwardVector * HorizontalTraceLength;

		if (UKismetSystemLibrary::SphereTraceSingleForObjects(this,
			TraceStart,
			TraceEnd,
			HorizontalTraceRadius,
			TraceObjectTypes,
			true,
			ActorToIgnore,
			DebugDrawType,
			TraceHitResult,
			true))
		{
			//第一次循环，查看是否有障碍物，INDEX_NONE表示第一次检测
			if (JumpToLocationIdx == INDEX_NONE&&(i<HorizontalTraceCount-1))
			{
				JumpToLocationIdx = i;
				JumpToLocation = TraceHitResult.Location;
			}
			//第二次循环
			//为真则为第二次检测到有物体，确实有障碍物，
			else if(JumpToLocationIdx==(i-1))
			{
				//初始化最大跳跃距离
				MaxJumpDistance = FVector::Dist2D(TraceHitResult.Location,TraceStart);
				break;
			}
		}
		else
		{
			//没有检测到碰撞
			if (JumpToLocationIdx!=INDEX_NONE)
			{
				break;
			}
		}
	}

	if (JumpToLocationIdx == INDEX_NONE)
	{
		return false;
	}
	//计算人物起始点与碰撞点的距离
	const float DistanceToJumpTo = FVector::Dist2D(StartLocation,JumpToLocation);

	//Maxjumpdistance意味着第二次射线检测结果 起始点到终点的距离，比如圆锥类，正立圆锥可以跳，倒立不可以
	const float MaxVerticalTraceDistance = MaxJumpDistance - DistanceToJumpTo;
	if (MaxVerticalTraceDistance<0)
	{
		return false;
	}
	//计算垂直 1.确定碰撞点z轴高度 2、确定over后落点
	if (i == HorizontalTraceCount)
	{
		i = HorizontalTraceCount - 1 ;
	}

	const float VerticalTraceLength = FMath::Abs(JumpToLocation.Z - (StartLocation + i * UpVector * HorizontalTraceStep).Z);
	FVector VerticalStartLocation = JumpToLocation + UpVector * VerticalTraceLength;

	i = 0;
	const float VerticalTraceCount = MaxVerticalTraceDistance/VerticalTraceStep;
	bool bJumpOverLocationSet = false;

	for (;i<=VerticalTraceCount;i++)
	{
		const FVector TraceStart = VerticalStartLocation + (i * ForwardVector * VerticalTraceStep);
		const FVector TraceEnd = TraceStart - UpVector * VerticalTraceLength;

		if (UKismetSystemLibrary::SphereTraceSingleForObjects(this,
			TraceStart,
			TraceEnd,
			HorizontalTraceRadius,
			TraceObjectTypes,
			true,
			ActorToIgnore,
			DebugDrawType,
			TraceHitResult,
			true))
		{
			//最终跳跃位置，设置为撞击点
			JumpOverLocation = TraceHitResult.ImpactPoint;

			//i==0 第一次垂直碰撞
			if (i == 0)
			{
				JumpToLocation = JumpOverLocation;
			}
		}
		else if(i != 0)
		{
			bJumpOverLocationSet = true;
			break;
		}
	}
	if (!bJumpOverLocationSet)
	{
		return false;	
	}
	//TODO:跳跃点有了，需要计算往前距离
	//射线起始点，是障碍物第一个攀爬点+向前一个向量的垂直间距的距离，正好难呢过跳过障碍
	const FVector TraceStart = JumpOverLocation + ForwardVector * VerticalTraceStep;//const FVector TraceStart = JumpOverLocation + ForwardVector + VerticalTraceStep
	if (UKismetSystemLibrary::SphereTraceSingleForObjects(this,
		TraceStart,
		JumpOverLocation,
		HorizontalTraceRadius,
		TraceObjectTypes,
		true,
		ActorToIgnore,
		DebugDrawType,
		TraceHitResult,
		true))
	{
		JumpOverLocation = TraceHitResult.ImpactPoint;
	}
	if (bShowDebugTravelsal)
	{
		DrawDebugSphere(GetWorld(),JumpToLocation,15.f,16,FColor::White,false,7.f);
		DrawDebugSphere(GetWorld(),JumpOverLocation,15.f,16,FColor::White,false,7.f);
	}
	return true;
	
	
}

void UGA_Vault::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitCheck(Handle,ActorInfo,ActivationInfo,nullptr))
	{
		K2_EndAbility();
		return;
	}
	AActionGasCharacter* Character = GetActionGameCharacterFromActorInfo();
	UCharacterMovementComponent* CharacterMovementComponent = Character?Character->GetCharacterMovement():nullptr;
	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->SetMovementMode(MOVE_Flying);
		//UKismetSystemLibrary::PrintString(this,FString::Printf(TEXT("Move_Flying - - > %f ")),true,true,FLinearColor::Red,10.f);
	}
	UCapsuleComponent* CapsuleComponent = Character?Character->GetCapsuleComponent():nullptr;
	if (CapsuleComponent)
	{
		for (ECollisionChannel Channel:CollisionChannelsToIgnore)
		{
			CapsuleComponent->SetCollisionResponseToChannel(Channel,ECR_Ignore);
		}
		
	}
	UAG_MotionWarpingComponent* MotionWarpingComponent = Character?Character->GetAGMotionWarpingComponent():nullptr;
	if (MotionWarpingComponent)
	{
		//设置动作扭曲目标点
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("JumpToLocation"),JumpToLocation,Character->GetActorRotation());
		//MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("JumpOverLocation"),JumpOverLocation,Character->GetActorRotation());
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("JumpOverLocation"),JumpToLocation,Character->GetActorRotation());
		//MotionWarpingComponent->SendWarpPointsToClients();
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,VaultMontage);
	//打断时停止
	MontageTask->OnBlendOut.AddDynamic(this,&UGA_Vault::K2_EndAbility);
	//完成时GA停止
	MontageTask->OnCompleted.AddDynamic(this,&UGA_Vault::K2_EndAbility);
	//中断时 直接GA停止
	MontageTask->OnInterrupted.AddDynamic(this,&UGA_Vault::K2_EndAbility);
	//取消时 直接GA停止
	MontageTask->OnCancelled.AddDynamic(this,&UGA_Vault::K2_EndAbility);

	//激活任务
	MontageTask->ReadyForActivation();
}

void UGA_Vault::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	if (IsValid(MontageTask))
	{
		MontageTask->EndTask();
	}
	AActionGasCharacter* Character = GetActionGameCharacterFromActorInfo();
	UCharacterMovementComponent* CharacterMovementComponent = Character?Character->GetCharacterMovement():nullptr;
	
	// if (CharacterMovementComponent)
	// {
	// 	CharacterMovementComponent->SetMovementMode(MOVE_Walking);
	// 	
	// }
	
	if (CharacterMovementComponent&&CharacterMovementComponent->IsFlying())
	{
		CharacterMovementComponent->SetMovementMode(MOVE_Falling);
		
	}

	//恢复碰撞
	UCapsuleComponent* CapsuleComponent = Character?Character->GetCapsuleComponent():nullptr;
	if (CapsuleComponent)
	{
		for (ECollisionChannel Channel:CollisionChannelsToIgnore)
		{
			CapsuleComponent->SetCollisionResponseToChannel(Channel,ECR_Block);
		}
	}

	UAG_MotionWarpingComponent* MotionWarpingComponent = Character?Character->GetAGMotionWarpingComponent():nullptr;
	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveWarpTarget(TEXT("JumpToLocation"));
		MotionWarpingComponent->RemoveWarpTarget(TEXT("JumpOverLocation"));
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);	
	
}
