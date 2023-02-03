// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTFPS_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	// if an object is not posseble/ we want basic events such as opening doors attaching to ship bShouldPosses must return FALSE
	// if we want to posses the actor GunnerPod EscapePod DrivingWheel etc bShouldPosses must return TRUE
	// PossesByInteraction method takes AController* this might be changed to FPS_PlayerController try to make it work via AController
	
public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsPossessable();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractWithActor(APawn* InteractionCaller);

	/*there must be a pointer to Previous pawn to possess back
	UPROPERTY()
	APawn* PreviousPossessedPawn;
	if not, create one at the calelr pawn class
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPreviousPossessedPawn(APawn* PreviousPawnReference);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PossesByInteraction(AController* CallerController);
};
