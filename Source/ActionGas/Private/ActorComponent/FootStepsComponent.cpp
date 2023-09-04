// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/FootStepsComponent.h"

#include "ActionGas/ActionGasCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/AG_PhysicalMaterial.h"

static TAutoConsoleVariable<int32> CVarShowFootsteps(TEXT("ShowFootsteps"),
                                                     0,
                                                     TEXT("Draw debug info about footstep/n   0: off/n  1:on/n"),
                                                     ECVF_Cheat);

// Sets default values for this component's properties
UFootStepsComponent::UFootStepsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFootStepsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFootStepsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFootStepsComponent::HandleFootstep(EFoot Foot)
{
	if (auto Character = Cast<AActionGasCharacter>(GetOwner()))
	{
		int32 DebugFootsteps = CVarShowFootsteps.GetValueOnAnyThread();
		if (auto Mesh = Character->GetMesh())
		{
			const auto SocketLocation = Mesh->GetSocketLocation(Foot == EFoot::LEFT? LeftFootSocketName:RightFootSocketName);

			FHitResult HitResult;
			const FVector Location = SocketLocation+FVector::UpVector*20.0f;
			
			FCollisionQueryParams QueryParams;
			QueryParams.bReturnPhysicalMaterial = true;
			QueryParams.AddIgnoredActor(Character);

			if (GetWorld()->LineTraceSingleByChannel(HitResult,Location,Location+::FVector::UpVector*-50.0f,ECC_WorldStatic,QueryParams))
			{
				if(HitResult.bBlockingHit)
				{
					if (HitResult.PhysMaterial.Get())
					{
						// 获取物理资产
						UAG_PhysicalMaterial* PhysicalMaterial = Cast<UAG_PhysicalMaterial>(HitResult.PhysMaterial.Get());
						if (PhysicalMaterial)
						{
							//播放声音
							UGameplayStatics::PlaySoundAtLocation(this,PhysicalMaterial->FootstepSound,Location,0.1f,0.1f);

							//打印命中名称
							if (DebugFootsteps>0)
							{
								DrawDebugString(GetWorld(),Location,PhysicalMaterial->GetName(),nullptr,FColor::White,4.f);
							}
						}
						if (DebugFootsteps>0)
						{
							DrawDebugSphere(GetWorld(),Location,16,16,FColor::Red,false,4.f);
						}
					}
					else
					{
						if (DebugFootsteps>0)
						{
							DrawDebugLine(GetWorld(),Location,Location+FVector::UpVector*-50.f,FColor::Red,false,4.f,0,1);
						}
					}
				}
				else
				{
					if (DebugFootsteps>0)
					{
						DrawDebugLine(GetWorld(),Location,Location+FVector::UpVector*-50.f,FColor::Red,false,4.f,0,1);
						DrawDebugSphere(GetWorld(),Location,16,16,FColor::Red,false,4.f);
					}
				}
					
			}
		}
	}	
}

