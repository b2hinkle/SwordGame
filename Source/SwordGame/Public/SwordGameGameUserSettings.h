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

    FORCEINLINE const FString& GetAutoOpenURL() const
    {
        return AutoOpenURL;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetAutoOpenURL(FString newValue)
    {
        AutoOpenURL = MoveTemp(newValue);
    }

private:

    UFUNCTION(BlueprintPure)
    static FORCEINLINE USwordGameGameUserSettings* GetSwordGameGameUserSettingsChecked()
    {
        return &GetChecked();
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE FString BPGetAutoOpenURL() const
    {
        return AutoOpenURL;
    }

    UFUNCTION(BlueprintPure)
    FORCEINLINE FString BPGetAutoOpenURLDefaultValue() const
    {
        return FString(AutoOpenURLDefaultValue);
    }

public:

    // @remark @Christian: [settings][version] Make sure you increment the serialized version if you ever change this default, so that existing users get this value updated instead of loading the old default value.
    static constexpr FStringView AutoOpenURLDefaultValue = TEXTVIEW("127.0.0.1");

private:

    UPROPERTY(Config)
    FString AutoOpenURL;
};
