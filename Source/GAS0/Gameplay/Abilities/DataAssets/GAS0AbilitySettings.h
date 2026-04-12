#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GAS0AbilitySettings.generated.h"

/**
 * Developer settings for GAS0 project abilities.
 * Allows configuring global data table references and other settings in the Project Settings menu.
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="GAS0 Ability Settings"))
class GAS0_API UGAS0AbilitySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGAS0AbilitySettings();

	/** Global Data Table for character skill slots. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "DataTables")
	TSoftObjectPtr<UDataTable> CharacterSkillTable;
};
