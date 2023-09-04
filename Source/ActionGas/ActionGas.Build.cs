// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionGas : ModuleRules
{
	public ActionGas(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"MotionWarping",
			"NetCore",
			"CoreUObject"
		});
		DynamicallyLoadedModuleNames.AddRange(new string[]
		{
			"NetworkReplayStreaming",
			"LocalFileNetworkReplayStreaming"
		});
	}
}
