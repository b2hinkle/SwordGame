// Fill out your copyright notice in the Description page of Project Settings.

#include "SwordGameGameUserSettings.h"

void USwordGameGameUserSettings::LoadSettings(bool bForceReload)
{
    Super::LoadSettings(bForceReload);
}

void USwordGameGameUserSettings::SetToDefaults()
{
    Super::SetToDefaults();

    SetAutoConnectURL(FString(AutoConnectURLDefaultValue));
    SetAutoConnectNumTries(AutoConnectNumTriesDefaultValue);
    SetAutoReconnectMaxTimes(AutoReconnectMaxTimesDefaultValue);
    SetIsAutoConnectEnabled(bIsAutoConnectEnabledDefaultValue);
    SetIsAutoReconnectEnabled(bIsAutoReconnectEnabledDefaultValue);
}

USwordGameGameUserSettings& USwordGameGameUserSettings::GetChecked()
{
    check(GEngine);
    UGameUserSettings* gameUserSettings = GEngine->GetGameUserSettings();
    check(gameUserSettings);
    ThisClass* gameUserSettingsCasted = Cast<ThisClass>(gameUserSettings);
    check(gameUserSettingsCasted);
    return *gameUserSettingsCasted;
}
