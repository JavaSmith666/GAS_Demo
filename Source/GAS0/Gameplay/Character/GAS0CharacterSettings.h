#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GAS0CharacterSettings.generated.h"

class UGAS0CharacterGlobalConfig;

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="GAS0 Character Settings"))
class UGAS0CharacterSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "DataAssets")
	TSoftObjectPtr<UGAS0CharacterGlobalConfig> CharacterGlobalConfig;
};
