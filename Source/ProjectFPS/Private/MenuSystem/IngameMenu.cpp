// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/IngameMenu.h"
#include "Components/Button.h"
#include "ProjectFPSGameInstance.h"

bool UIngameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;
	
	if (ReturnMainMenuButton == nullptr) return false;
	ReturnMainMenuButton->OnClicked.AddDynamic(this, &UIngameMenu::ReturnMainMenu);

	if (ContinueButton == nullptr) return false;
	ContinueButton->OnClicked.AddDynamic(this, &UIngameMenu::Continue);
	
	return true;
}




void UIngameMenu::Setup()
{

	
	
	this -> AddToViewport();
	this->bIsFocusable = true;

	APlayerController* PlayerController = GetOwningPlayer();
	
	if (!PlayerController) return;

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	
	PlayerController->SetInputMode(InputModeData);
	PlayerController->SetShowMouseCursor(true);
	
}


void UIngameMenu::ReturnMainMenu()
{
	RemoveFromViewport();

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr))
		return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController -> ClientTravel("/Game/MenuSystem/MainMenu",TRAVEL_Absolute);
	}

	/* old slow method
	UPuzzlePlatformsGameInstance* PuzzlePlatformsGameInstance = Cast<UPuzzlePlatformsGameInstance>(GetGameInstance());
	if (PuzzlePlatformsGameInstance == nullptr) return;
	PuzzlePlatformsGameInstance->LoadMenu();
	*/

}

void UIngameMenu::Continue()
{
	RemoveFromViewport();
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr))
		return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr))
		return;
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = false;
}
