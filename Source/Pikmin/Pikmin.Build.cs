// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Pikmin : ModuleRules
{
	public Pikmin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

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
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Pikmin",
			"Pikmin/Variant_Platforming",
			"Pikmin/Variant_Platforming/Animation",
			"Pikmin/Variant_Combat",
			"Pikmin/Variant_Combat/AI",
			"Pikmin/Variant_Combat/Animation",
			"Pikmin/Variant_Combat/Gameplay",
			"Pikmin/Variant_Combat/Interfaces",
			"Pikmin/Variant_Combat/UI",
			"Pikmin/Variant_SideScrolling",
			"Pikmin/Variant_SideScrolling/AI",
			"Pikmin/Variant_SideScrolling/Gameplay",
			"Pikmin/Variant_SideScrolling/Interfaces",
			"Pikmin/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
