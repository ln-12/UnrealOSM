// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include <Engine/Engine.h>
#include "Containers/Array.h"
#include <string>
#include "XYZReader.generated.h"

/**
 * Reader for XYZ Files
 * Not needed anymore
 */
UCLASS()
class OSMLOADER_API UXYZReader : public UObject
{
	GENERATED_BODY()
private:
	TMap<int64, double> data;

public:
	UXYZReader();
	UXYZReader(FString FileName);
	~UXYZReader();

	void readFile(FString FileName);
	TMap<int64, double> getData();
	TArray<double> getZValues();

	void static readFile(FString FileName, TMap<int64, double>* outData);
};