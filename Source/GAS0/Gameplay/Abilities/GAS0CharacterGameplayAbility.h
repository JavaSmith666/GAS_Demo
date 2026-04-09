// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GAS0CharacterGameplayAbility.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;

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

protected:

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;

    /** Plays fire montage and binds end callbacks. Override for custom play behavior. */
    virtual bool PlayFireMontage();

    /** Montage played when this ability activates (for example, fire animation). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Animation")
    TObjectPtr<UAnimMontage> FireMontage;

    UFUNCTION()
    void OnFireMontageCompleted();

    UFUNCTION()
    void OnFireMontageBlendOut();

    UFUNCTION()
    void OnFireMontageInterrupted();

    UFUNCTION()
    void OnFireMontageCancelled();

protected:
    UPROPERTY(Transient)
    TObjectPtr<UAbilityTask_PlayMontageAndWait> ActiveMontageTask;

private:
    void HandleFireMontageEnded(bool bWasCancelled);
};

