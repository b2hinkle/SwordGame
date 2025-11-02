// Fill out your copyright notice in the Description page of Project Settings.

#include "SwordGamePlayerController.h"

#include "ConsoleSettings.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Console.h"
#include "../SwordGameCharacter.h"

namespace
{
    FDelayedAutoRegisterHelper delayedAutoRegisterHelper(
        EDelayedRegisterRunPhase::EndOfEngineInit,
        []()
        {
            UConsole::RegisterConsoleAutoCompleteEntries.AddStatic(
                [](TArray<FAutoCompleteCommand>& autoCompleteEntries)
                {
                    TArray<FString> mapNames = ASwordGamePlayerController::GetAllAutoCompleteMapNames();

                    {
                        const int32 newNum = autoCompleteEntries.Num() + mapNames.Num();
                        autoCompleteEntries.Reserve(FMath::Max(autoCompleteEntries.Max(), newNum));
                    }

                    const FName commandName = GET_FUNCTION_NAME_CHECKED_OneParam(ASwordGamePlayerController, NinjaTravel, const FName&);

                    for (FString& mapName : mapNames)
                    {
                        FAutoCompleteCommand& autoCompleteCommand = autoCompleteEntries.Emplace_GetRef();
                        autoCompleteCommand.Command = WriteToString<64>(commandName, TEXT(' ')) + MoveTemp(mapName);

                        autoCompleteCommand.Color = GetDefault<UConsoleSettings>()->AutoCompleteCommandColor;
                    }
                }
                );
        }
        );
}

ASwordGamePlayerController::ASwordGamePlayerController(const FObjectInitializer& objectInitializer)
    : Super(objectInitializer)
{
}

void ASwordGamePlayerController::EndPlayingState()
{
    Super::EndPlayingState();

    if (APawn* pawn = GetPawn())
    {
        if (ASwordGameCharacter* swordGameCharacter = Cast<ASwordGameCharacter>(pawn))
        {
            swordGameCharacter->SetRemoteViewYaw(0.f);
        }
    }
}

FString ASwordGamePlayerController::GetNetworkURL(const UObject* worldContextObject)
{
    if (!worldContextObject)
    {
        return FString();
    }

    const UWorld* world = worldContextObject->GetWorld();
    if (!world)
    {
        return FString();
    }

    return world->GetAddressURL();
}

TArray<FString> ASwordGamePlayerController::GetAllAutoCompleteMapNames()
{
    // Note: [duplicate-code-engine] This code is completely copied from `UConsole::BuildRuntimeAutoCompleteList()`.

    auto FindPackagesInDirectory = [](TArray<FString>& OutPackages, const FString& InPath)
    {
        FString PackagePath;
        if (FPackageName::TryConvertFilenameToLongPackageName(InPath, PackagePath))
        {
            if (FAssetRegistryModule* AssetRegistryModule = FModuleManager::LoadModulePtr<FAssetRegistryModule>(TEXT("AssetRegistry")))
            {
                TArray<FAssetData> Assets;
                AssetRegistryModule->Get().GetAssetsByPath(FName(*PackagePath), Assets, true);

                for (const FAssetData& Asset : Assets)
                {
                    if (!!(Asset.PackageFlags & PKG_ContainsMap) && Asset.IsUAsset())
                    {
                        OutPackages.AddUnique(Asset.AssetName.ToString());
                    }
                }
            }
        }
        TArray<FString> Filenames;
        FPackageName::FindPackagesInDirectory(Filenames, InPath);

        for (const FString& Filename : Filenames)
        {
            const int32 NameStartIdx = Filename.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
            const int32 ExtIdx = Filename.Find(*FPackageName::GetMapPackageExtension(), ESearchCase::IgnoreCase, ESearchDir::FromEnd);

            if (NameStartIdx != INDEX_NONE && ExtIdx != INDEX_NONE)
            {
                OutPackages.AddUnique(Filename.Mid(NameStartIdx + 1, ExtIdx - NameStartIdx - 1));
            }
        }
    };

    TArray<FString> Packages;
    for (const FString& MapPath : GetDefault<UConsoleSettings>()->AutoCompleteMapPaths)
    {
        FindPackagesInDirectory(Packages, FString::Printf(TEXT("%s%s"), *FPaths::ProjectDir(), *MapPath));
    }

    FindPackagesInDirectory(Packages, FPaths::GameUserDeveloperDir());

    return Packages;
}

void ASwordGamePlayerController::NinjaJoin(const FName& address) const
{
    FStringBuilderBase&& command = WriteToString<64>(TEXT("open "), address);

    ThisClass& thisMutable = const_cast<ThisClass&>(*this);

    constexpr bool shouldWriteToLog = true;
    thisMutable.ConsoleCommand(FString(MoveTemp(command)), shouldWriteToLog);
}

void ASwordGamePlayerController::NinjaRestart()
{
    HandleRestartWorld();
}

void ASwordGamePlayerController::NinjaTravel(const FName& mapName)
{
    HandleTravelToWorld(mapName);
}

void ASwordGamePlayerController::NinjaForceReconnectAllPlayers() const
{
    HandleForceReconnectAllPlayers();
}

void ASwordGamePlayerController::NinjaToggleModTools() const
{
    HandleToggleModTools();
}

void ASwordGamePlayerController::NinjaEnableFriendlyFireForRandomPlayer() const
{
    HandleEnableFriendlyFireForRandomPlayer();
}

void ASwordGamePlayerController::NinjaBallz(const int howMany) const
{
    HandleBallz(howMany);
}

void ASwordGamePlayerController::NinjaClearObjects() const
{
    HandleClearObjects();
}

void ASwordGamePlayerController::NinjaLaunchSelf(const float force) const
{
    HandleLaunchSelf(force);
}

void ASwordGamePlayerController::NinjaToggleSwitchViewTargetOnFriendlyFire() const
{
    HandleToggleSwitchViewTargetOnFriendlyFire();
}

void ASwordGamePlayerController::NinjaMrIncredi1() const
{
    HandleMrIncredi1();
}

void ASwordGamePlayerController::NinjaMrIncredi2() const
{
    HandleMrIncredi2();
}

void ASwordGamePlayerController::NinjaMrIncredi3() const
{
    HandleMrIncredi3();
}

void ASwordGamePlayerController::NinjaSetPawnRelative3DScale(const float valueX, const float valueY, const float valueZ) const
{
    HandleSetPawnRelative3DScale(valueX, valueY, valueZ);
}

void ASwordGamePlayerController::NinjaGiveChargeAbility() const
{
    HandleGiveChargeAbility();
}

void ASwordGamePlayerController::NinjaGiveMineAbility() const
{
    HandleGiveMineAbility();
}

void ASwordGamePlayerController::NinjaGiveTeleportAbility() const
{
    HandleGiveTeleportAbility();
}

void ASwordGamePlayerController::NinjaDisableInvisibilityForTheInnocent() const
{
    HandleDisableInvisibilityForTheInnocent();
}

void ASwordGamePlayerController::NinjaEnableInvisibilityForTheInnocent() const
{
    HandleEnableInvisibilityForTheInnocent();
}

void ASwordGamePlayerController::NinjaChangeMaxInvisibility(const float value) const
{
    HandleChangeMaxInvisibilityStamina(value);
}

void ASwordGamePlayerController::NinjaServerExec(const FString& commandToExecuteOnTheServer)
{
    ServerNinjaExec(commandToExecuteOnTheServer);
}

void ASwordGamePlayerController::ServerNinjaExec_Implementation(const FString& command)
{
    constexpr bool shouldWriteToLog = true;
    ConsoleCommand(command, shouldWriteToLog);
}
