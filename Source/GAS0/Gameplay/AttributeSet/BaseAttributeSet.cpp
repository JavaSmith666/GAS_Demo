#include "BaseAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UBaseAttributeSet::UBaseAttributeSet()
{
	InitMaxHp(100.0f);
	InitHp(GetMaxHp());
}

void UBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Hp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxHp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Mp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxMp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxStrength, COND_None, REPNOTIFY_Always);
}

void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// if (Attribute == GetMaxHpAttribute())
	// {
	// 	NewValue = FMath::Max(0.0f, NewValue);
	// 	SetHp(FMath::Clamp(GetHp(), 0.0f, NewValue));
	// }
}

void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// if (Data.EvaluatedData.Attribute == GetHpAttribute())
	// {
	// 	SetHp(FMath::Clamp(GetHp(), 0.0f, GetMaxHp()));
	// }
	// else if (Data.EvaluatedData.Attribute == GetMaxHpAttribute())
	// {
	// 	SetMaxHp(FMath::Max(0.0f, GetMaxHp()));
	// 	SetHp(FMath::Clamp(GetHp(), 0.0f, GetMaxHp()));
	// }
}