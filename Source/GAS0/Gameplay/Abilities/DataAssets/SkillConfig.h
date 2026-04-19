#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillConfig.generated.h"

class UGameplayEffect;

/**
 * Base data asset for all skill configurations.
 */
UCLASS(BlueprintType)
class USkillConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Montage played when this ability activates (for example, fire animation). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Animation")
	TObjectPtr<UAnimMontage> FireMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Info")
	int32 AbilityIndex = -1;
};

UCLASS(BlueprintType)
class UMeleeConfig : public USkillConfig
{
	GENERATED_BODY()

public:
	
};

UCLASS(BlueprintType)
class UHealthRegenConfig : public USkillConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffect")
	TSubclassOf<UGameplayEffect> HealthRegenEffect;
};

UCLASS(BlueprintType)
class UDashConfig : public USkillConfig
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ForwardImpulse = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffect")
	TSubclassOf<UGameplayEffect> DashDamageEffect;
};
