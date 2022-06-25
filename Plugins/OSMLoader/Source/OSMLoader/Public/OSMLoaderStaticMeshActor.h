#pragma once

#include "Engine/StaticMeshActor.h"
#include "OSMLoaderStaticMeshActor.generated.h"


// this class is just needed to determine if a class was spawned by us because we
// don't want to delete all static mesh actors in the scene when resetting the data

UCLASS()
class OSMLOADER_API AOSMLoaderStaticMeshActor : public AStaticMeshActor {
	GENERATED_BODY()
};


