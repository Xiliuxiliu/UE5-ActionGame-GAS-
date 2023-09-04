// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionGasCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemLog.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ActorComponent/AG_CharacterMovementComponent.h"
#include "ActorComponent/AG_MotionWarpingComponent.h"
#include "ActorComponent/FootStepsComponent.h"
#include "ActorComponent/InventoryComponent.h"
#include "DataAssets/CharacterDataAsset.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


//////////////////////////////////////////////////////////////////////////
// AActionGasCharacter

AActionGasCharacter::AActionGasCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UAG_CharacterMovementComponent>(CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bReplicates=true;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	AGCharacterMovementComponent = Cast<UAG_CharacterMovementComponent>(GetCharacterMovement());
	AGMotionWarpingComponent = CreateDefaultSubobject<UAG_MotionWarpingComponent>("MotionWarpingComponent");

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	AbilitySystemComponent = CreateDefaultSubobject<UAGAbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAG_AttributeSetBase>(TEXT("AttributeSet"));

	// 绑定客户端监听属性变化，GetGameplayAttributeValueChangeDelegate是用于注册属性值发生变化的回调函数，属性值变化时候会触发本函数
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxMovementSpeedAttribute()).AddUObject(this,&AActionGasCharacter::OnMaxMovementSpeedChange);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetStaminaAttribute()).AddUObject(this,&AActionGasCharacter::StaminaChanged);
	FootStepsComponent = CreateDefaultSubobject<UFootStepsComponent>(TEXT("FootStepsComponent"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);
	
}

void AActionGasCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	if (auto PlayerController = Cast<APlayerController>(GetController()))
	{
		if (auto SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->ClearAllMappings();
			SubSystem->AddMappingContext(DefaultMappingContext,0);
			UE_LOG(LogTemp,Warning,TEXT("use new mapping"));
		}
	}
}

UAbilitySystemComponent* AActionGasCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent?AbilitySystemComponent:nullptr;
}

UAG_AttributeSetBase* AActionGasCharacter::GetAttributeSetBase() const
{
	return AbilitySystemComponent?(AttributeSet?AttributeSet:nullptr):nullptr;
}

//在组件完成初始化后立刻调用
void AActionGasCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (IsValid(CharacterDataAsset))
	{
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
	
}

void AActionGasCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

UFootStepsComponent* AActionGasCharacter::GetFootStepsComponent()
{
	return FootStepsComponent;
}

UAG_MotionWarpingComponent* AActionGasCharacter::GetAGMotionWarpingComponent() const
{
	if (AGMotionWarpingComponent)
	{
		return AGMotionWarpingComponent;
	}
	return nullptr;
}

void AActionGasCharacter::OnRep_CharacterData()
{
	InitFromCharacterData(CharacterData,true);
}

void AActionGasCharacter::StaminaChanged(const FOnAttributeChangeData& Data)
{
	
	if (Data.NewValue < AttributeSet->GetMaxStamina())
	{
		if (!AbilitySystemComponent->HasAnyMatchingGameplayTags(StaminaNotFullTags))
		{
			AbilitySystemComponent->AddLooseGameplayTags(StaminaNotFullTags);

			if (HasAuthority()&&!StaminaTimerHandle.IsValid())
			{
				GetWorldTimerManager().SetTimer(StaminaTimerHandle, this, &AActionGasCharacter::RecoverStamina, 1.f,
				                                true);
			}
			
		}
	}
	else if (Data.NewValue == AttributeSet->GetMaxStamina())
	{
		if (AbilitySystemComponent->HasAnyMatchingGameplayTags(StaminaNotFullTags))
		{
			AbilitySystemComponent->RemoveLooseGameplayTags(StaminaNotFullTags);
			if (HasAuthority()&&StaminaTimerHandle.IsValid())
			{
				GetWorldTimerManager().ClearTimer(StaminaTimerHandle);
			}
		
		}
	}
}

void AActionGasCharacter::RecoverStamina() const 
{
	AttributeSet->SetStamina(AttributeSet->GetStamina()+1.0f);
}

const FCharacterData& AActionGasCharacter::GetCharacterData() const
{
	return CharacterData;
}

void AActionGasCharacter::SetCharacterData(const FCharacterData& InCharacterData)
{
	CharacterData = InCharacterData;
	InitFromCharacterData(CharacterData,true);
}

void AActionGasCharacter::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{
	
}


// void AActionGasCharacter::InitializeAttributes()
// {
// 	UE_LOG(LogTemp,Error,TEXT("Ready to InitializeAttributes."));
// 	if (GetLocalRole()==ROLE_Authority)
// 	{
// 		// 创建GE
// 		FGameplayEffectContextHandle EffectContext= AbilitySystemComponent->MakeEffectContext();
// 		// 添加源目标
// 		EffectContext.AddSourceObject(this);
// 		ApplyGameplayEffectToSelf(DefaultAttributeSet.Get(),EffectContext);
// 		UE_LOG(LogTemp,Error,TEXT("InitializeAttributes has finished."));
// 	}
// }

void AActionGasCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	GiveAbility();
	// InitializeAttributes();
	ApplyStartupEffects();
}



// 授予能力
void AActionGasCharacter::GiveAbility()
{
	if (HasAuthority()&&AbilitySystemComponent)
	{
		for (auto DefaultAbility:CharacterData.Abilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
		FHitResult HitResult;
		if (HitResult.bBlockingHit)
		{
			//never run
		}
		
	}
}

void AActionGasCharacter::ApplyStartupEffects()
{
	if(GetLocalRole()==ROLE_Authority)
	{
		// 创建ge
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		for (const TSubclassOf<UGameplayEffect> CharacterEffect:CharacterData.Effects)
		{
			// 应用给自身
			ApplyGameplayEffectToSelf(CharacterEffect,EffectContext);
		}
	}
}

bool AActionGasCharacter::ApplyGameplayEffectToSelf(const TSubclassOf<UGameplayEffect> Effect,
	FGameplayEffectContextHandle& InEffectContext)
{
	if (!Effect.Get())
	{
		return false;
	}

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect,1,InEffectContext);

	if (SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		return  ActiveGEHandle.WasSuccessfullyApplied();
	}
	return false;
}

void AActionGasCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AActionGasCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// // Set up action bindings
	// if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
	// 	
	// 	//Jumping
	// 	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	// 	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	//
	// 	//Moving
	// 	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AActionGasCharacter::Move);
	//
	// 	//Looking
	// 	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AActionGasCharacter::Look);
	//
	// }

	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		if (MoveForwardInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(MoveForwardInputAction, ETriggerEvent::Triggered, this, &AActionGasCharacter::OnMoveForwardAction);
		}
		if (MoveRightInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(MoveRightInputAction, ETriggerEvent::Triggered, this, &AActionGasCharacter::OnMoveRightAction);
		}
		if (TurnInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(TurnInputAction, ETriggerEvent::Triggered, this, &AActionGasCharacter::OnTurnAction);
		}
		if (LookUpInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(LookUpInputAction, ETriggerEvent::Triggered, this, &AActionGasCharacter::OnLookUpAction);
		}
		if (JumpInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Started, this, &AActionGasCharacter::OnJumpActionStart);
			PlayerEnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Completed, this, &AActionGasCharacter::OnJumpActionEnded);
		}
		if (CrouchInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(CrouchInputAction, ETriggerEvent::Started, this, &AActionGasCharacter::OnCrouchActionStart);
			PlayerEnhancedInputComponent->BindAction(CrouchInputAction, ETriggerEvent::Completed, this, &AActionGasCharacter::OnCrouchActionEnded);
		}
		if (SprintInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Started, this, &AActionGasCharacter::OnSprintActionStart);
			PlayerEnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Completed, this, &AActionGasCharacter::OnSprintActionEnded);
		}
	}

	

}

// void AActionGasCharacter::Move(const FInputActionValue& Value)
// {
// 	// input is a Vector2D
// 	FVector2D MovementVector = Value.Get<FVector2D>();
//
// 	if (Controller != nullptr)
// 	{
// 		// find out which way is forward
// 		const FRotator Rotation = Controller->GetControlRotation();
// 		const FRotator YawRotation(0, Rotation.Yaw, 0);
//
// 		// get forward vector
// 		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
// 	
// 		// get right vector 
// 		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
//
// 		// add movement 
// 		AddMovementInput(ForwardDirection, MovementVector.Y);
// 		AddMovementInput(RightDirection, MovementVector.X);
// 	}
// }
//
// void AActionGasCharacter::Look(const FInputActionValue& Value)
// {
// 	// input is a Vector2D
// 	FVector2D LookAxisVector = Value.Get<FVector2D>();
//
// 	if (Controller != nullptr)
// 	{
// 		// add yaw and pitch input to controller
// 		AddControllerYawInput(LookAxisVector.X);
// 		AddControllerPitchInput(LookAxisVector.Y);
// 	}
// }

void AActionGasCharacter::OnMaxMovementSpeedChange(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void AActionGasCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AActionGasCharacter,CharacterData);
	DOREPLIFETIME(AActionGasCharacter,InventoryComponent);
}

void AActionGasCharacter::OnMoveForwardAction(const FInputActionValue& Value)
{
	const float Magnitude = Value.GetMagnitude();
	if ((Controller!=nullptr)&&(Magnitude!=0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0,Rotation.Yaw,0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction,Magnitude);
	}
}

void AActionGasCharacter::OnMoveRightAction(const FInputActionValue& Value)
{
	const float Magnitude = Value.GetMagnitude();
	if ((Controller!=nullptr)&&(Magnitude!=0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0,Rotation.Yaw,0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction,Magnitude);
	}
}

void AActionGasCharacter::OnTurnAction(const FInputActionValue& Value)
{
	AddControllerYawInput(Value.GetMagnitude()*50.f*GetWorld()->GetDeltaSeconds());
}

void AActionGasCharacter::OnLookUpAction(const FInputActionValue& Value)
{
	AddControllerPitchInput(Value.GetMagnitude()*50.f*GetWorld()->GetDeltaSeconds());
}


void AActionGasCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveActiveEffectsWithTags(InAirTags);
	}
	
}

void AActionGasCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	if (!CrouchStateEffect.Get())
	{
		return;
	}
	if (AbilitySystemComponent)
	{
		FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CrouchStateEffect,1,EffectContextHandle);

		if (EffectSpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

			if (!ActiveHandle.WasSuccessfullyApplied())
			{
				ABILITY_LOG(Log,TEXT("Ability %s failed to aplly crouch effect %s"),*GetName(),*GetNameSafe(CrouchStateEffect));
			}
		}
		
	}
}

void AActionGasCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (AbilitySystemComponent&&CrouchStateEffect.Get())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(CrouchStateEffect,AbilitySystemComponent);
	}
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AActionGasCharacter::OnCrouchActionStart(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(CrouchTags,true);
	}
}

void AActionGasCharacter::OnCrouchActionEnded(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities(&CrouchTags);
	}
}

void AActionGasCharacter::OnSprintActionStart(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(SprintTags,true);
	}
}

void AActionGasCharacter::OnSprintActionEnded(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities(&SprintTags);
	}
}


void AActionGasCharacter::OnJumpActionStart(const FInputActionValue& Value)
{
	// //Jump();
	//
	// //通过事件触发jump
	// FGameplayEventData Payload;
	// Payload.Instigator = this;
	// Payload.EventTag = JumpEventTag;
	//
	// UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,JumpEventTag,Payload);
	// UE_LOG(LogTemp,Warning,TEXT("Has Send Event to actor"));

	AGCharacterMovementComponent->TryTravelsal(AbilitySystemComponent);
	
}

void AActionGasCharacter::OnJumpActionEnded(const FInputActionValue& Value)
{
	StopJumping();
}







