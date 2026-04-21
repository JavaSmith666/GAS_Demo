#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GAS0AbilitySettings.generated.h"

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="GAS0 Ability Settings"))
class UGAS0AbilitySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGAS0AbilitySettings();

	/** Global Data Table for character skill slots. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "DataTables")
	TSoftObjectPtr<UDataTable> CharacterSkillTable;
};
