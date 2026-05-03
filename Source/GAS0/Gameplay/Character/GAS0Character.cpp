// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS0Character.h"

#include "GAS0CharacterGlobalConfig.h"
#include "GAS0CharacterSettings.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GAS0AICharacter.h"
#include "Gameplay/Abilities/GAS0AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "Gameplay/Abilities/GAS0CharacterGameplayAbility.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Gameplay/Abilities/DataAssets/GAS0AbilitySettings.h"
#include "Gameplay/Abilities/DataTables/CharacterSkillSlotsRow.h"
#include "Gameplay/Abilities/DataAssets/SkillConfig.h"
#include "Gameplay/AttributeSet/BaseAttributeSet.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"


AGAS0Character::AGAS0Character()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	
	if (!IsA<AGAS0AICharacter>())
	{
		LaserPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("LaserPoint"));
		LaserPoint->SetupAttachment(RootComponent);
		
		DashDamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DashDamageSphere"));
		DashDamageSphere->SetupAttachment(RootComponent);
	}

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	AbilitySystemComponent = CreateDefaultSubobject<UGAS0AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void AGAS0Character::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeCharacterGlobalConfig();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeSkillDataFromDataTable();
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBaseAttributeSet::GetHPAttribute()).AddUObject(this, &AGAS0Character::OnHPAttributeChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBaseAttributeSet::GetMPAttribute()).AddUObject(this, &AGAS0Character::OnMPAttributeChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBaseAttributeSet::GetStrengthAttribute()).AddUObject(this, &AGAS0Character::OnStrengthAttributeChanged);
		
		if (GAS0CharacterGlobalConfig && GAS0CharacterGlobalConfig->HealthRegenInfiniteEffect && TeamID != ETeamID::Enemy)
		{
			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GAS0CharacterGlobalConfig->HealthRegenInfiniteEffect, 1.f, AbilitySystemComponent->MakeEffectContext());
			HealthRegenInfiniteEffectSpecHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
	if (DashDamageSphere)
	{
		DashDamageSphere->OnComponentBeginOverlap.AddDynamic(this, &AGAS0Character::OnDashDamageSphereOverlap);
	}
}

void AGAS0Character::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(HealthRegenInfiniteEffectSpecHandle);
	}
	
	if (DashDamageSphere)
	{
		DashDamageSphere->OnComponentBeginOverlap.RemoveDynamic(this, &AGAS0Character::OnDashDamageSphereOverlap);
	}
}

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

		InitializeCharacterGlobalConfig();
		TryBindPendingSkills();
		BindOtherActions();
	}
}

void AGAS0Character::InitializeCharacterGlobalConfig()
{
	const UGAS0CharacterSettings* Settings = GetDefault<UGAS0CharacterSettings>();
	if (!GAS0CharacterGlobalConfig && Settings && !Settings->CharacterGlobalConfig.IsNull())
	{
		UGAS0CharacterGlobalConfig* CharacterGlobalConfig = Settings->CharacterGlobalConfig.LoadSynchronous();
		GAS0CharacterGlobalConfig = CharacterGlobalConfig;
	}
}

void AGAS0Character::OnDashDamageSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this || DashOverlapActors.Contains(OtherActor) || !GAS0CharacterGlobalConfig)
	{
		return;
	}
	
	DashOverlapActors.Add(OtherActor);
	if (AGAS0Character* OtherCharacter = Cast<AGAS0Character>(OtherActor))
	{
		if (TeamID == OtherCharacter->GetTeamID())
		{
			return;
		}
		
		if (GetNetMode() < NM_Client)
		{
			OtherCharacter->MultiPlayMontage(GAS0CharacterGlobalConfig->StunMontage);
			if (UGAS0AbilitySystemComponent* OtherASC = OtherCharacter->GetAbilitySystemComponent())
			{
				FGameplayEffectSpecHandle SpecHandle = OtherASC->MakeOutgoingSpec(GAS0CharacterGlobalConfig->DashDamageEffect, 1.f, OtherASC->MakeEffectContext());
				OtherASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				
				FVector Direction = (OtherCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				OtherCharacter->PushAway(Direction, GAS0CharacterGlobalConfig->DashImpulse, 1.f);
			}
		}
		
		if (OtherCharacter->IsLocallyControlled() && OtherCharacter->GetTeamID() != ETeamID::Enemy)
		{
			OtherCharacter->Stun(1.f);
		}
	}
}

APlayerController* AGAS0Character::GetPlayerController() const
{
	return Cast<APlayerController>(GetController());
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

void AGAS0Character::MultiPlayMontage_Implementation(UAnimMontage* MontageToPlay)
{
	if (!MontageToPlay)
	{
		return;
	}
	
	if (IsLocallyControlled() && TeamID != ETeamID::Enemy)
	{
		return;
	}
	
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(MontageToPlay);
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
					Binding.AbilityTags = Entry.AbilityTags;
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

void AGAS0Character::BindOtherActions()
{
	if (InputComponent && IsLocallyControlled() && GAS0CharacterGlobalConfig)
	{
		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
		{
			if (GAS0CharacterGlobalConfig->CancelAbilityAction)
			{
				EIC->BindAction(GAS0CharacterGlobalConfig->CancelAbilityAction, ETriggerEvent::Started, this, &AGAS0Character::OnCancelActionBound);	
			}
					
			if (GAS0CharacterGlobalConfig->ConfirmSkillAbilityAction)
			{
				EIC->BindAction(GAS0CharacterGlobalConfig->ConfirmSkillAbilityAction, ETriggerEvent::Started, this, &AGAS0Character::OnConfirmSkillActionBound);	
			}
		}
	}
}

void AGAS0Character::OnCancelActionBound()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
	}
}

void AGAS0Character::OnConfirmSkillActionBound()
{
	OnSkillConfirmed.ExecuteIfBound();
}

void AGAS0Character::CheckDeath(float InCurrentHP)
{
	if (!bIsDead && InCurrentHP <= 0.f && GAS0CharacterGlobalConfig)
	{
		bIsDead = true;
		
		if (GetNetMode() != NM_DedicatedServer)
		{
			// 本地预测播放蒙太奇
			if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
			{
				AnimInst->Montage_Play(GAS0CharacterGlobalConfig->DeathMontage);
			}
		}
		else
		{
			// 多播RPC让模拟端播蒙太奇
			MultiPlayMontage(GAS0CharacterGlobalConfig->DeathMontage);	
		}
		
		if (IsLocallyControlled())
		{
			if (APlayerController* PC = GetPlayerController())
			{
				PC->DisableInput(PC);
			}
		}
	}
}

void AGAS0Character::OnHPAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnHPChange.Broadcast(Data.NewValue);
	CheckDeath(Data.NewValue);
	if (GetNetMode() != NM_DedicatedServer)
	{
		UpdateHPBar();
		UpdateHPAttributeBar();
	}
}

void AGAS0Character::OnMPAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnMPChange.Broadcast(Data.NewValue);
	if (GetNetMode() != NM_DedicatedServer)
	{
		UpdateMPAttributeBar();
	}
}

void AGAS0Character::OnStrengthAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnStrengthChange.Broadcast(Data.NewValue);
	if (GetNetMode() != NM_DedicatedServer)
	{
		UpdateStrengthAttributeBar();
	}
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

void AGAS0Character::PushAway(const FVector& Dir, float Strength, float DelayTime)
{
	SetFrictionZero();
	if (UCharacterMovementComponent* TempCharacterMovement = GetCharacterMovement())
	{
		TempCharacterMovement->StopMovementImmediately();
		TempCharacterMovement->AddImpulse(Dir.GetSafeNormal() * Strength, true);
		GetWorld()->GetTimerManager().SetTimer(PushAwayTimerHandle, FTimerDelegate::CreateUObject(this, &AGAS0Character::OnPushAwayDelayTimeReached), DelayTime, false);
	}
}

void AGAS0Character::OnPushAwayDelayTimeReached()
{
	ResetFriction();
}

void AGAS0Character::Stun(float Duration)
{
	if (GetNetMode() != NM_DedicatedServer)
	{
		if (APlayerController* PC = GetPlayerController())
		{
			DisableInput(PC);
			GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, FTimerDelegate::CreateUObject(this, &AGAS0Character::OnStunDurationReached), Duration, false);
		}
	}
}

void AGAS0Character::OnStunDurationReached()
{
	if (GetNetMode() != NM_DedicatedServer)
	{
		if (APlayerController* PC = GetPlayerController())
		{
			EnableInput(PC);
		}
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
					EIC->BindAction(Binding.ActivateAction, ETriggerEvent::Started, this, &AGAS0Character::OnSkillActionStarted, Binding.AbilityTags);
				}
			}
			// Clear pending bindings after they are successfully bound to avoid duplicate bindings
			PendingBindings.Empty();
		}
	}
}

void AGAS0Character::OnSkillActionStarted(FGameplayTagContainer AbilityTags)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags);
	}
}

void AGAS0Character::UpdateCameraLockState(bool bLock)
{
	bUseControllerRotationYaw = bLock;
	if (CameraBoom && GAS0CharacterGlobalConfig)
	{
		CameraBoom->bUsePawnControlRotation	= !bLock;
		CameraBoom->SetRelativeLocation(bLock ? GAS0CharacterGlobalConfig->LockCameraRelativeLocation : FVector::ZeroVector);
		CameraBoom->SetRelativeRotation(bLock ? GAS0CharacterGlobalConfig->LockCameraRelativeRotation : FRotator::ZeroRotator);
	}
	
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
	{
		CharacterMovementComponent->bOrientRotationToMovement = !bLock;
	}
}
