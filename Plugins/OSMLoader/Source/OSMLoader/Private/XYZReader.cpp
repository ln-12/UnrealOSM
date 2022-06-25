// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZReader.h"

UXYZReader::UXYZReader() {  }

UXYZReader::~UXYZReader() {  }

UXYZReader::UXYZReader(FString FileName) {
	readFile(FileName);
}

void UXYZReader::readFile(FString FileName) {
	//UE_LOG(LogTemp, Error, TEXT("Can not read file %s."), *FileName);

	if (!FPaths::FileExists(FileName)) {
		return;
	}
	TArray<FString> stringArray;
	if (FFileHelper::LoadFileToStringArray(stringArray, *FileName)) {
		UE_LOG(LogTemp, Display, TEXT("Read from file %s: %d lines."), *FileName, stringArray.Num());
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Can not read file %s."), *FileName);
		return;
	}

	data.Empty();

	for (int i = 0; i < stringArray.Num(); i++) {
		FString left, right;

		right = stringArray[i];

		right.Split(FString(TEXT(" ")), &left, &right);
		int32 x = (FCString::Atoi(*left));

		right.Split(FString(TEXT(" ")), &left, &right);
		int32 y = (FCString::Atoi(*left));

		double z = (FCString::Atod(*right));

		int64 index = x;
		index = index << 32;
		index = index | y;

		data.Add(index, z);
	}
}

TMap<int64, double> UXYZReader::getData() {
	return data;
}

TArray<double> UXYZReader::getZValues() {
	TArray<double> result;

	for (auto& value : data) {
		result.Add(value.Value);
	}

	return result;
}

void UXYZReader::readFile(FString FileName, TMap<int64, double>* outData) {
	if (!FPaths::FileExists(FileName)) {
		return;
	}
	TArray<FString> stringArray;
	if (FFileHelper::LoadFileToStringArray(stringArray, *FileName)) {
		UE_LOG(LogTemp, Display, TEXT("Read from file %s: %d lines."), *FileName, stringArray.Num());
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Can not read file %s."), *FileName);
		return;
	}

	outData->Empty();

	for (int i = 0; i < stringArray.Num(); i++) {
		FString left, right;

		right = stringArray[i];

		right.Split(FString(TEXT(" ")), &left, &right);
		int32 x = (FCString::Atoi(*left));

		right.Split(FString(TEXT(" ")), &left, &right);
		int32 y = (FCString::Atoi(*left));

		double z = (FCString::Atod(*right));

		int64 index = x;
		index = index << 32;
		index = index | y;

		outData->Add(index, z);
	}
}