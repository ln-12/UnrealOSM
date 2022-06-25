// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProceduralMeshComponent.h" 
#include "earcut.hpp"
#include "PolygonGroundLink.h"
#include "OSMLoaderProceduralMeshActor.h"
#include "Materials/MaterialInstanceDynamic.h" 
#include "Misc/ScopedSlowTask.h"
#include <utility>
#include "schema_generated.h"
#include <vector>
#include "CoreMinimal.h"

// Every node that has a size bigger than MinSize has child nodes
#define MinSize 250

/**
 * Single Mesh section, node of the ground quadtree
 * Only contains points if the node is a leaf
 */
class OSMLOADER_API MeshSection {
private:
	// All child nodes
	TArray<MeshSection> Children;

	// Distance between points in the grid
	int16 PointDistance;

	// Bounding box
	double x, y, MaxX, MaxY;

	// Greatest index of points in x and y
	int size;

	// List of points, just needed for init, will be emptied after init
	TArray<double> list;

	// Mesh variables
	TArray<int32> triangles; 
	TArray<FVector> points;
	TArray<FVector2D> uvs;

	// Is init finished?
	bool Finished = false;

	// Point inside polygon check
	bool isPointInsidePolygon(const TArray<FVector>& Polygon, const FVector Point);

	// Order points clockwise, using angels, polygon has to be convex
	TArray<FVector> OrderPointsClockwise(TArray<FVector> Points);

	// Returns array of point indices arround given index
	TArray<int> GetPointIndicesAroundPoint(int PointIndex);

	// Create child nodes
	void CreateChildren();

	// Actor that contains a procedural mesh
	AOSMLoaderProceduralMeshActor* MeshActor;

	// World pointer
	UWorld* World;

public:
	// Construtors
	MeshSection();
	MeshSection(int sizeIn, double xIn, double yIn, int16 pointDisIn, UWorld* worldIn);
	~MeshSection();

	// Triangulate the section
	void Triangulate();

	// Create the mesh
	bool CreateMesh(double maxX, double maxY, double minX, double minY, UMaterialInstanceDynamic* Material, bool TextureOverCompleteGround, FScopedSlowTask *Task, int ProgressPerLeave);

	// Add single data point
	void addDataPoint(FVector point);

	// Point inside mesh section test
	bool isPointInsideSection(FVector point);

	// Adjust ground of section to polygon
	void MergeMeshWithPolygon(UPolygonGroundLink *Link);

	// Adjust ground of section to polygon in a bit simpler and faster way (without a link, just adjusts ground height at polygon area)
	void MergeMeshWithPolygonSimple(TArray<FVector> Polygon);

	// Finalize init after all points are set
	void FinalizeInit();

	// Delete all data and destroy mesh actor
	void DeleteData();

	// Return number of leaves
	void GetNumberOfLeaves(int* Counter);
};
