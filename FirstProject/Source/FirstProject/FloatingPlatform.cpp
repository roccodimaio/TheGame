// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components///StaticMeshComponent.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Adding a StaticMeshComponent to Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	// Assinging Mesh as the RootComponent of the class
	RootComponent = Mesh; 

	StartPoint = FVector(0.0f);
	EndPoint = FVector(0.0f);

}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();

	// Set the StartPoint on BeginPlay
	StartPoint = GetActorLocation(); 

	/**EndPoint widget uses delta in location from StartPoint in local space, 
	need to add world space from StartPoint */
	EndPoint += StartPoint; 

	// Speed that FloatinPlatform will move, can be changed via blueprint
	InterpSpeed = 4.0f; 
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Retrieve  current location every frame
	FVector CurrentLocation = GetActorLocation();

	// Mathematical method to move between two points smoothly
	FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);

	SetActorLocation(Interp); 
}

