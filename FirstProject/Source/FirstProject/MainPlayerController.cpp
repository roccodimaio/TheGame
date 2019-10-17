// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
	}

	// Add to Viewport
	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	;

	// Check for an Enemy health bar
	if (WEnemyHealthBar)
	{
	
		// if so assign the Enemy health bar to EnemyHealthBar
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);

		// Check for Enemy health bar
		if (EnemyHealthBar)
		{
			// Add the Enemy health bar to the view port
			EnemyHealthBar->AddToViewport();

			// Hide the Enemy health bar on the view port
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}

		FVector2D Alignment(0.0f, 0.0f);
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}



}

void AMainPlayerController::Tick(float DelatTime)
{
	Super::Tick(DelatTime);

	if (EnemyHealthBar)
	{
		// Get EnemyLocation in 3d space and display in 2d on screen

		FVector2D PositionInViewPort;

		//Funtion call that takes a world location and projects to the screen
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewPort);

		PositionInViewPort.Y -= 100.0f; 

		FVector2D SizeInViewport = FVector2D(150.0f, 25.0f); 

		EnemyHealthBar->SetPositionInViewport(PositionInViewPort);

		//Set size of viewport
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
	UE_LOG(LogTemp, Warning, TEXT("In MainPlayerController->DisplayEnemyHealthBar()_01"));
	if (EnemyHealthBar)
	{
		UE_LOG(LogTemp, Warning, TEXT("In MainPlayerController->DisplayEnemyHealthBar()_02"));
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

