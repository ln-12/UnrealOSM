#include "OSMLoaderProceduralMeshActor.h"


// Sets default values
AOSMLoaderProceduralMeshActor::AOSMLoaderProceduralMeshActor() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	mesh->bUseAsyncCooking = true;
	mesh->ContainsPhysicsTriMeshData(true);
	mesh->SetMobility(EComponentMobility::Type::Stationary);

	RootComponent = mesh;
}
