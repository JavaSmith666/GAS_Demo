// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAS0 : ModuleRules
{
	public GAS0(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.Never;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"GAS0",
			"GAS0/Variant_Platforming",
			"GAS0/Variant_Platforming/Animation",
			"GAS0/Variant_Combat",
			"GAS0/Variant_Combat/AI",
			"GAS0/Variant_Combat/Animation",
			"GAS0/Variant_Combat/Gameplay",
			"GAS0/Variant_Combat/Interfaces",
			"GAS0/Variant_Combat/UI",
			"GAS0/Variant_SideScrolling",
			"GAS0/Variant_SideScrolling/AI",
			"GAS0/Variant_SideScrolling/Gameplay",
			"GAS0/Variant_SideScrolling/Interfaces",
			"GAS0/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
