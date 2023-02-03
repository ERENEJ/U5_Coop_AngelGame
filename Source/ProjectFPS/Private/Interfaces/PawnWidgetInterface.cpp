// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/PawnWidgetInterface.h"

// Add default functionality here for any IPawnWidgetInterface functions that are not pure virtual.
TSubclassOf<UUserWidget> IPawnWidgetInterface::GetPawnWidgetClass()
{
	return nullptr;
}
