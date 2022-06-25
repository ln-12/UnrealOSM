// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "OSMLoaderCommands.h"

#define LOCTEXT_NAMESPACE "FOSMLoaderModule"

void FOSMLoaderCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "OSMLoader", "Bring up OSMLoader window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
