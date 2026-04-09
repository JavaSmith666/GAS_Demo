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
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

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
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
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

		// Fire (press left mouse to activate GrantedAbilities[0])
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AGAS0Character::OnFireActionStarted);
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

void AGAS0Character::OnFireActionStarted(const FInputActionValue& Value)
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (GrantedAbilities.Num() <= 0)
	{
		return;
	}

	UClass* FireAbilityClass = GrantedAbilities[0].Get();
	if (!FireAbilityClass)
	{
		UE_LOG(LogGAS0, Warning, TEXT("Fire ability not loaded yet. Ensure GrantedAbilities[0] is valid and loaded."));
		return;
	}

	AbilitySystemComponent->TryActivateAbilityByClass(FireAbilityClass);
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

void AGAS0Character::BeginPlay()
{
	Super::BeginPlay();

	// Initialize Ability System Component and grant default abilities
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		// Collect soft paths to request async load
		TArray<FSoftObjectPath> PathsToLoad;
		PathsToLoad.Reserve(GrantedAbilities.Num());
		for (const TSoftClassPtr<UGameplayAbility>& AbilityClassSoft : GrantedAbilities)
		{
			FSoftObjectPath Path = AbilityClassSoft.ToSoftObjectPath();
			if (!Path.IsValid())
			{
				continue;
			}
			PathsToLoad.Add(Path);
		}

		if (PathsToLoad.Num() > 0)
		{
			// Request async load; OnGrantedAbilitiesLoaded will be called when ready
			UAssetManager::GetStreamableManager().RequestAsyncLoad(PathsToLoad, FStreamableDelegate::CreateUObject(this, &AGAS0Character::OnGrantedAbilitiesLoaded));
		}
	}
}

void AGAS0Character::OnGrantedAbilitiesLoaded()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	for (const TSoftClassPtr<UGameplayAbility>& AbilityClassSoft : GrantedAbilities)
	{
		UClass* AbilityClass = AbilityClassSoft.Get();
		if (AbilityClass)
		{
			FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
			AbilitySystemComponent->GiveAbility(Spec);
		}
	}
}

