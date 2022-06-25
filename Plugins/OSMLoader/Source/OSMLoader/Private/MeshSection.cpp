// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshSection.h"

// Default constructor
MeshSection::MeshSection(){
	// Set default values
	size = 64;
	x = 0;
	y = 0;
	PointDistance = 100;

	// Calculate bounding box
	MaxX = x + size * PointDistance;
	MaxY = y + size * PointDistance;

	if (size > MinSize) {
		// If the size of the node is greater than MinSize -> create child nodes
		CreateChildren();
	}
	else {
		// Only init list if the section is a leaf node
		list.Init(0.0, (size + 1) * (size + 1));
	}
}

// Constructor
MeshSection::MeshSection(int sizeIn, double xIn, double yIn, int16 pointDisIn, UWorld* worldIn){
	// Set given values
	size = sizeIn;
	PointDistance = pointDisIn;
	x = (int)(xIn);
	y = (int)(yIn);
	World = worldIn;

	// Calculate bounding box
	MaxX = x + size * PointDistance;
	MaxY = y + size * PointDistance;

	if (size > MinSize) {
		// If the size of the node is greater than MinSize -> create child nodes
		CreateChildren();
	}
	else {
		// Only init list if the section is a leaf node
		list.Init(0.0, (size + 1) * (size + 1));
	}
}

MeshSection::~MeshSection(){  }

// Creates 4 child nodes
/*
---------
| 0 | 1 |
|---|---|
| 2 | 3 |
---------
*/
void MeshSection::CreateChildren() {
	int HalfSize = size / 2;

	Children.Add(MeshSection(HalfSize, x, y, PointDistance, World));
	Children.Add(MeshSection(HalfSize, x + HalfSize * PointDistance, y, PointDistance, World));
	Children.Add(MeshSection(HalfSize, x, y + HalfSize * PointDistance, PointDistance, World));
	Children.Add(MeshSection(HalfSize, x + HalfSize * PointDistance, y + HalfSize * PointDistance, PointDistance, World));
}

// Create mesh actor and mesh
bool MeshSection::CreateMesh(double maxX, double maxY, double minX, double minY, UMaterialInstanceDynamic* Material, bool TextureOverCompleteGround, FScopedSlowTask *Task, int ProgressPerLeave) {
	// Not working correctly, because CreateMesh is recursive and only can cancle single creations 
	//if (Task->ShouldCancel()) {
	//	return false;
	//}

	// Only the leafs have to create meshes
	if (Children.Num() > 0) {
		for (auto& Child : Children) {
			Child.CreateMesh(maxX, maxY, minX, minY, Material, TextureOverCompleteGround, Task, ProgressPerLeave);
		}
	}
	else {
		// Print progess
		for (int i = 0; i < ProgressPerLeave; i++) {
			Task->EnterProgressFrame();
		}

		// Create remaining mesh variables, we don't need these values (for now)
		TArray<FVector> normals;
		//normals.Init(FVector(0.0f, 0.0f, 1.0f), points.Num());

		TArray<FLinearColor> vertexColors;
		//vertexColors.Init(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), 4);

		TArray<FProcMeshTangent> tangents;
		//tangents.Init(FProcMeshTangent(1.0f, 0.0f, 0.0f), 4);

		// Fill the UVS mesh list, so the texture will be displayed correctly
		double DiffX = maxX - minX;
		double DiffY = maxY - minY;

		uvs.Empty();
		if (TextureOverCompleteGround) {
			for (int n = 0; n < points.Num(); n++) {
				uvs.Add(FVector2D((minX - points[n].X) / DiffX, (minY - points[n].Y) / DiffY));
			}
		}
		else {
			for (int n = 0; n < points.Num(); n++) {
				uvs.Add(FVector2D((x - points[n].X) / (size), (y - points[n].Y) / (size)));
			}
		}

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

	return true;
}

// Finishes init
void MeshSection::FinalizeInit() {
	// Triangulate all given points
	Triangulate();

	// Remove all items in list, we don't need them anymore, all points are stored in the points array
	list.Empty();

	// Finilize all children
	for (auto& Child : Children) {
		Child.FinalizeInit();
	}

	Finished = true;
}

// Deletes all data and destroys the mesh and the mesh actor
void MeshSection::DeleteData() {
	// Delete the data of all child nodes
	for (auto& Child : Children) {
		Child.DeleteData();
	}

	// Empty all arrays
	list.Empty();
	points.Empty();
	triangles.Empty();
	uvs.Empty();

	// Destroy the mesh and the mesh actor
	if (MeshActor) {
		MeshActor->mesh->ClearAllMeshSections();
		World->DestroyActor(MeshActor);
	}
}

// Adds a single data point
void MeshSection::addDataPoint(FVector point) {
	// Only add the point if the section is a leaf node
	if (Children.Num() > 0) {
		// Try to add the data point to all child nodes
		// They will only add the point if it is inside the node
		for (auto& Child : Children) {
			Child.addDataPoint(point);
		}
	}
	else {
		// Add the point to the list of points if it is inside the section and the init is not finished
		if (isPointInsideSection(point) && !Finished) {
			list[((point.X - x) + (size + 1) * (point.Y - y)) / PointDistance] = point.Z;
		}
	}
}

// Checks if a point is inside the current section
bool MeshSection::isPointInsideSection(FVector point) {
	if (point.X > MaxX) {
		return false;
	}
	else if (point.Y > MaxY) {
		return false;
	}
	else if (point.X < x) {
		return false;
	}
	else if (point.Y < y) {
		return false;
	}
	else {
		return true;
	}
}

// Calculates all triangles which are needed to draw the section

// TODO: fehlende Datenpunkte ausgleichen

void MeshSection::Triangulate() {
	// Only leafs have to calculate the triangles
	if (Children.Num() > 0) {
		return;
	}

	// Fill the points array using the list array
	if (list.Num() == 0) {
		for (int i = 0; i < size + 1; i++) {
			for (int j = 0; j < size + 1; j++) {
				double pointX = x + i * PointDistance;
				double pointY = y + j * PointDistance;
				points.Add(FVector(pointX, pointY, 0.0));
			}
		}
	}
	else {
		for (int i = 0; i < size + 1; i++) {
			for (int j = 0; j < size + 1; j++) {
				double pointX = x + i * PointDistance;
				double pointY = y + j * PointDistance;
				points.Add(FVector(pointX, pointY, list[i + j * (size + 1)]));
			}
		}
	}

	// Fill the triangles array
	// Iterate over all points except the last row and column
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			// For each point add two triangles
			/*
			P0--P1--P2
			| \ | \ |
			PX--PY--PZ
			*/
			triangles.Add(i + j * (size + 1));
			triangles.Add(i + j * (size + 1) + size + 2);
			triangles.Add(i + j * (size + 1) + size + 1);

			triangles.Add(i + j * (size + 1));
			triangles.Add(i + j * (size + 1) + 1);
			triangles.Add(i + j * (size + 1) + size + 2);
		}
	}

	// Checks if a single value differs ffrom its neighbors and adjusts the value
	// Border only valid for landscapes outside mountains
	float Border = 1000.0f;
	int Counter = 0;

	for (auto& Point : points) {
		bool DifferentToAll = true;
		float Median = 0;
		auto Indices = GetPointIndicesAroundPoint(Counter);

		for (auto& Index : Indices) {
			if (abs(points[Index].Z - Point.Z) < Border) {
				DifferentToAll = false;
				break;
			}
			Median += points[Index].Z;
		}

		if (DifferentToAll) {
			Median /= (float)Indices.Num();
			Point.Z = Median;
		}

		Counter++;
	}
}

// Sorts the given points clockwise, using angle calculations
// Only possible if the outer polygon is convex
TArray<FVector> MeshSection::OrderPointsClockwise(TArray<FVector> Points) {
	double SumX = 0;
	double SumY = 0;
	
	for (int i = 0; i < Points.Num(); i++) {
		SumX += Points[i].X;
		SumY += Points[i].Y;
	}

	FVector2D MidPoint(SumX / Points.Num(), SumY / Points.Num());

	TMap<FVector, double> PointsToAngles;

	for (int i = 0; i < Points.Num(); i++) {
		FVector2D A(-1.0, 0.0);
		FVector2D B(Points[i].X - MidPoint.X, Points[i].Y - MidPoint.Y);

		double angle = acos((A.X * B.X + A.Y * B.Y) / sqrt(B.X * B.X + B.Y * B.Y));
		if (B.Y < 0.0) {
			angle = 2.0 * PI - angle;
		}

		PointsToAngles.Add(Points[i], angle);
	}

	PointsToAngles.ValueSort([](const double& A, const double& B) { return A < B; });

	TArray<FVector> Result;

	for (auto& Item : PointsToAngles) {
		Result.Add(Item.Key);
	}

	return Result;
}

// Returns point indices arround the given index
TArray<int> MeshSection::GetPointIndicesAroundPoint(int PointIndex) {
	TArray<int> PointIndices;

	// If the point is at the right side of the mesh -> don't return points of the left side
	if (PointIndex % (size + 1) == 0) {
		int index = PointIndex - size - 1;
		for (int i = 1; i <= 6; i++) {
			if (index >= 0 && index < points.Num()) {
				PointIndices.Add(index);
			}
			if (i % 2 == 0) {
				index += size - 1;
			}
			index++;
		}
	} // Same for left side
	else if (PointIndex % (size + 1) == size) {
		int index = PointIndex - size - 2;
		for (int i = 1; i <= 6; i++) {
			if (index >= 0 && index < points.Num()) {
				PointIndices.Add(index);
			}
			if (i % 2 == 0) {
				index += size - 1;
			}
			index++;
		}
	} // Not on right or left side -> return all indices arround
	else {
		int index = PointIndex - size - 2;
		for (int i = 1; i <= 9; i++) {
			if (index >= 0 && index < points.Num()) {
				PointIndices.Add(index);
			}
			if (i % 3 == 0) {
				index += size - 2;
			}
			index++;
		}
	}

	// Remove the index of the given point and all indices greater max size and smaller 0
	TArray<int> Remove;
	Remove.Add(PointIndex);
	for (auto& Index : PointIndices) {
		if (Index < 0 || Index >= points.Num()) {
			Remove.Add(Index);
		}
	}
	Remove.Sort();
	for (int i = 0; i < Remove.Num(); i++) {
		PointIndices.Remove(Remove[i] - i);
	}
	
	return PointIndices;
}
 
// Adjust ground of section to a polygon, Link contains the polygon
void MeshSection::MergeMeshWithPolygon(UPolygonGroundLink* Link) {
	// Get the polygon
	TArray<FVector> Polygon = Link->GetInnerPolygon();

	// If the polygon is not inside the section return, nothing to do here
	int counter = 0;
	for (auto& Point : Polygon) {
		if (isPointInsideSection(Point)) {
			counter++;
		}
	}
	if (counter == 0) {
		return;
	}

	// If the section has child nodes -> nothing to do here, the child nodes have to adjust the ground
	if (Children.Num() > 0) {
		for (auto& Child : Children) {
			Child.MergeMeshWithPolygon(Link);
		}
		return;
	}

	// Get minimun height of the polygon

	double MinHeightPolygon = DBL_MAX;
	for (auto& Point : Polygon) {
		if (Point.Z < MinHeightPolygon) {
			MinHeightPolygon = Point.Z;
		}
	}

	for (int i = 0; i < points.Num(); i++) {
		if (isPointInsidePolygon(Polygon, points[i])) {

			// Apply min height to all points inside the polygon
			points[i].Z = MinHeightPolygon - 1.0;

			// For each point inside the polygon add all neighbour points that are not inside the polygon to the Link object
			for (auto& Index : GetPointIndicesAroundPoint(i)) {
				if (!isPointInsidePolygon(Polygon, points[Index])) {
					Link->AddOuterPoint(points[Index]);
				}
			}
		}
	}
}

// Adjust ground of section to polygon in a bit simpler and faster way (without a link, just adjusts ground height at polygon area)
void MeshSection::MergeMeshWithPolygonSimple(TArray<FVector> Polygon) {
	// If the polygon is not inside the section return, nothing to do here
	int counter = 0;
	for (auto& Point : Polygon) {
		if (isPointInsideSection(Point)) {
			counter++;
		}
	}
	if (counter == 0) {
		return;
	}

	// If the section has child nodes -> nothing to do here, the child nodes have to adjust the ground
	if (Children.Num() > 0) {
		for (auto& Child : Children) {
			Child.MergeMeshWithPolygonSimple(Polygon);
		}
		return;
	};

	// Calculate the min height of the polygon
	double MinHeightPolygon = DBL_MAX;
	for (auto& Point : Polygon) {
		if (Point.Z < MinHeightPolygon) {
			MinHeightPolygon = Point.Z;
		}
	}

	// Set the height of all points inside the polygon to the min height of the polygon
	for (int i = 0; i < points.Num(); i++) {
		if (isPointInsidePolygon(Polygon, points[i])) {
			points[i].Z = MinHeightPolygon - 1.0;
			//points[i].Z -= 10.0;
		}
	}
}

// Point inside polygon check
bool MeshSection::isPointInsidePolygon(const TArray<FVector>& Polygon, const FVector Point) {
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

void MeshSection::GetNumberOfLeaves(int* Counter) {
	if (Children.Num() > 0) {
		for (auto& Child : Children) {
			Child.GetNumberOfLeaves(Counter);
		}
	}
	else {
		*Counter += 1;
	}
}