// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPS_PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFPS_API AFPS_PlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual  void OnPossess(APawn* InPawn) override;

	
	// since this function will be called from onPosses 
	// I decided to run it on only the client that calls the on posses event
	UFUNCTION(Client, Reliable)
	void RemovePreviousWidget();

	UFUNCTION(Client, Reliable)
	void AddWidgetToScreen(TSubclassOf<UUserWidget> UserWidgetClass);

	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY()
	UUserWidget *HUD;

	
};
