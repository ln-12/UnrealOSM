// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "OSMLoader.h"
#include "OSMLoaderStyle.h"
#include "OSMLoaderCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Input/SButton.h" 
#include "OSMLoaderBuildingsActor.h"
#include "Engine/World.h"

static const FName OSMLoaderTabName("OSMLoader");

#define LOCTEXT_NAMESPACE "FOSMLoaderModule"

void FOSMLoaderModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FOSMLoaderStyle::Initialize();
	FOSMLoaderStyle::ReloadTextures();

	FOSMLoaderCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FOSMLoaderCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FOSMLoaderModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FOSMLoaderModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FOSMLoaderModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(OSMLoaderTabName, FOnSpawnTab::CreateRaw(this, &FOSMLoaderModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FOSMLoaderTabTitle", "OSMLoader"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FOSMLoaderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FOSMLoaderStyle::Shutdown();

	FOSMLoaderCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(OSMLoaderTabName);
}

TSharedRef<SDockTab> FOSMLoaderModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::FromString("Add an actor of type OSMLoaderBuildingsActor_Blueprint to your level, adjust the configuration tab and click \"Load Data\".\nYou can also run you scene in Play Mode an hit the \"K\" key to start loading.");
 
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)	
				.AutoWrapText(true)
			
			]
		];
}

void FOSMLoaderModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(OSMLoaderTabName);
}

void FOSMLoaderModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FOSMLoaderCommands::Get().OpenPluginWindow);
}

void FOSMLoaderModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FOSMLoaderCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOSMLoaderModule, OSMLoader)