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

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_HP, Category = "Attributes")
	FGameplayAttributeData HP;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, HP)

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	FGameplayAttributeData MaxHP;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHP)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MP, Category = "Attributes")
	FGameplayAttributeData MP;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MP)
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	FGameplayAttributeData MaxMP;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxMP)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Strength, Category = "Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Strength)
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	FGameplayAttributeData MaxStrength;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxStrength)
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	FGameplayAttributeData SkillLevel;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, SkillLevel)
	
protected:
	UFUNCTION()
	void OnRep_HP(FGameplayAttributeData& RepData);
	
	UFUNCTION()
	void OnRep_MP(FGameplayAttributeData& RepData);
	
	UFUNCTION()
	void OnRep_Strength(FGameplayAttributeData& RepData);
};

