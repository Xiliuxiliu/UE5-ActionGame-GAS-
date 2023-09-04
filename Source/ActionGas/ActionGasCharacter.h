// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ActionGameTypes.h"
#include "InputAction.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Ability/Components/AGAbilitySystemComponentBase.h"
#include "AbilitySystem/AttributeSets/AG_AttributeSetBase.h"
#include "ActionGasCharacter.generated.h"

class UCharacterDataAsset;
class UFootStepsComponent;
class UAG_MotionWarpingComponent;
class UAG_CharacterMovementComponent;
class UInventoryComponent;

UCLASS(config=Game)
class AActionGasCharacter : public ACharacter,public IAbilitySystemInterface
{
	GENERATED_BODY()

	//背包组件
	UPROPERTY(EditAnywhere,Replicated)
	UInventoryComponent* InventoryComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Anim, meta = (AllowPrivateAccess = "true"))
	UAG_MotionWarpingComponent* AGMotionWarpingComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UAG_CharacterMovementComponent* AGCharacterMovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	UFootStepsComponent* FootStepsComponent;
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// /** Jump Input Action */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	// class UInputAction* JumpAction;
	//
	// /** Move Input Action */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	// class UInputAction* MoveAction;
	//
	// /** Look Input Action */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	// class UInputAction* LookAction;

public:
	// AActionGasCharacter();
	AActionGasCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void PawnClientRestart() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UAG_AttributeSetBase* GetAttributeSetBase() const;

	virtual void PostInitializeComponents() override;
	
// protected:
//
// 	/** Called for movement input */
// 	void Move(const FInputActionValue& Value);
//
// 	/** Called for looking input */
// 	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	UPROPERTY(EditDefaultsOnly)
	UAGAbilitySystemComponentBase* AbilitySystemComponent;

	UPROPERTY(Transient)
	UAG_AttributeSetBase* AttributeSet;

	// // 初始化角色默认属性
	// UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="GAS")
	// TSubclassOf<UGameplayEffect> DefaultAttributeSet;
	//
	// // 能力
	// UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="GAS")
	// TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
	//
	// // 效果
	// UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="GAS")
	// TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

	UPROPERTY(EditDefaultsOnly)
	UCharacterDataAsset* CharacterDataAsset;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterData)
	FCharacterData CharacterData;

	FDelegateHandle MaxMovementSpeedChangeDelegateHandle;

	UFUNCTION()
	void OnRep_CharacterData();
	
	virtual void StaminaChanged(const FOnAttributeChangeData& Data);

	UPROPERTY()
	FTimerHandle StaminaTimerHandle;

	UFUNCTION()
	void RecoverStamina() const;

protected:
	
	// void InitializeAttributes();
	void GiveAbility();
	void ApplyStartupEffects();

	// 角色切换，只发生在服务器上
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnRep_PlayerState() override;

public:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* MoveForwardInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	UInputAction* MoveRightInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* TurnInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* LookUpInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* JumpInputAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* CrouchInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	UInputAction* SprintInputAction;

	//用于触发gameplay event
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag JumpEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer CrouchTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer SprintTags;

	UPROPERTY(EditDefaultsOnly,Replicated)
	FGameplayTagContainer StaminaNotFullTags;

	//用于触发后清理Gameplay tags的容器
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer InAirTags;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CrouchStateEffect;
public:
	
	UFootStepsComponent* GetFootStepsComponent();

	UAG_MotionWarpingComponent* GetAGMotionWarpingComponent()const;
	
	UFUNCTION(BlueprintCallable)
	const FCharacterData& GetCharacterData()const;

	UFUNCTION(BlueprintCallable)
	void SetCharacterData(const FCharacterData& InCharacterData);
	
	bool ApplyGameplayEffectToSelf(const TSubclassOf<UGameplayEffect> Effect,FGameplayEffectContextHandle& InEffectContext);

	virtual void InitFromCharacterData(const FCharacterData& InCharacterData,bool bFromReplication = false);

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnMoveForwardAction(const FInputActionValue& Value);
	void OnMoveRightAction(const FInputActionValue& Value);
	void OnTurnAction(const FInputActionValue& Value);
	void OnLookUpAction(const FInputActionValue& Value);
	void OnJumpActionStart(const FInputActionValue& Value);
	void OnJumpActionEnded(const FInputActionValue& Value);

	void OnCrouchActionStart(const FInputActionValue& Value);
	void OnCrouchActionEnded(const FInputActionValue& Value);

	void OnSprintActionStart(const FInputActionValue& Value);
	void OnSprintActionEnded(const FInputActionValue& Value);

	// 绑定属性变化
	void OnMaxMovementSpeedChange(const FOnAttributeChangeData& Data);
	
};

