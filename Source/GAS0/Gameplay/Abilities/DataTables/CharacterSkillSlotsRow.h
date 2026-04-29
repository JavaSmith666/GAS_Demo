#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterSkillSlotsRow.generated.h"

class USkillConfig;
class UInputAction;
class UGameplayAbility;

USTRUCT(BlueprintType)
struct FSkillSlotEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText SkillDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<UGameplayAbility> AbilityClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer AbilityTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USkillConfig> SkillConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UInputAction> ActivateAction;
};

USTRUCT(BlueprintType)
struct FCharacterSkillSlotsRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText CharacterName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSkillSlotEntry DefaultSkill;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSkillSlotEntry> SlotSkills;
};
