#pragma once

#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <limits>
#include <random>
#include <fstream>
#include "earcut.hpp"
#include "PolygonTools.h"
#include "schema_generated.h"
#include "HeightmapImportActor.h"
#include "POI_Actor.h"
#include "OSMLoaderProceduralMeshActor.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h" 
#include "Containers/UnrealString.h" 
#include "Factories/MaterialFactoryNew.h"
#include "UObject/ConstructorHelpers.h" 
#include "Materials/MaterialInstanceDynamic.h" 
#include "HAL/PlatformFilemanager.h" 
#include "Misc/FileHelper.h" 
#include "Math/UnrealMathUtility.h"
#include "Misc/ScopedSlowTask.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshActor.h" 
#include "OSMLoaderSplineActor.h"
#include "OSMLoaderStaticMeshActor.h"
#include "Engine/Blueprint.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/FileManagerGeneric.h" 
#include "OSMLoaderBuildingsActor.generated.h"

#define XYZSize 2000

namespace mapbox {
	namespace util {
		template <>
		struct nth<0, FVector> {
			inline static auto get(const FVector& t) {
				return t.X;
			};
		};
		template <>
		struct nth<1, FVector> {
			inline static auto get(const FVector& t) {
				return t.Y;
	        };
	    };
	}
};

UCLASS()
class OSMLOADER_API AOSMLoaderBuildingsActor: public AActor {
	GENERATED_BODY()
	
public:	
	AOSMLoaderBuildingsActor();

private:
	float calculateArea(const TArray<FVector>& Polygon);
	TArray<FString> getFilesInFolder(FString Directory, FString Extension);
	void spawnStaticMeshActor(FVector Location, UStaticMesh* staticMesh);

	AHeightmapImportActor* heightmapImportActor = nullptr; 
	APOI_Actor* POI_Actor = nullptr;

	void RemoveActor(AActor* foundActor);

	//TEMPLATE Load Obj From Path
	template <typename ObjClass>
	static FORCEINLINE ObjClass* LoadObjFromPath(const FName& Path) {
		if (Path == NAME_None) return NULL;

		return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
	}

	// Load Material From Path 
	static FORCEINLINE UMaterialInterface* LoadMatFromPath(const FName& Path) {
		if (Path == NAME_None) return NULL;

		return LoadObjFromPath<UMaterialInterface>(Path);
	}

	// Load Static Mesh From Path 
	static FORCEINLINE UStaticMesh* LoadMeshFromPath(const FName& Path) {
		if (Path == NAME_None) return NULL;

		return LoadObjFromPath<UStaticMesh>(Path);
	}

	/* TODO these varibles were used to set the right spline blueprint in the editor

	UPROPERTY(EditAnywhere, Category = "Customization")
		TSubclassOf<class AOSMLoaderSplineActor> cyclewayBlueprint;

	UPROPERTY(EditAnywhere, Category = "Customization")
		TSubclassOf<class AOSMLoaderSplineActor> footwayBlueprint;

	UPROPERTY(EditAnywhere, Category = "Customization")
		TSubclassOf<class AOSMLoaderSplineActor> highwayDefaultBlueprint;

	UPROPERTY(EditAnywhere, Category = "Customization")
		TSubclassOf<class AOSMLoaderSplineActor> highwayLargeBlueprint;

	UPROPERTY(EditAnywhere, Category = "Customization")
		TSubclassOf<class AOSMLoaderSplineActor> highwayMediumBlueprint;

	UPROPERTY(EditAnywhere, Category = "Customization")
		TSubclassOf<class AOSMLoaderSplineActor> highwaySmallBlueprint;

	UPROPERTY(EditAnywhere, Category = "Customization")
		TSubclassOf<class AOSMLoaderSplineActor> riverBlueprint;

	UPROPERTY(EditAnywhere, Category = "Customization")
		TSubclassOf<class AOSMLoaderSplineActor> streamBlueprint;
	*/

	// The file name inside the Data folder.
	UPROPERTY(EditAnywhere, Category = "Customization")
		FString mapName = "333825642_dgm1.bin";

	// Defines which adjust algorithm is used -> some are not completly implemented, so better don't change
	bool completeGroundAdjust = true;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Customization")
		void LoadData();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Customization")
		void ResetData();

	bool parseBuildings();

	// textures for house walls
	TArray<UMaterialInterface*> wallTextures;

	// textures for house roofs
	TArray<UMaterialInterface*> roofTextures;

	// landscape textures
	UMaterialInterface* water;
	UMaterialInterface* highway_large;
	UMaterialInterface* highway_medium;
	UMaterialInterface* highway_small;
	UMaterialInterface* cycleway;
	UMaterialInterface* footway;
	UMaterialInterface* asphalt;
	UMaterialInterface* test;

	// trees
	UStaticMesh* treeMesh;

	// lamps
	UStaticMesh* lampMesh;

	// vegetation
	TArray<UStaticMesh*> vegetationArray;
};
