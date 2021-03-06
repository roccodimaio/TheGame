// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUp.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"

APickUp::APickUp()
{
	
}
void APickUp::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//UE_LOG(LogTemp, Warning, TEXT("PickUp::OnOverLapBegin()"));

	if (OtherActor)
	{
		// Cast the retruned OtherActor to the MainCharacter which is an AMainCharater class
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		// Check if MainCharactor has been assigned a Character class
		if (MainCharacter)
		{
			OnPickupBP(MainCharacter);

			// Add the location (FVector) of where the item was picked up to the PickupLocations TArray declared in MainCharacter
			MainCharacter->PickupLocations.Add(GetActorLocation());

			// Check if OverlapParticles has been set via blueprint
			if (OverlapParticles)
			{
				// Spawn particle OverlapParticles on OverlapBegin
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.0f), true);
			}

			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}

			Destroy();
		}
	}
}

void APickUp::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	UE_LOG(LogTemp, Warning, TEXT("PickUp::OnOverLapEnd()"));
}
