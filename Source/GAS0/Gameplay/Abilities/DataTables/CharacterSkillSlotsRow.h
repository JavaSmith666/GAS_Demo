#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterSkillSlotsRow.generated.h"

class USkillConfig;
class UInputAction;
class UGameplayAbility;

/**
 * Encapsulates skill-specific data used within the character skill row.
 */
USTRUCT(BlueprintType)
struct FSkillSlotEntry
{
	GENERATED_BODY()

	/** Optional readable display name for UI/debug. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText SkillDisplayName;

	/** Ability class driven by this skill config. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<UGameplayAbility> AbilityClass;
	
	/** The configuration data asset for this skill. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USkillConfig> SkillConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UInputAction> ActivateAction;
};

/**
 * One row in character skill slot DataTable.
 * Stores character name and three slot skill entries.
 */
USTRUCT(BlueprintType)
struct FCharacterSkillSlotsRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Character identifier/name for this row. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText CharacterName;

	/** Skill entry bound to slot 0. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSkillSlotEntry Slot0;

	/** Skill entry bound to slot 1. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSkillSlotEntry Slot1;

	/** Skill entry bound to slot 2. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSkillSlotEntry Slot2;
};
