// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingActor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFloatingActor::AFloatingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CustomStaticMesh"));

	InitalLocation = FVector(0.0f);
	PlacedLocation = FVector(0.0f);
	WorldOrigin = FVector(0.0f, 0.0f, 0.0f);
	bInitializedFloatingActorLocations = false;
	InitalDirection = FVector(0.0f, 0.0f, 0.0f);
	InitialForce = FVector(200000.0f, 0.0f, 0.0f);
	InitialTorque = FVector(200000.0f, 0.0f, 0.0f);
	bShouldFloat = false; 
		
}

// Called when the game starts or when spawned
void AFloatingActor::BeginPlay()
{
	Super::BeginPlay();

	PlacedLocation = GetActorLocation(); 

	if (bInitializedFloatingActorLocations)
	{
		SetActorLocation(InitalLocation);
	}
	
	//FHitResult OutHitResult;
	//FVector LocalOffset = FVector(2000.0f, 0.0f, 0.0f);
	//FRotator Rotation = FRotator(0.0f, 0.0f, 30.0f);
	//AddActorWorldOffset(LocalOffset, true, &OutHitResult);
	//AddActorLocalRotation(Rotation);
	
	StaticMesh->AddForce(InitialForce);
	StaticMesh->AddTorque(InitialTorque);

}

// Called every frame
void AFloatingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldFloat)
	{
		FHitResult OutHitResult; 
		AddActorLocalOffset(InitalDirection, true, &OutHitResult);

		FVector HitLocation = OutHitResult.Location;

		//UE_LOG(LogTemp, Warning, TEXT("Hit location: X = %f, Y = %f, Z = %f!"), HitLocation.X, HitLocation.Y, HitLocation.Z);

	}

}

