#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseAttributeSet.generated.h"

// Standard GAS helper macro for generated attribute accessor functions.
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBaseAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	FGameplayAttributeData Hp;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Hp)

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	FGameplayAttributeData MaxHp;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHp)
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	FGameplayAttributeData Mp;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Mp)
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	FGameplayAttributeData MaxMp;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxMp)
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Strength)
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	FGameplayAttributeData MaxStrength;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxStrength)
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	FGameplayAttributeData SkillLevel;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, SkillLevel)
};

