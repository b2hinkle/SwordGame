// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"

#include "SwordGameGameUserSettings.generated.h"

/**
 *
 */
UCLASS()
class SWORDGAME_API USwordGameGameUserSettings : public UGameUserSettings
{
    GENERATED_BODY()

public:

    // UGameUserSettings overrides.
    virtual void LoadSettings(bool bForceReload = false) override;
    virtual void SetToDefaults() override;
    // UGameUserSettings overrides.

    static USwordGameGameUserSettings& GetChecked();

    FORCEINLINE const FString& GetAutoConnectURL() const
    {
        return AutoConnectURL;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetAutoConnectURL(FString newValue)
    {
        AutoConnectURL = MoveTemp(newValue);
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE uint8 GetAutoConnectNumTries() const
    {
        return AutoConnectNumTries;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetAutoConnectNumTries(uint8 newValue)
    {
        AutoConnectNumTries = newValue;
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE uint8 GetAutoReconnectMaxTimes() const
    {
        return AutoReconnectMaxTimes;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetAutoReconnectMaxTimes(uint8 newValue)
    {
        AutoReconnectMaxTimes = newValue;
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE bool GetIsAutoConnectEnabled() const
    {
        return bIsAutoConnectEnabled;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetIsAutoConnectEnabled(bool newValue)
    {
        bIsAutoConnectEnabled = newValue;
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE bool GetIsAutoReconnectEnabled() const
    {
        return bIsAutoReconnectEnabled;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetIsAutoReconnectEnabled(bool newValue)
    {
        bIsAutoReconnectEnabled = newValue;
    }

private:

    UFUNCTION(BlueprintPure)
    static FORCEINLINE USwordGameGameUserSettings* GetSwordGameGameUserSettingsChecked()
    {
        return &GetChecked();
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE FString BPGetAutoConnectURL() const
    {
        return AutoConnectURL;
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE FString BPGetAutoConnectURLDefaultValue() const
    {
        return FString(AutoConnectURLDefaultValue);
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE uint8 BPGetAutoConnectNumTriesDefaultValue() const
    {
        return AutoConnectNumTries;
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE uint8 BPGetAutoReconnectMaxTimesDefaultValue() const
    {
        return AutoReconnectMaxTimes;
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE bool BPGetIsAutoConnectEnabledDefaultValue() const
    {
        return bIsAutoConnectEnabledDefaultValue;
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE bool BPGetIsAutoReconnectEnabledDefaultValue() const
    {
        return bIsAutoReconnectEnabledDefaultValue;
    }

public:

    // These are the default values of our settings. @remark @Christian: [settings][version] Make sure you increment the
    // serialized version if you ever change this default, so that existing users get this value updated instead of loading the old default value.
    static constexpr FStringView AutoConnectURLDefaultValue = TEXTVIEW("mkep.nekocloud.app:7777");
    static constexpr uint8 AutoConnectNumTriesDefaultValue = 4u;
    static constexpr uint8 AutoReconnectMaxTimesDefaultValue = 8u;
    static constexpr bool bIsAutoConnectEnabledDefaultValue = true;
    static constexpr bool bIsAutoReconnectEnabledDefaultValue = true;

private:

    UPROPERTY(Config)
    FString AutoConnectURL;

    UPROPERTY(Config)
    uint8 AutoConnectNumTries = 0u;

    UPROPERTY(Config)
    uint8 AutoReconnectMaxTimes = 0u;

    UPROPERTY(Config)
    bool bIsAutoConnectEnabled = false;

    UPROPERTY(Config)
    bool bIsAutoReconnectEnabled = false;
};
