// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "earcut.hpp"
#include <vector>
#include "OSMLoaderProceduralMeshActor.h"
#include "PolygonGroundLink.generated.h"

/**
 * Object that connects a OSM polygon with the ground mesh
 */

UCLASS()
class OSMLOADER_API UPolygonGroundLink : public UObject
{
	GENERATED_BODY()
	
public:
	// Add the OSM polygon
	void SetInnerPolygon(TArray<FVector> Polygon, TArray<int> Triangles);
	
	// Get OSM polygon
	TArray<FVector> GetInnerPolygon();

	// Add point to Polygonring around the OSM polygon
	void AddOuterPoint(FVector Point);

	// Checks if a Point is already added to the OuterPolygon
	bool IsOuterPointAlreadyAdded(FVector Point);

	// Creates the link mesh
	void CreateMesh(double maxX, double maxY, double minX, double minY, UMaterialInterface* Material, bool TextureOverCompleteGround, UWorld* World);

private:
	// Order points of the outer/inner polygon clockwise
	void OrderOuterPointsClockwise();
	void OrderInnerPointsClockwise();

	// Stores the outer/inner polygon
	TArray<FVector> InnerPolygon;
	TArray<FVector> OuterPolygon;

	// Triangles of the OSM polygon, needed to sort the points
	TArray<int> InnerPolygonTriangles;

	// Actor that contains a procedural mesh
	AOSMLoaderProceduralMeshActor* MeshActor;
};
