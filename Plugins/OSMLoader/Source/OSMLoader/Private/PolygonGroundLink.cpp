// Fill out your copyright notice in the Description page of Project Settings.


#include "PolygonGroundLink.h"

void UPolygonGroundLink::SetInnerPolygon(TArray<FVector> Polygon, TArray<int> Triangles) {
	InnerPolygon = Polygon;
	InnerPolygonTriangles = Triangles;
	OrderInnerPointsClockwise();
}

TArray<FVector> UPolygonGroundLink::GetInnerPolygon() {
	return InnerPolygon;
}

void UPolygonGroundLink::AddOuterPoint(FVector Point) {
	OuterPolygon.Add(Point);
}

bool UPolygonGroundLink::IsOuterPointAlreadyAdded(FVector Point) {
	return OuterPolygon.Contains(Point);
}

void UPolygonGroundLink::CreateMesh(double maxX, double maxY, double minX, double minY, UMaterialInterface* Material, bool TextureOverCompleteGround, UWorld *World) {
	// Doesn't work if the outer polygon is not convex
	OrderOuterPointsClockwise();

	if (OuterPolygon.Num() == 0 || InnerPolygon.Num() == 0) {
		// Error -> cannot create mesh for this Link
		return;
	}

	// Mesh variables
	TArray<int32> triangles;
	TArray<FVector> points;
	TArray<FVector2D> uvs;
	
	// Create variables to use earcut correctly
	std::vector<std::vector<std::pair<double, double>>> AllPoints;
	std::vector<std::pair<double, double>> FirstRing;
	for (auto& Point : OuterPolygon) {
		FirstRing.push_back(std::pair<double, double>(Point.X, Point.Y));
	}
	AllPoints.push_back(FirstRing);

	std::vector<std::pair<double, double>> SecondRing;
	for (auto& Point : InnerPolygon) {
		SecondRing.push_back(std::pair<double, double>(Point.X, Point.Y));
	}
	AllPoints.push_back(SecondRing);

	// Earcut
	using N = uint32_t;
	std::vector<N> Indices = mapbox::earcut<N>(AllPoints);

	// Add all triangle indices to dhe mesh variable
	// invert the direction of rotation, so all triangles are visible from the top
	for (int i = 0; i < Indices.size(); i += 3) {
		triangles.Add(Indices[i]);
		triangles.Add(Indices[i + 2]);
		triangles.Add(Indices[i + 1]);
	}

	// Add all Points in the right order to the mesh variable
	for (auto& Point : OuterPolygon) {
		points.Add(Point);
	}
	for (auto& Point : InnerPolygon) {
		points.Add(Point);
	}

	// Fill the UVS mesh list, so the texture will be displayed correctly
	double DiffX = maxX - minX;
	double DiffY = maxY - minY;
	if (TextureOverCompleteGround) {
		uvs.Empty();
		for (int n = 0; n < points.Num(); n++) {
			uvs.Add(FVector2D((minX - points[n].X) / DiffX, (minY - points[n].Y) / DiffY));
		}
	}

	// Mesh variables
	// We don't need these values (for now)
	TArray<FVector> normals;
	//normals.Init(FVector(0.0f, 0.0f, 1.0f), points.Num());

	TArray<FLinearColor> vertexColors;
	//vertexColors.Init(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), 4);

	TArray<FProcMeshTangent> tangents;
	//tangents.Init(FProcMeshTangent(1.0f, 0.0f, 0.0f), 4);

	// Spawn mesh actor
	const FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnParams;
	MeshActor = World->SpawnActor<AOSMLoaderProceduralMeshActor>(AOSMLoaderProceduralMeshActor::StaticClass(), Location, Rotation, SpawnParams);

	// Create Mesh, set material and set some settings
	if (MeshActor) {
		MeshActor->mesh->SetMobility(EComponentMobility::Type::Static);
		MeshActor->mesh->CreateMeshSection_LinearColor(0, points, triangles, normals, uvs, vertexColors, tangents, true);
		MeshActor->mesh->SetMaterial(0, Material);
		MeshActor->mesh->ContainsPhysicsTriMeshData(true);
	}
}

// Sorts the outer points clockwise, using angle calculations
// Only possible if the outer polygon is convex
void UPolygonGroundLink::OrderOuterPointsClockwise() {
	double SumX = 0;
	double SumY = 0;

	for (int i = 0; i < OuterPolygon.Num(); i++) {
		SumX += OuterPolygon[i].X;
		SumY += OuterPolygon[i].Y;
	}

	FVector2D MidPoint(SumX / OuterPolygon.Num(), SumY / OuterPolygon.Num());

	TMap<FVector, double> PointsToAngles;

	for (int i = 0; i < OuterPolygon.Num(); i++) {
		FVector2D A(-1.0, 0.0);
		FVector2D B(OuterPolygon[i].X - MidPoint.X, OuterPolygon[i].Y - MidPoint.Y);

		double angle = acos((A.X * B.X + A.Y * B.Y) / sqrt(B.X * B.X + B.Y * B.Y));
		if (B.Y < 0.0) {
			angle = 2.0 * PI - angle;
		}

		PointsToAngles.Add(OuterPolygon[i], angle);
	}

	PointsToAngles.ValueSort([](const double& A, const double& B) { return A < B; });

	OuterPolygon.Empty();

	for (auto& Item : PointsToAngles) {
		OuterPolygon.Add(Item.Key);
	}
}

// Sorts the outer points clockwise, using angle calculations
// Also works for non-convex polygons because we got all triangles
void UPolygonGroundLink::OrderInnerPointsClockwise() {
	TArray<TPair<int, int>> SingleTriangleEdges;

	// Identify all triangle egdes that are unique and store them in SingleTriangleEdges -> outer edges of the polygon
	for (int i = 0; i < InnerPolygonTriangles.Num(); i += 3) {
		TPair<int, int> Edge1 = TPair<int, int>(InnerPolygonTriangles[i], InnerPolygonTriangles[i + 1]);
		TPair<int, int> Edge1I = TPair<int, int>(InnerPolygonTriangles[i + 1], InnerPolygonTriangles[i]);
		TPair<int, int> Edge2 = TPair<int, int>(InnerPolygonTriangles[i], InnerPolygonTriangles[i + 2]);
		TPair<int, int> Edge2I = TPair<int, int>(InnerPolygonTriangles[i + 2], InnerPolygonTriangles[i]);
		TPair<int, int> Edge3 = TPair<int, int>(InnerPolygonTriangles[i + 2], InnerPolygonTriangles[i + 1]);
		TPair<int, int> Edge3I = TPair<int, int>(InnerPolygonTriangles[i + 1], InnerPolygonTriangles[i + 2]);

		if (!SingleTriangleEdges.Contains(Edge1) && !SingleTriangleEdges.Contains(Edge1I)) {
			SingleTriangleEdges.Add(Edge1);
		}
		else {
			SingleTriangleEdges.Remove(Edge1);
			SingleTriangleEdges.Remove(Edge1I);
		}

		if (!SingleTriangleEdges.Contains(Edge2) && !SingleTriangleEdges.Contains(Edge2I)) {
			SingleTriangleEdges.Add(Edge2);
		}
		else {
			SingleTriangleEdges.Remove(Edge2);
			SingleTriangleEdges.Remove(Edge2I);
		}

		if (!SingleTriangleEdges.Contains(Edge3) && !SingleTriangleEdges.Contains(Edge3I)) {
			SingleTriangleEdges.Add(Edge3);
		}
		else {
			SingleTriangleEdges.Remove(Edge3);
			SingleTriangleEdges.Remove(Edge3I);
		}
	}

	if (SingleTriangleEdges.Num() < 1) {
		return;
	}

	// Create a sorted array and add a starting point
	TArray<int> SortedPoints;
	SortedPoints.Add(SingleTriangleEdges[0].Key);
	SortedPoints.Add(SingleTriangleEdges[0].Value);
	SingleTriangleEdges.RemoveAt(0);

	// All points have two egdes in the SingleTriangleEdges array -> if the first one is added, add the second one
	// Add all points sorted to the array
	while (SingleTriangleEdges.Num() > 0) {
		bool FoundNextEdge = false;
		for (int i = 0; i < SingleTriangleEdges.Num(); i++) {
			if (SingleTriangleEdges[i].Key == SortedPoints[SortedPoints.Num() - 1]) {
				SortedPoints.Add(SingleTriangleEdges[i].Value);
				SingleTriangleEdges.RemoveAt(i);
				FoundNextEdge = true;
				break;
			}
			else if (SingleTriangleEdges[i].Value == SortedPoints[SortedPoints.Num() - 1]) {
				SortedPoints.Add(SingleTriangleEdges[i].Key);
				SingleTriangleEdges.RemoveAt(i);
				FoundNextEdge = true;
				break;
			}
		}

		// If the next edge is not found and an error has occurred
		if (!FoundNextEdge) {
			return;
		}
	}

	TArray<FVector> Copy;

	for (auto& Index : SortedPoints) {
		Copy.Add(InnerPolygon[Index]);
	}

	// Set the sorted points to the real array
	InnerPolygon = Copy;
}