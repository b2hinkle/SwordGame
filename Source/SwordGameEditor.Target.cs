// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SwordGameEditorTarget : TargetRules
{
    public SwordGameEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
    }
}
