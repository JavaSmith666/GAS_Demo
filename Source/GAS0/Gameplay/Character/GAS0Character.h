// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GameplayEffectTypes.h"
#include "Gameplay/Abilities/DataTables/CharacterSkillSlotsRow.h"
#include "Gameplay/Core/GAS0PlayerController.h"
#include "GAS0Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
class UGAS0AbilitySystemComponent;
class UGameplayAbility;
class UArrowComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChangeEvent, float, NewHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMPChangeEvent, float, NewMP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStrengthChangeEvent, float, NewStrength);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainUICreated);

USTRUCT(BlueprintType)
struct FPendingAbilityBinding
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY()
	UInputAction* ActivateAction;
};

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AGAS0Character : public ACharacter
{
	GENERATED_BODY()
	
#pragma region Base
public:
	/** Constructor */
	AGAS0Character();

	/** Actor lifecycle */
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CharacterID = -1;
#pragma endregion Base
	
#pragma region State
public:
	bool IsDead() const { return bIsDead; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDead = false;
#pragma endregion State
	
#pragma region PlayerController
public:
	APlayerController* GetPlayerController() const;
	
#pragma endregion PlayerController
	
#pragma region Input
public:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	
protected:
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;
#pragma endregion Input
	
#pragma region UI
public:
	UFUNCTION(BlueprintCallable, Category="UI")
	void OnMainUICreatedEvent();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void InitSkillIcon(FGameplayAbilityInfo AbilityInfo);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool bHasMainUICreated = false;
#pragma endregion UI

#pragma region Abilities
public:	
	UGAS0AbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	UArrowComponent* GetLaserPoint() const { return LaserPoint; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
    UArrowComponent* LaserPoint = nullptr;
	
protected:
	/** Callback when async loading of DT entries completes */
	void OnSkillConfigsLoaded();

	/** Ability System Component - used to grant and manage gameplay abilities */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	UGAS0AbilitySystemComponent* AbilitySystemComponent;
	
private:
	/** List of abilities and actions waiting for a valid InputComponent to be bound */
	UPROPERTY()
	TArray<FPendingAbilityBinding> PendingBindings;
	
	TArray<FSkillSlotEntry> PendingSkillEntries;
	FSkillSlotEntry PendingDefaultSkill;
	
	void InitializeSkillDataFromDataTable();
	void BindCancelAction();
	void OnCancelActionBound();
	
	/** Generic handler for skill input actions */
	void OnSkillActionStarted(TSubclassOf<UGameplayAbility> AbilityClass);

	/** Attempts to bind any skills that were loaded but didn't have an InputComponent yet */
	void TryBindPendingSkills();

#pragma endregion Abilities
	
#pragma region Camera
public:
	void UpdateCameraLockState(bool bLock);
	
protected:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	/** Minimum allowed camera pitch (degrees). Use to clamp camera looking down. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MinCameraPitch = -40.f;

	/** Maximum allowed camera pitch (degrees). Use to clamp camera looking up. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MaxCameraPitch = 40.f;
	
private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
#pragma endregion Camera
	
#pragma region Attributes
public:
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnHPChangeEvent OnHPChange;
	
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnMPChangeEvent OnMPChange;
	
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnStrengthChangeEvent OnStrengthChange;
	
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnMainUICreated OnMainUICreated;
	
private:
	void OnHPAttributeChanged(const FOnAttributeChangeData& Data);
	void OnMPAttributeChanged(const FOnAttributeChangeData& Data);
	void OnStrengthAttributeChanged(const FOnAttributeChangeData& Data);
#pragma endregion Attributes
	
#pragma region Movement
public:
	UFUNCTION(BlueprintCallable, Category="Movement")
	void SetFrictionZero();
	
	UFUNCTION(BlueprintCallable, Category="Movement")
	void ResetFriction();
	
	UFUNCTION(BlueprintCallable, Category="Movement")
	void PushAway(AGAS0Character* InInstigator, float Strength, float DelayTime = 0.f);
	
	void OnPushAwayDelayTimeReached();
	
	FTimerHandle PushAwayTimerHandle;
	
protected:
	float LastFriction;
#pragma endregion Movement
	
#pragma region Effects
public:
	UFUNCTION(BlueprintCallable, Category="Effects")
	void Stun(float Duration);
	
	void OnStunDurationReached();
	
	FTimerHandle StunTimerHandle;
	
#pragma endregion Effects
};
