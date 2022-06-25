#include "OSMLoaderBuildingsActor.h"

// #include "Public/EditorLevelLibrary.h"
// #include "IMeshMergeUtilities.h" 
// #include "IMergeActorsModule.h"

// Sets default values
AOSMLoaderBuildingsActor::AOSMLoaderBuildingsActor() {
	UE_LOG(LogTemp, Warning, TEXT("AOSMLoaderBuildingsActor"));

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	UE_LOG(LogTemp, Display, TEXT("Loading textures..."));  

	// load custom material
	wallTextures.Add(LoadMatFromPath(FName("/Game/Materials/Walls/sc1tdbip_8K_Albedo_window")));
	wallTextures.Add(LoadMatFromPath(FName("/Game/Materials/Walls/thpjbidg_8K_Albedo_window")));
	wallTextures.Add(LoadMatFromPath(FName("/Game/Materials/Walls/bricks_window")));
	wallTextures.Add(LoadMatFromPath(FName("/Game/Materials/Walls/concrete_window")));

	roofTextures.Add(LoadMatFromPath(TEXT("Material'/Game/Materials/Roofs/220_material.220_material'")));
	roofTextures.Add(LoadMatFromPath(TEXT("Material'/Game/Materials/Roofs/roof_tiles_arched2_material.roof_tiles_arched2_material'")));
	roofTextures.Add(LoadMatFromPath(TEXT("Material'/Game/Materials/Roofs/roof_tiles_weird_material.roof_tiles_weird_material'")));

	water = LoadMatFromPath(TEXT("Material'/Game/Materials/Landscape/lake.lake'"));
	highway_large = LoadMatFromPath(TEXT("Material'/Game/Materials/Landscape/street_asphalt_large_material.street_asphalt_large_material'"));
	highway_medium = LoadMatFromPath(TEXT("Material'/Game/Materials/Landscape/street_asphalt_medium_material.street_asphalt_medium_material'"));
	cycleway = LoadMatFromPath(TEXT("Material'/Game/Materials/Landscape/concrete_tiles.concrete_tiles'"));
	footway = LoadMatFromPath(TEXT("Material'/Game/Materials/Landscape/cobblestone_smooth.cobblestone_smooth'"));
	asphalt = LoadMatFromPath(TEXT("Material'/Game/Materials/Landscape/asphalt.asphalt'"));
	test = LoadMatFromPath(TEXT("Material'/Game/Materials/test.test'"));

	UE_LOG(LogTemp, Display, TEXT("Loading meshes..."));

	// load meshes
	treeMesh = LoadMeshFromPath(FName("/Game/Scenery/Trees/HillTree_02"));

	lampMesh = LoadMeshFromPath(FName("/Game/Scenery/Lamps/street_lamp"));

	// TODO dynamic search in file system?
	vegetationArray.Add(LoadMeshFromPath(FName("/Game/MS_GardenLawn/3D_Plant/01_Grass_Clumps_rbdoo/rbdoo_Var1_LOD0")));
	vegetationArray.Add(LoadMeshFromPath(FName("/Game/MS_GardenLawn/3D_Plant/01_Grass_Clumps_rbdoo/rbdoo_Var2_LOD0")));
	vegetationArray.Add(LoadMeshFromPath(FName("/Game/MS_GardenLawn/3D_Plant/01_Grass_Clumps_rbdoo/rbdoo_Var3_LOD0")));
	vegetationArray.Add(LoadMeshFromPath(FName("/Game/MS_GardenLawn/3D_Plant/02_Tall_Grass_rbEla/rbEla_Var1_LOD0")));
	vegetationArray.Add(LoadMeshFromPath(FName("/Game/MS_GardenLawn/3D_Plant/02_Tall_Grass_rbEla/rbEla_Var2_LOD0")));
	vegetationArray.Add(LoadMeshFromPath(FName("/Game/MS_GardenLawn/3D_Plant/02_Tall_Grass_rbEla/rbEla_Var3_LOD0")));
	vegetationArray.Add(LoadMeshFromPath(FName("/Game/MS_GardenLawn/3D_Plant/04_Grass_Clumps_qmBr2/qmBr2_Var1_LOD0")));
	vegetationArray.Add(LoadMeshFromPath(FName("/Game/MS_GardenLawn/3D_Plant/04_Grass_Clumps_qmBr2/qmBr2_Var2_LOD0")));
	vegetationArray.Add(LoadMeshFromPath(FName("/Game/MS_GardenLawn/3D_Plant/04_Grass_Clumps_qmBr2/qmBr2_Var3_LOD0")));
	vegetationArray.Add(LoadMeshFromPath(FName("/Game/MS_GardenLawn/3D_Plant/05_Tall_Grass_rbEkP/rbEkP_Var1_LOD0")));
	vegetationArray.Add(LoadMeshFromPath(FName("/Game/MS_GardenLawn/3D_Plant/05_Tall_Grass_rbEkP/rbEkP_Var2_LOD0")));
	vegetationArray.Add(LoadMeshFromPath(FName("/Game/MS_GardenLawn/3D_Plant/05_Tall_Grass_rbEkP/rbEkP_Var3_LOD0")));

	srand(time(NULL));

	UE_LOG(LogTemp, Display, TEXT("Done."));
}

TArray<FString> AOSMLoaderBuildingsActor::getFilesInFolder(FString Directory, FString Extension) {
	TArray<FString> output;
	if (FPaths::DirectoryExists(Directory + "/")) {
		FFileManagerGeneric::Get().FindFilesRecursive(output, *Directory, *Extension, true, false);
	}

	for (auto filename : output) {
		FString p = FPaths::ConvertRelativePathToFull(filename);
		UE_LOG(LogTemp, Display, TEXT("Found asset: %s"), *filename);
		UE_LOG(LogTemp, Display, TEXT("Path: %s"), *p);

		
	}

	return output;
}

// taken from https://github.com/ue4plugins/StreetMap/blob/master/Source/StreetMapRuntime/PolygonTools.h
float AOSMLoaderBuildingsActor::calculateArea(const TArray<FVector>& Polygon) {
	const int32 PointCount = Polygon.Num();

	float HalfArea = 0.0f;
	for (int32 P = PointCount - 1, Q = 0; Q < PointCount; P = Q++)
	{
		HalfArea += Polygon[P].X * Polygon[Q].Y - Polygon[Q].X * Polygon[P].Y;
	}
	return HalfArea * 0.5f;
}

void AOSMLoaderBuildingsActor::spawnStaticMeshActor(FVector Location, UStaticMesh* staticMesh) {
	// Get world context
	UWorld* World = this->GetWorld();
	if (World) {
		// we don't need any rotation
		FRotator Rotation(0.0f, 0.0f, 0.0f);

		// params can also be empty
		FActorSpawnParameters SpawnParams;

		// spawn actor in world
		AOSMLoaderStaticMeshActor* result = World->SpawnActor<AOSMLoaderStaticMeshActor>(AOSMLoaderStaticMeshActor::StaticClass(), Location, Rotation, SpawnParams);

		if (result) {
			// trees can't move
			result->SetMobility(EComponentMobility::Stationary);

			// get mesh component from actor
			UStaticMeshComponent* comp = result->GetStaticMeshComponent();

			if (comp) {
				// better double check if our asset was loaded
				if (staticMesh) {
					comp->SetStaticMesh(staticMesh);
				}
			}
		}
	}
}

bool AOSMLoaderBuildingsActor::parseBuildings() {
	UWorld* world = this->GetWorld();

	const FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnParams;

	// Spawn new AHeightmapImportActor
	if (world) {

		FString TexturePath = "/Game/Materials/Landscape/dop20c_" + mapName.Mid(0, 5) + "_" + mapName.Mid(5, 4);
		UMaterialInterface* GroundMaterial = AOSMLoaderBuildingsActor::LoadMatFromPath(FName("/Game/Materials/Landscape/Luftbild"));
		UMaterialInstanceDynamic* DynamicGroundMaterial = UMaterialInstanceDynamic::Create(GroundMaterial, NULL);
		bool DefaultTexture = false;

		UTexture* Texture = LoadObjFromPath<UTexture>(FName(*TexturePath));
		if (Texture) {
			DynamicGroundMaterial->SetTextureParameterValue(TEXT("Texture"), Texture);
			UE_LOG(LogTemp, Warning, TEXT("Texture: %s"), *Texture->GetFName().ToString());
		}
		else {
			DynamicGroundMaterial = UMaterialInstanceDynamic::Create(AOSMLoaderBuildingsActor::LoadMatFromPath(FName("/Game/Materials/Landscape/grass")), NULL);
			DefaultTexture = true;
		}
	
		heightmapImportActor = world->SpawnActor<AHeightmapImportActor>(Location, Rotation, SpawnParams);
		heightmapImportActor->SetMaterial(DynamicGroundMaterial, !DefaultTexture);
	}

	// Spawn new APOI_Actor
	POI_Actor = world->SpawnActor<APOI_Actor>(Location, Rotation, SpawnParams);

	// get the path to our map file
	FString ThePath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::GameContentDir(), TEXT("Data/"), mapName));

	if (!FPaths::FileExists(ThePath)) {
		UE_LOG(LogTemp, Error, TEXT("ERROR: File %s does not exist!"), *ThePath);
		return false;
	}

	UE_LOG(LogTemp, Display, TEXT("Loading file into memory %s..."), *ThePath);

	// read file
	std::ifstream infile;
	infile.open(TCHAR_TO_ANSI(*ThePath), std::ios::binary | std::ios::in);
	infile.seekg(0,std::ios::end);
	int length = infile.tellg();
	infile.seekg(0,std::ios::beg);
	char *data = new char[length];
	infile.read(data, length);
	infile.close();

	UE_LOG(LogTemp, Display, TEXT("Done."));

	// get data structure
	auto entryList = OSMImporter::GetEntryList(data);

	// all geometry entries
	auto entries = entryList->list();

	// array of height values (from .xyz file)
	auto heightMap = entryList->heightMap();

	//UE_LOG(LogTemp, Warning, TEXT("startX %d	startY: %d"), entryList->startX(), entryList->startY());
	//UE_LOG(LogTemp, Warning, TEXT("centerX %d	centerY: %d"), entryList->centerX(), entryList->centerY());
	//for (uint64_t tmp = 0; tmp < heightMap->size(); tmp += 300) {
	//	UE_LOG(LogTemp, Warning, TEXT("X: %.2f, Y: %.2f, Z: %.2f"), heightMap->Get(tmp), heightMap->Get(tmp + 1), heightMap->Get(tmp + 2));
	//}

	heightmapImportActor->SetHeightData(heightMap, FVector2D(entryList->centerX(), entryList->centerY()), FVector2D(entryList->startX(), entryList->startY()), XYZSize);

	// startX and startY are used to calculate the index for a given (x, y) pair into the heightMap
	auto startX = entryList->startX();
	auto startY = entryList->startY();

	// the map center is used to translate each point so the the center
	// of our map is equal to the center of the global coordinate system
	auto centerX = entryList->centerX();
	auto centerY = entryList->centerY();

	// simple index to match roof sections later
	int roofCounter = 0;

	// progress bar update
	FScopedSlowTask parsingBuildingsTask(entries->size() * 2, FText::FromString("Reading geometry..."));
	parsingBuildingsTask.MakeDialog(true);

	UE_LOG(LogTemp, Display, TEXT("Done."));
	UE_LOG(LogTemp, Display, TEXT("Reading geometry..."));

	// all points of the polygon(s)
	TArray<FVector> vertices;

	TArray<FVector2D> uvs;

	// iterate over all geometry
	for (uint32_t i = 0; i < entries->size(); i++) {
		if (parsingBuildingsTask.ShouldCancel()) {
			return false;
		}
		parsingBuildingsTask.EnterProgressFrame();

		UE_LOG(LogTemp, Display, TEXT("Reading geometry [%d/%d]..."), i + 1, entries->size());

		// get current geometry
		auto entry = entries->Get(i);

		// get the type (building, water, street, ...)
		OSMImporter::EntityType type = entry->entityType();

		// get the geometry height (only important for buildings)
		double geometryHeight = (double)entry->height();

		// values for calculating UV mappings
		double firstX = 0.0;
		double firstY = 0.0;
		double firstZ = 0.0;

		double lastX = 0.0;
		double lastY = 0.0;
		double sum = 0.0;

		double alpha = 0.0;

		bool first = true;
		bool second = true;

		// get the raw geometry coordinates
		auto rawVertices = entry->vertices();

		// get vertex indices for triangles
		auto rawTriangles = entry->triangles();

		auto rawUVs = entry->uvs();

		auto name = entry->name()->c_str();

		auto poi = entry->poi();

		auto holeStartID = entry->holeStartId();

		if (rawVertices) {
			// resize arrays to save some allocation cost
			vertices.Empty(rawVertices->size());
			uvs.Empty(rawVertices->size());

			// read in all vertices and calculate UV mappings
			for (uint32_t m = 0; m < rawVertices->size(); m++) {				
				float x = rawVertices->Get(m)->x() - centerX;
				float y = rawVertices->Get(m)->y() - centerY;
				float z = rawVertices->Get(m)->z();

				// we need our coordinates in meters
				vertices.Add(FVector(x * -100.0f, y * 100.0f, z * 100.0f));
			}
		} else {
			// resize arrays to save some allocation cost
			vertices.Empty();
			uvs.Empty();
		}

		if (rawUVs) {
			for (uint32_t m = 0; m < rawUVs->size(); m++) {
				uvs.Add(FVector2D(rawUVs->Get(m)->x(), rawUVs->Get(m)->y()));
			}
		}

		/* TODO this was the way splines were used, maybe this becomes useful again later   ¯\_(ツ)_/¯

		if (type == OSMImporter::EntityType_POLYLINE_HIGHWAY_DEFAULT ||
			type == OSMImporter::EntityType_POLYLINE_HIGHWAY_PRIMARY || type == OSMImporter::EntityType_POLYLINE_HIGHWAY_SECONDARY ||
			type == OSMImporter::EntityType_POLYLINE_HIGHWAY_RESIDENTIAL ||
			type == OSMImporter::EntityType_POLYLINE_HIGHWAY_SERVICE ||
			type == OSMImporter::EntityType_POLYLINE_HIGHWAY_FOOTWAY || type == OSMImporter::EntityType_POLYLINE_HIGHWAY_CYCLEWAY ||
			type == OSMImporter::EntityType_POLYLINE_RIVER || type == OSMImporter::EntityType_POLYLINE_STREAM) {

			// Get world context
			UWorld* World = this->GetWorld();
			if (World) {
				// the actor itself is not translated
				const FVector Location(0.0f, 0.0f, 0.0f);

				// we also don't need any rotation
				FRotator Rotation(0.0f, 0.0f, 0.0f);

				// params can also be empty
				FActorSpawnParameters SpawnParams;

				TSubclassOf<AOSMLoaderSplineActor> actor;

				// highways are stored as polylines so we use splines to handle them
				if (type == OSMImporter::EntityType_POLYLINE_HIGHWAY_DEFAULT) {
					actor = TSubclassOf<AOSMLoaderSplineActor>(highwayDefaultBlueprint);
				}
				else if (type == OSMImporter::EntityType_POLYLINE_HIGHWAY_PRIMARY || type == OSMImporter::EntityType_POLYLINE_HIGHWAY_SECONDARY) {
					actor = TSubclassOf<AOSMLoaderSplineActor>(highwayLargeBlueprint);
				}
				else if (type == OSMImporter::EntityType_POLYLINE_HIGHWAY_RESIDENTIAL) {
					actor = TSubclassOf<AOSMLoaderSplineActor>(highwayMediumBlueprint);
				}
				else if (type == OSMImporter::EntityType_POLYLINE_HIGHWAY_SERVICE) {
					actor = TSubclassOf<AOSMLoaderSplineActor>(highwaySmallBlueprint);
				}
				else if (type == OSMImporter::EntityType_POLYLINE_HIGHWAY_FOOTWAY) {
					actor = TSubclassOf<AOSMLoaderSplineActor>(footwayBlueprint);
				}
				else if (type == OSMImporter::EntityType_POLYLINE_HIGHWAY_CYCLEWAY) {
					actor = TSubclassOf<AOSMLoaderSplineActor>(cyclewayBlueprint);
				}
				else if (type == OSMImporter::EntityType_POLYLINE_RIVER) {
					actor = TSubclassOf<AOSMLoaderSplineActor>(riverBlueprint);
				}
				else if (type == OSMImporter::EntityType_POLYLINE_STREAM) {
					actor = TSubclassOf<AOSMLoaderSplineActor>(streamBlueprint);
				}

				// spawn actor in world
				AOSMLoaderSplineActor* result = World->SpawnActor<AOSMLoaderSplineActor>(actor, Location, Rotation, SpawnParams);

				if (result) {
					// setup acutal spline from points
					result->createSpline(vertices);

					// calculate it
					result->updateSpline();
				}
			}

			continue;
		}
		else*/

		if (poi && POI_Actor != nullptr){
			if (name && strlen(name) > 0) {
				POI_Actor->AddLocation(FString(name), vertices[0]);
			}
		}

		if (type == OSMImporter::EntityType_NODE_TREE) {
			spawnStaticMeshActor(vertices[0], treeMesh);

			continue;
		}
		else if (type == OSMImporter::EntityType_NODE_HIGHWAY_STREET_LAMP) {
			spawnStaticMeshActor(vertices[0], lampMesh);

			continue;
		}
		else if (type == OSMImporter::EntityType_POLYGON_FOREST) {
			for (auto point : vertices) {
				spawnStaticMeshActor(point, treeMesh);
			}

			continue;
		}
		else if (type == OSMImporter::EntityType_POLYGON_GRASS) {
			// https://www.unrealengine.com/marketplace/en-US/product/fe0a7c01da854223bda935f111aab4f4

			for (auto point : vertices) {
				spawnStaticMeshActor(point, vegetationArray[rand() % vegetationArray.Num()]);
			}

			continue;
		}

		// triangulation is performed during preprocessing so we can now directly use the triangle indices from our map file
		TArray<int32> triangles;
		if (rawTriangles) {
			for (uint32_t k = 0; k < rawTriangles->size(); k++) {
				triangles.Add(rawTriangles->Get(k));
			}
		}

		if (vertices.Num() == 0 || triangles.Num() == 0) {
			// we have no geometry to draw (e.g. for POIs), so just skip this step
			continue;
		}

		// until now we have the roof of the building (or a lake e.g.), so let's add it to the mesh

		// we don't need these values (for now)
		TArray<FVector> normals;
		TArray<FLinearColor> vertexColors;
		TArray<FProcMeshTangent> tangents;


		AOSMLoaderProceduralMeshActor* act;

		// Get world context
		UWorld* World = this->GetWorld();

		FVector Location(0.0f, 0.0f, 0.0f);

		// we don't need any rotation
		FRotator Rotation(0.0f, 0.0f, 0.0f);

		// params can also be empty
		FActorSpawnParameters SpawnParams;

		// spawn actor in world
		act = World->SpawnActor<AOSMLoaderProceduralMeshActor>(AOSMLoaderProceduralMeshActor::StaticClass(), Location, Rotation, SpawnParams);

		// create the actual mesh section for the building with the given data
		act->mesh->CreateMeshSection_LinearColor(i, vertices, triangles, normals, uvs, vertexColors, tangents, true);

		UMaterialInterface* meshMaterial = nullptr;

		if (type == OSMImporter::EntityType_POLYGON_BUILDING_ROOF) {
			meshMaterial = roofTextures[rand() % roofTextures.Num()];
		} else if (type == OSMImporter::EntityType_POLYGON_BUILDING_WALL) {
			meshMaterial = wallTextures[rand() % wallTextures.Num()];
		} else if (type == OSMImporter::EntityType_POLYGON_WATER) {
			if (heightmapImportActor != nullptr) {
				if (completeGroundAdjust) {
					heightmapImportActor->AdjustPolygonArea(vertices, triangles, holeStartID);
				}
				else {
					heightmapImportActor->AdjustPolygonAreaSimple(vertices);
				}
			}

			meshMaterial = water;
		} else if (type == OSMImporter::EntityType_POLYLINE_RIVER || type == OSMImporter::EntityType_POLYLINE_STREAM) {
			if (heightmapImportActor != nullptr) {
				if (completeGroundAdjust) {
					// Not working correctly for Polylines
					// heightmapImportActor->AdjustPolygonArea(vertices, triangles);
				}
				else {
					heightmapImportActor->AdjustPolygonAreaSimple(vertices);
				}
			}
			meshMaterial = water;
		} else if (type == OSMImporter::EntityType_POLYGON_HIGHWAY) {
			if (heightmapImportActor != nullptr) {
				if (completeGroundAdjust) {
					heightmapImportActor->AdjustPolygonArea(vertices, triangles, holeStartID);
				}
				else {
					heightmapImportActor->AdjustPolygonAreaSimple(vertices);
				}
			}

			meshMaterial = asphalt;
		} else if (type == OSMImporter::EntityType_POLYLINE_HIGHWAY_PRIMARY || type == OSMImporter::EntityType_POLYLINE_HIGHWAY_SECONDARY) {
			if (heightmapImportActor != nullptr) {
				if (completeGroundAdjust) {
					// Not working correctly for Polylines
					// heightmapImportActor->AdjustPolygonArea(vertices, triangles);
				}
				else {
					heightmapImportActor->AdjustPolygonAreaSimple(vertices);
				}
			}
			meshMaterial = highway_large;
		} else if (type == OSMImporter::EntityType_POLYLINE_HIGHWAY_RESIDENTIAL) {
			if (heightmapImportActor != nullptr) {
				if (completeGroundAdjust) {
					// Not working correctly for Polylines
					// heightmapImportActor->AdjustPolygonArea(vertices, triangles);
				}
				else {
					heightmapImportActor->AdjustPolygonAreaSimple(vertices);
				}
			}
			meshMaterial = highway_medium;
		} else if (type == OSMImporter::EntityType_POLYLINE_HIGHWAY_DEFAULT || type == OSMImporter::EntityType_POLYLINE_HIGHWAY_SERVICE) {
			if (heightmapImportActor != nullptr) {
				if (completeGroundAdjust) {
					// Not working correctly for Polylines
					// heightmapImportActor->AdjustPolygonArea(vertices, triangles);
				}
				else {
					heightmapImportActor->AdjustPolygonAreaSimple(vertices);
				}
			}
			meshMaterial = asphalt;
		} else if (type == OSMImporter::EntityType_POLYLINE_HIGHWAY_FOOTWAY) {
			if (heightmapImportActor != nullptr) {
				if (completeGroundAdjust) {
					// Not working correctly for Polylines
					// heightmapImportActor->AdjustPolygonArea(vertices, triangles);
				}
				else {
					heightmapImportActor->AdjustPolygonAreaSimple(vertices);
				}
			}
			meshMaterial = footway;
		} else if (type == OSMImporter::EntityType_POLYLINE_HIGHWAY_CYCLEWAY) {
			if (heightmapImportActor != nullptr) {
				if (completeGroundAdjust) {
					// Not working correctly for Polylines
					// heightmapImportActor->AdjustPolygonArea(vertices, triangles);
				}
				else {
					heightmapImportActor->AdjustPolygonAreaSimple(vertices);
				}
			}
			meshMaterial = cycleway;
		}  else {
			meshMaterial = test;
		}

		// set the material for that specific mesh section
		act->mesh->SetMaterial(i, meshMaterial);
	}

	if (!heightmapImportActor->CreateMesh(&parsingBuildingsTask, entries->size())) {
		return false;
	
	}

	return true;
}

void AOSMLoaderBuildingsActor::RemoveActor(AActor* foundActor) {
	// see: https://wiki.unrealengine.com/Garbage_Collection_%26_Dynamic_Memory_Allocation#Destroying_Actors
	if (!foundActor) return;
	if (!foundActor->IsValidLowLevel()) return;

	UE_LOG(LogTemp, Display, TEXT("Removing actor: %s"), *(foundActor->GetDebugName(foundActor)));

	foundActor->Destroy();
	foundActor->ConditionalBeginDestroy(); //essential extra step, in all my testing
}

void AOSMLoaderBuildingsActor::ResetData() {
	// get world
	UWorld* world = this->GetWorld();

	TArray<AActor*> foundActors;


	// remove static mesh actors
	UGameplayStatics::GetAllActorsOfClass(world, AOSMLoaderStaticMeshActor::StaticClass(), foundActors);
	for (auto foundActor : foundActors) {
		Cast<AOSMLoaderStaticMeshActor>(foundActor)->GetStaticMeshComponent()->SetStaticMesh(nullptr);
		RemoveActor(foundActor);
	}
	GetWorld()->ForceGarbageCollection();


	// remove procedural mesh actors
	UGameplayStatics::GetAllActorsOfClass(world, AOSMLoaderProceduralMeshActor::StaticClass(), foundActors);
	for (auto foundActor : foundActors) {
		Cast<AOSMLoaderProceduralMeshActor>(foundActor)->mesh->ClearAllMeshSections();
		RemoveActor(foundActor);
	}
	GetWorld()->ForceGarbageCollection();
	

	UE_LOG(LogTemp, Display, TEXT("Removed all actors."));

	
	// delete the heightmap actor
	if (heightmapImportActor != nullptr) {
		//heightmapImportActor->DeleteData();
		world->DestroyActor(heightmapImportActor);
		UE_LOG(LogTemp, Display, TEXT("Removed actor: %s"), *(heightmapImportActor->GetDebugName(heightmapImportActor)));
		heightmapImportActor = nullptr;
	}

	// delete the poi actor
	if (POI_Actor != nullptr) {
		world->DestroyActor(POI_Actor);
		UE_LOG(LogTemp, Display, TEXT("Removed actor: %s"), *(POI_Actor->GetDebugName(POI_Actor)));
		POI_Actor = nullptr;
	}
}

void AOSMLoaderBuildingsActor::LoadData() {
	ResetData();

	UE_LOG(LogTemp, Display, TEXT("Loading map data..."));

	// perform actual loading
	if (!parseBuildings()) {
		ResetData();		
	}

	UE_LOG(LogTemp, Display, TEXT("Done."));
}
