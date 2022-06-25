// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Engine/GameEngine.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "OSMLoaderSplineActor.generated.h"

UCLASS()
class OSMLOADER_API AOSMLoaderSplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOSMLoaderSplineActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USplineComponent* customSpline;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void updateSpline();

	void createSpline(TArray<FVector>& points);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
