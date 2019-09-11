// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	// Adding a mesh component to the class to hold the default mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
	class UStaticMeshComponent* Mesh;

	// Vector to hold the starting/return point of the platform
	UPROPERTY(EditAnywhere)
	FVector StartPoint;

	// Vector to hold the end/travel to point of the platform
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true")) // meta - creates a visualization widget for the FVector
	FVector EndPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpSpeed;

	FTimerHandle InterpTimer; 



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
