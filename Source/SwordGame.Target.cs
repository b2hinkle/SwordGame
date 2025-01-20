// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SwordGameTarget : TargetRules
{
    public SwordGameTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
    }
}
