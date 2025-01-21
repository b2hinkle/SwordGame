// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/**
 * @brief The editor module.
 */
class FSwordGameEditorModule : public FDefaultGameModuleImpl
{
protected:

    // ~ IModuleInterface override.
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    // ~ IModuleInterface override.
};
