// Fill out your copyright notice in the Description page of Project Settings.


#include "HeightmapImportActor.h"

// Sets default values
AHeightmapImportActor::AHeightmapImportActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set default values
	MapCenter = FVector2D(383028.0, 5642813.0);
	StartCoords = FVector2D(382000.0, 5642000.0);
	Size = 2000;
}

// Sets the height data
void AHeightmapImportActor::SetHeightData(const flatbuffers::Vector<float> *InData, FVector2D InMapCenter, FVector2D InStartCoords, int InSize) {
	// Store given variables
	MapCenter = InMapCenter;
	StartCoords = InStartCoords;
	Size = InSize;

	// Calculate the bounding box
	MinX = -100.0 * (StartCoords.X - MapCenter.X + Size);
	MinY = 100.0 * (StartCoords.Y - MapCenter.Y);
	MaxX = MinX + Size * DistanceBetweenPoints;
	MaxY = MinY + Size * DistanceBetweenPoints;

	// Create a root node (the root node will create the complete quadtree)
	Root = MeshSection(Size, MinX, MinY, DistanceBetweenPoints, GetWorld());

	// If the given size doen't match the given data size -> something is wrong -> return
	if (InData->size() != Size * Size) {
		UE_LOG(LogTemp, Display, TEXT("HeightMap value count not matching size!"));
		return;
	}

	// Restore all 3d point coordiantes and add them to the root node (the root node will add the points only to the correct leaf nodes)
	for (int Row = 0; Row < Size; Row++) {
		for (int Col = 0; Col < Size; Col++) {
			double X = MinX + (Size - Col - 1) * DistanceBetweenPoints;
			double Y = MinY + Row * DistanceBetweenPoints;
			double Z = InData->Get(Row * Size + Col) * 100.0;

			Root.addDataPoint(FVector(X, Y, Z));
		}
	}
	
	// Finalize all nodes
	Root.FinalizeInit();
}

// Creates the meshes of the ground
bool AHeightmapImportActor::CreateMesh(FScopedSlowTask *Task, int NumberOfTasksToComplete) {
	int counter = 0;
	Root.GetNumberOfLeaves(&counter);
	int ProgressPerLeave = NumberOfTasksToComplete / counter;

	return Root.CreateMesh(MaxX, MaxY, MinX, MinY, Material, TextureOverCompleteGround, Task, ProgressPerLeave);
}

// Deletes all data and destroy the meshes
void AHeightmapImportActor::DeleteData() {
	Root.DeleteData();
}

// Sets the material of the ground
void AHeightmapImportActor::SetMaterial(UMaterialInstanceDynamic* InMaterial, bool InTextureOverCompleteGround) {
	Material = InMaterial;
	TextureOverCompleteGround = InTextureOverCompleteGround;
}

// Point inside polygon check
bool AHeightmapImportActor::isPointInsidePolygon(const TArray<FVector>& Polygon, const FVector Point) {
	const int NumCorners = Polygon.Num();
	int PreviousCornerIndex = NumCorners - 1;
	bool bIsInside = false;

	for (int CornerIndex = 0; CornerIndex < NumCorners; CornerIndex++)
	{
		if (((Polygon[CornerIndex].Y < Point.Y && Polygon[PreviousCornerIndex].Y >= Point.Y) || (Polygon[PreviousCornerIndex].Y < Point.Y && Polygon[CornerIndex].Y >= Point.Y)) &&
			(Polygon[CornerIndex].X <= Point.X || Polygon[PreviousCornerIndex].X <= Point.X))
		{
			bIsInside ^= (Polygon[CornerIndex].X + (Point.Y - Polygon[CornerIndex].Y) / (Polygon[PreviousCornerIndex].Y - Polygon[CornerIndex].Y) * (Polygon[PreviousCornerIndex].X - Polygon[CornerIndex].X) < Point.X);
		}

		PreviousCornerIndex = CornerIndex;
	}

	return bIsInside;
}

// Adjusts the ground to a polygon
void AHeightmapImportActor::AdjustPolygonArea(TArray<FVector> polygon, TArray<int> Triangles, int HoleStartId) {
	// For now just use the outer polygon without holes
	if (HoleStartId != -1) {
		polygon.RemoveAt(HoleStartId, polygon.Num() - HoleStartId);
	}

	// Create a new link and add the polygon
	UPolygonGroundLink *Link = NewObject<UPolygonGroundLink>();
	Link->SetInnerPolygon(polygon, Triangles);

	// Call the merge methode of the root node (will call all its childs and so on to the leafs)
	Root.MergeMeshWithPolygon(Link); 

	// Create the link mesh
	Link->CreateMesh(MaxX, MaxY, MinX, MinY, Material, TextureOverCompleteGround, GetWorld());
}

// Adjusts ground to polygon in a bit simpler and faster way (without a link, just adjusts ground height at polygon area)
void AHeightmapImportActor::AdjustPolygonAreaSimple(TArray<FVector> polygon) {
	// Call the simple merge methode of the root node (will call all its childs and so on to the leafs)
	Root.MergeMeshWithPolygonSimple(polygon);
}