#pragma once

#include "ProceduralMeshComponent.h" 
#include "OSMLoaderProceduralMeshActor.generated.h"


// this class is just needed to determine if a class was spawned by us because we
// don't want to delete all static mesh actors in the scene when resetting the data

UCLASS()
class OSMLOADER_API AOSMLoaderProceduralMeshActor : public AActor {
	GENERATED_BODY()

public:
	AOSMLoaderProceduralMeshActor();

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* mesh;
};


