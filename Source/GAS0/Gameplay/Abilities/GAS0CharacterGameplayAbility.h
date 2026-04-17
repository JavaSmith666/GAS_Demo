// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DataAssets/SkillConfig.h"
#include "GAS0CharacterGameplayAbility.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;

UENUM(BlueprintType)
enum class ECostType : uint8
{
    Default,
    HP,
    MP,
    Strength
};

USTRUCT(BlueprintType)
struct FGameplayAbilityInfo
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilityInfo")
    int32 AbilityIndex;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilityInfo")
    float CD;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilityInfo")
    ECostType CostType;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilityInfo")
    float CostValue;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilityInfo")
    UMaterialInstance* MI;
    
    FGameplayAbilityInfo() : AbilityIndex(-1), CD(0), CostType(ECostType::HP), CostValue(0.0f), MI(nullptr) {}
    FGameplayAbilityInfo(int32 InAbilityIndex, float InCD, ECostType InCostType, float InCostValue, UMaterialInstance* InMI)
        : AbilityIndex(InAbilityIndex), CD(InCD), CostType(InCostType), CostValue(InCostValue), MI(InMI) {}
};

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
    UMaterialInstance* AbilityMaterialInstance = nullptr;
    
    UFUNCTION(BlueprintCallable)
    FGameplayAbilityInfo GetAbilityInfo(int32 Level) const;
    
    UFUNCTION(BlueprintNativeEvent)
    void InitSkillIcon(int32 InAbilityIndex);

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
    
    UPROPERTY(Transient)
    USkillConfig* RoleSkillConfig = nullptr;

private:
    void HandleFireMontageEnded(bool bWasCancelled);
};
