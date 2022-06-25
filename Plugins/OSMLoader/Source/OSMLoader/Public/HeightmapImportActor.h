// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XYZReader.h"
#include "schema_generated.h"
#include <vector>
#include "Misc/ScopedSlowTask.h"
#include "OSMLoaderCoordinates.h"
#include "Factories/MaterialFactoryNew.h"
#include "Materials/MaterialInstanceDynamic.h" 
#include "ProceduralMeshComponent.h" 
#include "MeshSection.h" 
#include "PolygonGroundLink.h"
#include "HeightmapImportActor.generated.h"

UCLASS()
class OSMLOADER_API AHeightmapImportActor : public AActor
{
	GENERATED_BODY()

public:
	// Default constructor
	AHeightmapImportActor();

	// Adjust the ground to a polygon
	void AdjustPolygonArea(TArray<FVector> polygon, TArray<int> Triangles, int HoleStartId);

	// Adjust ground to polygon in a bit simpler and faster way (without a link, just adjusts ground height at polygon area)
	void AdjustPolygonAreaSimple(TArray<FVector> polygon);

	// Set the height data
	void SetHeightData(const flatbuffers::Vector<float> *InData, FVector2D InMapCenter, FVector2D InStartCoords, int InSize);

	// Set the material of the ground
	void SetMaterial(UMaterialInstanceDynamic* InMaterial, bool InTextureOverCompleteGround);

	// Create the meshes of the ground
	bool CreateMesh(FScopedSlowTask *Task, int PercentOfTasksToComplete);

	// Delete all data and destroy the meshes
	void DeleteData();

private:
	// Root node of the quad tree
	MeshSection Root;

	// Ground material
	UMaterialInstanceDynamic* Material;
	bool TextureOverCompleteGround = false;

	// All links for polygons to the ground
	TArray<UPolygonGroundLink> Links;

	// Bounding box
	double MaxX = DBL_MIN;
	double MinX = DBL_MAX;
	double MaxY = DBL_MIN;
	double MinY = DBL_MAX;

	// Center of the map
	FVector2D MapCenter;

	// Starting coordinates of the map
	FVector2D StartCoords;

	// Size of the map
	int Size;

	// Distance between points in the grid
	int DistanceBetweenPoints = 100;

	// Point inside polygon check
	bool isPointInsidePolygon(const TArray<FVector>& Polygon, const FVector Point);
};
