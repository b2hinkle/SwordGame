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
    FORCEINLINE bool GetIsAutoConnectEnabled() const
    {
        return bIsAutoConnectEnabled;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetIsAutoConnectEnabled(bool newValue)
    {
        bIsAutoConnectEnabled = newValue;
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
    FORCEINLINE bool BPGetIsAutoConnectEnabledDefaultValue() const
    {
        return bIsAutoConnectEnabledDefaultValue;
    }

public:

    // These are the default values of our settings. @remark @Christian: [settings][version] Make sure you increment the
    // serialized version if you ever change this default, so that existing users get this value updated instead of loading the old default value.
    static constexpr FStringView AutoConnectURLDefaultValue = TEXTVIEW("127.0.0.1");
    static constexpr bool bIsAutoConnectEnabledDefaultValue = true;

private:

    UPROPERTY(Config)
    FString AutoConnectURL;

    UPROPERTY(Config)
    bool bIsAutoConnectEnabled = false;
};
