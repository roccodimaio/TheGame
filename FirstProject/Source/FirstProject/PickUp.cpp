// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUp.h"
#include "MainCharacter.h"

APickUp::APickUp()
{
	CoinCount = 1.0f; 
}
void APickUp::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	UE_LOG(LogTemp, Warning, TEXT("PickUp::OnOverLapBegin()"));

	if (OtherActor)
	{
		// Cast the retruned OtherActor to the MainCharacter which is an AMainCharater class
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		// Check if MainCharactor has been assigned a Character class
		if (MainCharacter)
		{
			MainCharacter->IncrementCoin(CoinCount);
		}
	}
}

void APickUp::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	UE_LOG(LogTemp, Warning, TEXT("PickUp::OnOverLapEnd()"));
}
