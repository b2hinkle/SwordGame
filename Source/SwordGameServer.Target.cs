// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class SwordGameServerTarget : TargetRules
{
    public SwordGameServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        // Our game is jank lol. We have lots of features through console as of now.
        {
            bUseLoggingInShipping = true;
        }
    }
}
