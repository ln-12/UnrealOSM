// Fill out your copyright notice in the Description page of Project Settings.


#include "OSMLoaderSplineActor.h"

// Sets default values
AOSMLoaderSplineActor::AOSMLoaderSplineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	customSpline = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
}

// Called when the game starts or when spawned
void AOSMLoaderSplineActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AOSMLoaderSplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// TODO how about this?
// https://forums.unrealengine.com/community/community-content-tools-and-tutorials/107198-free-simple-road-tool
void AOSMLoaderSplineActor::createSpline(TArray<FVector>& points) {
	customSpline->ClearSplinePoints(false);

	for (int i = 0; i < points.Num(); i++) {
		auto& point = points[i];
		customSpline->AddSplinePoint(point, ESplineCoordinateSpace::Type::World, false);
		customSpline->SetSplinePointType(i, ESplinePointType::Linear, false);
	}

	customSpline->UpdateSpline();
}
