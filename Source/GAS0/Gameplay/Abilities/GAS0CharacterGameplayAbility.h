// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DataAssets/SkillConfig.h"
#include "Gameplay/AttributeSet/BaseAttributeSet.h"
#include "GAS0CharacterGameplayAbility.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
class AGAS0Character;

/**
 * Minimal gameplay ability for GAS0 characters.
 * This class is intentionally small — extend it with ability logic as needed.
 */
UCLASS()
class UGAS0CharacterGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UGAS0CharacterGameplayAbility();
    
    /** Called when the ability is given to an AbilitySystemComponent */
    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
    
    /** Public setter for skill configuration. */
    void SetRoleSkillConfig(USkillConfig* InConfig) { RoleSkillConfig = InConfig; }
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UMaterialInstance> AbilityMaterialInstance;
    
    UFUNCTION(BlueprintCallable)
    int32 GetAbilityIndex() const { return AbilityIndex; }
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Animation")
    TSoftObjectPtr<UAnimMontage> FireMontage;
    
    virtual bool PlayFireMontage();

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;
    
    virtual bool OnGAS0CharacterGameplayAbilityActivated(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData);

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;
    
    virtual void PreGAS0CharacterGameplayAbilityEnded(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled);

    UFUNCTION()
    void OnFireMontageCompleted();

    UFUNCTION()
    void OnFireMontageBlendOut();

    UFUNCTION()
    void OnFireMontageInterrupted();

    UFUNCTION()
    void OnFireMontageCancelled();
    
    UFUNCTION()
    void OnMainUICreated();

protected:
    UPROPERTY(Transient)
    TObjectPtr<UAbilityTask_PlayMontageAndWait> ActiveMontageTask;
    
    UPROPERTY(Transient)
    USkillConfig* RoleSkillConfig = nullptr;
    
    int32 AbilityIndex = -1;
    
    UPROPERTY(Transient)
    AGAS0Character* OwnerCharacter = nullptr;
    
    UPROPERTY(Transient)
    APlayerController* OwnerPlayerController = nullptr;
    
private:
    void HandleFireMontageEnded(bool bWasCancelled);
};
