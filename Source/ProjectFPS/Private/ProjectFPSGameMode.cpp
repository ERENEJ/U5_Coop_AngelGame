// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectFPSGameMode.h"
#include "UObject/ConstructorHelpers.h"

AProjectFPSGameMode::AProjectFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BlueprintClasses/CharacterBase/BP_CharacterBase"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	
}
