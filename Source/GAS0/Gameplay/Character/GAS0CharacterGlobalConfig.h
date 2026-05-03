#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GAS0CharacterGlobalConfig.generated.h"

class UGameplayEffect;
class UAnimMontage;
class UInputMappingContext;

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
	
	UPROPERTY(EditAnywhere, Category="Montage")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditAnywhere, Category="Montage")
	TObjectPtr<UAnimMontage> StunMontage;
	
	UPROPERTY(EditAnywhere, Category="Effect")
	TSubclassOf<UGameplayEffect> HealthRegenInfiniteEffect;
	
	UPROPERTY(EditAnywhere, Category="Effect")
	TSubclassOf<UGameplayEffect> DashDamageEffect;
	
	UPROPERTY(EditAnywhere, Category="Impulse")
	float DashImpulse = 1500.f;
};