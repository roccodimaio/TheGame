// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Critter.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint()
{
	// Need for UKismetMathLibrary function below
	FVector Extend = SpawningBox->GetScaledBoxExtent();  	// Extent is for dimensions of Spawnbox
	FVector Origin = SpawningBox->GetComponentLocation();	// Origin is for center of Spawnbox

	// Calculate randon point in SpawingBox.  Will be used to determin spawning point
	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extend); 

	return Point; 
}

void ASpawnVolume::SpawnOurPawn_Implementation(UClass* ToSpawn, const FVector& Location)
{
	if (ToSpawn)
	{
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams; 


		if (World)
		{
			// Construct and spawn an actor, then return the actor that was spawned
			ACritter* CritterSpawn = World->SpawnActor<ACritter>(ToSpawn, Location, FRotator(0.0f), SpawnParams);
		}
	}
}

