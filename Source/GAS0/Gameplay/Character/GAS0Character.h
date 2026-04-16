// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GameplayEffectTypes.h"
#include "Gameplay/Abilities/DataTables/CharacterSkillSlotsRow.h"
#include "GAS0Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
class UGAS0AbilitySystemComponent;
class UGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChangeEvent, float, NewHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMPChangeEvent, float, NewMP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStrengthChangeEvent, float, NewStrength);

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

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
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

	/** Minimum allowed camera pitch (degrees). Use to clamp camera looking down. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MinCameraPitch = -40.f;

	/** Maximum allowed camera pitch (degrees). Use to clamp camera looking up. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MaxCameraPitch = 40.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CharacterID = -1;

public:

	/** Constructor */
	AGAS0Character();

	/** Actor lifecycle */
	virtual void BeginPlay() override;
	
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

	/** Generic handler for skill input actions */
	void OnSkillActionStarted(TSubclassOf<UGameplayAbility> AbilityClass);

	/** Attempts to bind any skills that were loaded but didn't have an InputComponent yet */
	void TryBindPendingSkills();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnHPChangeEvent OnHPChange;
	
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnMPChangeEvent OnMPChange;
	
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnStrengthChangeEvent OnStrengthChange;

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
	
	UFUNCTION()
	void InitializeSkillDataFromDataTable();
	
	void OnHPAttributeChanged(const FOnAttributeChangeData& Data);
	void OnMPAttributeChanged(const FOnAttributeChangeData& Data);
	void OnStrengthAttributeChanged(const FOnAttributeChangeData& Data);
};
