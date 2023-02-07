// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_PlayerControllerBase.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/PawnWidgetInterface.h"

void AFPS_PlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

}

void AFPS_PlayerControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	
	RemovePreviousWidget();
	
	//And add new pawn related widget
	IPawnWidgetInterface* PawnWidgetInterface = Cast<IPawnWidgetInterface>(InPawn);
	
	if (PawnWidgetInterface)
	{
		AddWidgetToScreen(PawnWidgetInterface->GetPawnWidgetClass());
		
		UE_LOG(LogTemp, Warning, TEXT("widget interface is valid and must be added right now"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("widget interface is NOT VALID"));
		
	}

}

void AFPS_PlayerControllerBase::AddWidgetToScreen_Implementation(TSubclassOf<UUserWidget> UserWidgetClass)
{
	if (IsLocalController())
	{
		UUserWidget* UserWidget = CreateWidget(this, UserWidgetClass);
    
		if (UserWidget != nullptr)
		{
			HUD = UserWidget;
			UserWidget->AddToViewport();
		}
	}
}
void AFPS_PlayerControllerBase::RemovePreviousWidget_Implementation()
{
	if(HUD)
	{
		//UE_LOG(LogTemp, Warning, TEXT("HUD REF IS  VALID"));
		HUD->RemoveFromViewport();
	}
}

