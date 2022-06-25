// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "POI_Actor.generated.h"

UCLASS()
class OSMLOADER_API APOI_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APOI_Actor();

	// Stores all locations and theire names
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "POI")
		TMap<FString, FVector> Locations;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Add a location to the actor
	void AddLocation(FString InName, FVector InLocation);

	// Returns all Locations
	UFUNCTION(BlueprintPure, Category = "POI")
		TMap<FString, FVector> GetLocations();
};
