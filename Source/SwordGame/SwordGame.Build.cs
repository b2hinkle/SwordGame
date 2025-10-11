// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SwordGame : ModuleRules
{
    public SwordGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "Slate",
                "UMG"
            }
        );

        PrivateDependencyModuleNames.AddRange(new string[] { "EngineSettings", "Slate"});
    }
}
