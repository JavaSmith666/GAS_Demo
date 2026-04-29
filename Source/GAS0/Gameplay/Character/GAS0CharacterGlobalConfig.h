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
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> CancelAbilityAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ConfirmSkillAbilityAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TArray<UInputMappingContext*> SkillMappingContexts;
	
	UPROPERTY(EditAnywhere, Category="Camera")
	FVector LockCameraRelativeLocation;
	
	UPROPERTY(EditAnywhere, Category="Camera")
	FRotator LockCameraRelativeRotation;
};