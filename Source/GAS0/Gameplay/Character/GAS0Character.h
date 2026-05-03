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
class UArrowComponent;
class UGAS0CharacterGlobalConfig;
class USphereComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChangeEvent, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMPChangeEvent, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStrengthChangeEvent, float);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainUICreated);
DECLARE_DELEGATE(FOnSkillConfirmed)

UENUM(BlueprintType)
enum class ETeamID : uint8
{
	Neutral,
	Player,
	Enemy
};

USTRUCT(BlueprintType)
struct FPendingAbilityBinding
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UGameplayAbility> AbilityClass;
	
	UPROPERTY()
	FGameplayTagContainer AbilityTags;

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
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UGAS0CharacterGlobalConfig* GetGAS0CharacterGlobalConfig() const { return GAS0CharacterGlobalConfig; }
	
	UFUNCTION(NetMulticast, UnReliable)
	void MultiPlayMontage(UAnimMontage* MontageToPlay);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CharacterID = -1;
	
	UPROPERTY(Transient)
	UGAS0CharacterGlobalConfig* GAS0CharacterGlobalConfig = nullptr;
	
private:
	void InitializeCharacterGlobalConfig();
#pragma endregion Base
	
#pragma region Dash
public:
	void ResetDashOverlapActorsArray() { DashOverlapActors.Empty(); }
	
	USphereComponent* GetDashDamageSphere() const { return DashDamageSphere; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Collision")
	USphereComponent* DashDamageSphere = nullptr;
	
	UPROPERTY(Transient)
	TArray<AActor*> DashOverlapActors;
	
	UFUNCTION()
	void OnDashDamageSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
#pragma endregion Dash
	
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
	
#pragma region Team
public:
	ETeamID GetTeamID() const { return TeamID; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Team")
	ETeamID TeamID = ETeamID::Neutral;
	
#pragma endregion Team
	
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
	void InitSkillIcon(int32 AbilityIndex, float CD, UMaterialInstance* MI);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool bHasMainUICreated = false;
#pragma endregion UI

#pragma region Abilities
public:	
	UGAS0AbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	UArrowComponent* GetLaserPoint() const { return LaserPoint; }
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartCD(int32 InAbilityIndex);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
    UArrowComponent* LaserPoint = nullptr;
	
	FOnSkillConfirmed OnSkillConfirmed;
	
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
	void BindOtherActions();
	void OnCancelActionBound();
	void OnConfirmSkillActionBound();
	
	/** Generic handler for skill input actions */
	void OnSkillActionStarted(FGameplayTagContainer AbilityTags);

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
	FOnHPChangeEvent OnHPChange;
	FOnMPChangeEvent OnMPChange;
	FOnStrengthChangeEvent OnStrengthChange;
	
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnMainUICreated OnMainUICreated;
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHPBar();
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHPAttributeBar();
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMPAttributeBar();
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateStrengthAttributeBar();
	
	UFUNCTION(BlueprintCallable, Category="Attributes")
	void CheckDeath(float InCurrentHP);
	
private:
	void OnHPAttributeChanged(const FOnAttributeChangeData& Data);
	void OnMPAttributeChanged(const FOnAttributeChangeData& Data);
	void OnStrengthAttributeChanged(const FOnAttributeChangeData& Data);
	
	FActiveGameplayEffectHandle HealthRegenInfiniteEffectSpecHandle;
#pragma endregion Attributes
	
#pragma region Movement
public:
	UFUNCTION(BlueprintCallable, Category="Movement")
	void SetFrictionZero();
	
	UFUNCTION(BlueprintCallable, Category="Movement")
	void ResetFriction();
	
	UFUNCTION(BlueprintCallable, Category="Movement")
	void PushAway(const FVector& Dir, float Strength, float DelayTime = 0.f);
	
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
