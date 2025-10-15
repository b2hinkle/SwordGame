// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SwordGameTarget : TargetRules
{
    public SwordGameTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        // Our game is jank lol. We have lots of features through console as of now.
        {
            bUseConsoleInShipping = true;
            bUseExecCommandsInShipping = true;
            bUseLoggingInShipping = true;
        }
    }
}
