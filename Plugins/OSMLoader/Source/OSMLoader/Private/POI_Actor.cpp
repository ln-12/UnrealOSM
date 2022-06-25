// Fill out your copyright notice in the Description page of Project Settings.


#include "POI_Actor.h"

// Sets default values
APOI_Actor::APOI_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APOI_Actor::BeginPlay()
{
	Super::BeginPlay();
}

void APOI_Actor::AddLocation(FString InName, FVector InLocation) {
	Locations.Add(InName, InLocation);
}

TMap<FString, FVector> APOI_Actor::GetLocations() {
	return Locations;
}

