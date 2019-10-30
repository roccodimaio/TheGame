// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FirstProjectSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Health;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Mana;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxMana;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxStamina; 

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	uint32 PowerCells;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FRotator Rotation; 


};

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UFirstProjectSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UFirstProjectSaveGame(); 

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex; 

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FCharacterStats CharacterStats;
	
};
