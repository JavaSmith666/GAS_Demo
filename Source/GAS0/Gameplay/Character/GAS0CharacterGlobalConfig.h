#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GAS0CharacterGlobalConfig.generated.h"

class UGameplayEffect;

UCLASS(BlueprintType)
class UGAS0CharacterGlobalConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> CancelAbilityAction;
};