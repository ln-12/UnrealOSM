// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OSMLoaderCoordinates.generated.h"


UCLASS()
class OSMLOADER_API UCoordinates: public UObject
{
	GENERATED_BODY()

public:
	UCoordinates();
	~UCoordinates();

	double x;
	double y;
	double z;
};
