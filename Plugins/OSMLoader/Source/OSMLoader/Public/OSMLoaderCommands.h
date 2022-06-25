// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "OSMLoaderStyle.h"

class FOSMLoaderCommands : public TCommands<FOSMLoaderCommands>
{
public:

	FOSMLoaderCommands()
		: TCommands<FOSMLoaderCommands>(TEXT("OSMLoader"), NSLOCTEXT("Contexts", "OSMLoader", "OSMLoader Plugin"), NAME_None, FOSMLoaderStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};