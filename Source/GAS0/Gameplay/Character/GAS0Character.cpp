// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS0Character.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GAS0.h"
#include "Gameplay/Abilities/GAS0AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "Gameplay/Abilities/GAS0CharacterGameplayAbility.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Gameplay/Abilities/DataAssets/GAS0AbilitySettings.h"
#include "Gameplay/Abilities/DataTables/CharacterSkillSlotsRow.h"
#include "Gameplay/Abilities/DataAssets/SkillConfig.h"
#include "Gameplay/AttributeSet/BaseAttributeSet.h"

class UGridGameplayMessageSubsystem;
class UGAS0CharacterGameplayAbility;

AGAS0Character::AGAS0Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Ability System Component
	AbilitySystemComponent = CreateDefaultSubobject<UGAS0AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

// (constructor defaults set above)

void AGAS0Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGAS0Character::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AGAS0Character::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGAS0Character::Look);

		// Try to bind any skills that might have loaded before this was called
		TryBindPendingSkills();
	}
	else
	{
		UE_LOG(LogGAS0, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AGAS0Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AGAS0Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AGAS0Character::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AGAS0Character::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		const FRotator ControlRot = GetController()->GetControlRotation();
		const float CurrentPitch = FRotator::NormalizeAxis(ControlRot.Pitch);
		float DesiredPitch = CurrentPitch - Pitch;
		if (DesiredPitch >= MinCameraPitch && DesiredPitch <= MaxCameraPitch)
		{
			FRotator NewRot = GetController()->GetControlRotation();
			NewRot.Pitch = ControlRot.Pitch - Pitch;
			GetController()->SetControlRotation(NewRot);
		}
	}
}

void AGAS0Character::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AGAS0Character::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AGAS0Character::OnMainUICreatedEvent()
{
	OnMainUICreated.Broadcast();
}

void AGAS0Character::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeSkillDataFromDataTable();
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBaseAttributeSet::GetHPAttribute()).AddUObject(this, &AGAS0Character::OnHPAttributeChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBaseAttributeSet::GetMPAttribute()).AddUObject(this, &AGAS0Character::OnMPAttributeChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBaseAttributeSet::GetStrengthAttribute()).AddUObject(this, &AGAS0Character::OnStrengthAttributeChanged);
	}
	
	PlayerController = Cast<AGAS0PlayerController>(GetController());
}

void AGAS0Character::OnSkillActionStarted(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (AbilitySystemComponent && AbilityClass)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(AbilityClass);
	}
}

void AGAS0Character::OnSkillConfigsLoaded()
{
	auto GrantAndBind = [&](const FSkillSlotEntry& Entry, int32 AbilityIndex) -> void
	{
		UClass* AbilityClass = Entry.AbilityClass.Get();
		if (AbilityClass && AbilitySystemComponent)
		{
			if (GetNetMode() < NM_Client)
			{
				FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
				
				// Use SourceObject to carry the config to the ability instances
				if (USkillConfig* Config = Entry.SkillConfig.Get())
				{
					Config->AbilityIndex = AbilityIndex;
					Spec.SourceObject = Config;
				}
				
				AbilitySystemComponent->GiveAbility(Spec);
			}
			
			if (GetNetMode() != NM_DedicatedServer)
			{
				if (UInputAction* Action = Entry.ActivateAction.Get())
				{
					FPendingAbilityBinding Binding;
					Binding.AbilityClass = AbilityClass;
					Binding.ActivateAction = Action;
					PendingBindings.Add(Binding);
				}	
			}
		}
	};
	
	GrantAndBind(PendingDefaultSkill, -1);
	for (int32 i = 0; i < PendingSkillEntries.Num(); i++)
	{
		GrantAndBind(PendingSkillEntries[i], i);
	}
	
	// Attempt binding immediately if InputComponent is already ready
	TryBindPendingSkills();
}

void AGAS0Character::InitializeSkillDataFromDataTable()
{
	// Get DT from settings
	const UGAS0AbilitySettings* Settings = GetDefault<UGAS0AbilitySettings>();
	if (Settings && !Settings->CharacterSkillTable.IsNull())
	{
		UDataTable* SkillTable = Settings->CharacterSkillTable.LoadSynchronous();
		if (SkillTable)
		{
			// Find row by CharacterID
			FCharacterSkillSlotsRow* Row = SkillTable->FindRow<FCharacterSkillSlotsRow>(FName(*FString::FromInt(CharacterID)), TEXT("Skill Grant"));
			if (Row)
			{
				PendingSkillEntries = Row->SlotSkills;
				PendingDefaultSkill = Row->DefaultSkill;
				TArray<FSoftObjectPath> PathsToLoad;
				if (!Row->DefaultSkill.AbilityClass.IsNull())
				{
					PathsToLoad.Add(Row->DefaultSkill.AbilityClass.ToSoftObjectPath());
				}
				if (!Row->DefaultSkill.SkillConfig.IsNull())
				{
					PathsToLoad.Add(Row->DefaultSkill.SkillConfig.ToSoftObjectPath());
				}
				if (!Row->DefaultSkill.ActivateAction.IsNull())
				{
					PathsToLoad.Add(Row->DefaultSkill.ActivateAction.ToSoftObjectPath());
				}
				
				for (auto& Entry : Row->SlotSkills)
				{
					if (!Entry.AbilityClass.IsNull())
					{
						PathsToLoad.Add(Entry.AbilityClass.ToSoftObjectPath());
					}
					if (!Entry.SkillConfig.IsNull())
					{
						PathsToLoad.Add(Entry.SkillConfig.ToSoftObjectPath());
					}
					if (!Entry.ActivateAction.IsNull())
					{
						PathsToLoad.Add(Entry.ActivateAction.ToSoftObjectPath());
					}
				}

				if (PathsToLoad.Num() > 0)
				{
					UAssetManager::GetStreamableManager().RequestAsyncLoad(PathsToLoad, FStreamableDelegate::CreateUObject(this, &AGAS0Character::OnSkillConfigsLoaded));
				}
			}
		}
	}
}

void AGAS0Character::OnHPAttributeChanged(const FOnAttributeChangeData& Data)
{
	// Authority
	OnHPChange.Broadcast(Data.NewValue);
}

void AGAS0Character::OnMPAttributeChanged(const FOnAttributeChangeData& Data)
{
	// Authority
	OnMPChange.Broadcast(Data.NewValue);
}

void AGAS0Character::OnStrengthAttributeChanged(const FOnAttributeChangeData& Data)
{
	// Authority
	OnStrengthChange.Broadcast(Data.NewValue);
}

void AGAS0Character::SetFrictionZero()
{
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
	{
		LastFriction = CharacterMovementComponent->GroundFriction;
		CharacterMovementComponent->GroundFriction = 0.f;
	}
}

void AGAS0Character::ResetFriction()
{
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
	{
		CharacterMovementComponent->GroundFriction = LastFriction;
	}
}

void AGAS0Character::TryBindPendingSkills()
{
	if (InputComponent && IsLocallyControlled() && PendingBindings.Num() > 0)
	{
		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
		{
			for (const FPendingAbilityBinding& Binding : PendingBindings)
			{
				if (Binding.ActivateAction && Binding.AbilityClass)
				{
					EIC->BindAction(Binding.ActivateAction, ETriggerEvent::Started, this, &AGAS0Character::OnSkillActionStarted, Binding.AbilityClass);
				}
			}
			// Clear pending bindings after they are successfully bound to avoid duplicate bindings
			PendingBindings.Empty();
		}
	}
}
