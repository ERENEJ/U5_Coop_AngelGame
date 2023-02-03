// Fill out your copyright notice in the Description page of Project Settings.
#include "MenuSystem/MainMenu.h"

#include "MenuSystem/SessionListWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"



UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	
	static ConstructorHelpers::FClassFinder<UUserWidget> MenuWBPClass(TEXT("/Game/MenuSystem/WBP_SessionList"));
	if (MenuWBPClass.Class)
	{
		SessionListClass = MenuWBPClass.Class;
	}
}




bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;
	
	if (SinglePlayerButton == nullptr) return false;
	SinglePlayerButton->OnClicked.AddDynamic(this, &UMainMenu::StartSinglePlayerGame);

	if (MultiplayerButton == nullptr) return false;
	MultiplayerButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMultiplayerMenu);

	if (HostButton == nullptr) return false;
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::SetHostServerVariables);
	
	if (HostMenu_HostButton == nullptr) return false;
	HostMenu_HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (JoinIPButton == nullptr) return false;
	JoinIPButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServerWithIPAdress);
	
	

	/*
	if (JoinWithSeassionButton == nullptr) return false;
	JoinWithSeassionButton->OnClicked.AddDynamic(this, &UMainMenu::UMainMenu::JoinServerWithIndex);
	*/
	
	/* Index meant to be session indexes at join with session menu  */
	if (JoinSessionButton == nullptr) return false;
	JoinSessionButton->OnClicked.AddDynamic(this, &UMainMenu::UMainMenu::JoinServerWithIndex);
	
	if (JoinButton == nullptr) return false;
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	/* Open session list menu NOT labeled as JOIN */
	if (JoinWithSeassionButton == nullptr) return false;
	JoinWithSeassionButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinwithSeassionMenu);

	if (HostMenu_CancelButton == nullptr) return false;
	HostMenu_CancelButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
	
	if (CancelSessionMenuButton == nullptr) return false;
	CancelSessionMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
	
	if (CancelButton == nullptr) return false;
	CancelButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (MultiplayerMenu_CancelButton == nullptr) return false;
	MultiplayerMenu_CancelButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (ExitButton == nullptr) return false;
	ExitButton->OnClicked.AddDynamic(this, &UMainMenu::ExitGame);
	return true;
}


void UMainMenu::SetMenuInterface(IMenuInterface* I_MenuInterface)
{
	MenuInterface = I_MenuInterface;
}



void UMainMenu::Setup()
{
	this->AddToViewport();
	this->bIsFocusable = true;
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	
	PlayerController->SetInputMode(InputModeData);
	PlayerController->SetShowMouseCursor(true);

	MenuSwitcher->SetActiveWidget(MainMenu);

}


void UMainMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) 
{
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
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

void UMainMenu::HostServer()
{
	if(!ensure(HostMenu!=nullptr)) return;
	if(!ensure(MenuSwitcher!=nullptr)) return;
	MenuSwitcher->SetActiveWidget(HostMenu);
	if(HostMenu_HostNameBox->GetText().ToString().Len()>1)
	{
		HostName = HostMenu_HostNameBox->GetText().ToString();
	}
	else
	{
		HostName = "DefaultName";
	}
	
	
	if ( MenuInterface == nullptr) return;
	MenuInterface -> Host(HostName);
	
}

void UMainMenu::SetHostServerVariables()
{
	if(!ensure(HostMenu!=nullptr)) return;
	if(!ensure(MenuSwitcher!=nullptr)) return;
	MenuSwitcher->SetActiveWidget(HostMenu);


}

/*

if(!ensure(HostMenu!=nullptr)) return;
if(!ensure(MenuSwitcher!=nullptr)) return;
MenuSwitcher->SetActiveWidget(HostMenu);

MenuInterface -> Host(Test);

 */

void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (!ensure(World!=nullptr)) return;

	//removing all old seassions from list @joinwithseasion menu
	SessionListScrollBox->ClearChildren();

	uint32 i = 0;
	
	for (const FServerData& ServerData: ServerNames)
	{
		USessionListWidget* SessionRow = CreateWidget<USessionListWidget>(World,SessionListClass);

		SessionRow->ServerName->SetText(FText::FromString((ServerData.Name)));
		SessionRow->HostUser->SetText(FText::FromString((ServerData.HostUserName)));
		SessionRow->PlayerNumber->SetText(FText::FromString((FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers))));
		SessionRow->Setup(this,i);
		++i;
		SessionListScrollBox->AddChild(SessionRow);
	}

}

void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateChildren();
}
void UMainMenu::UpdateChildren()
{
	for (int32 i=0; i < SessionListScrollBox->GetChildrenCount(); i++)
	{
		USessionListWidget* SessionListWidget = Cast <USessionListWidget> (SessionListScrollBox->GetChildAt(i));
		if (SessionListWidget)
		{
			SessionListWidget->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
		}
	}
}



void UMainMenu::JoinServerWithIPAdress()
{
	if ( MenuInterface == nullptr) return;
	const FString Address = IPAddressBox->GetText().ToString();

	if (!ensure(IPAddressBox!=nullptr)) return;
	MenuInterface -> Join(Address);
}

void UMainMenu::JoinServerWithIndex()
{
	if ( SelectedIndex.IsSet()  && MenuInterface!=nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index %d"), SelectedIndex.GetValue());
		MenuInterface->JoinWithIndex(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index not set"));
	}

}

void UMainMenu::StartSinglePlayerGame()
{
	UWorld* World = GetWorld();
	
	if (World)
	{
		//TODO this location creates the map change this value when new map is finished
		World->ServerTravel("/Game/Platforms/Maps/TestMap");
		
	}
}

void UMainMenu::OpenMultiplayerMenu()
{
	if(!ensure(MultiplayerMenu!=nullptr)) return;
	if(!ensure(MenuSwitcher!=nullptr)) return;
	MenuSwitcher->SetActiveWidget(MultiplayerMenu);
}

/*
void UMainMenu::JoinSeassion()
{
	if (SelectedIndex.IsSet())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index %d"), SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index is not set "));
		return;
	}

	MenuInterface -> JoinWithIndex(SelectedIndex.GetValue());
	
}
*/

void UMainMenu::OpenJoinMenu()
{
	if(!ensure(JoinWithIPMenu!=nullptr)) return;
	if(!ensure(MenuSwitcher!=nullptr)) return;
	MenuSwitcher->SetActiveWidget(JoinWithIPMenu);

	if (MenuInterface!=nullptr)
	{
		MenuInterface->RefreshServerList();
	}
	
}

void UMainMenu::OpenJoinwithSeassionMenu()
{
	if(!ensure(SessionList!=nullptr)) return;
	if(!ensure(MenuSwitcher!=nullptr)) return;
	MenuSwitcher->SetActiveWidget(SessionList);

	
	if (MenuInterface!=nullptr)
	{
		MenuInterface->RefreshServerList();
	}
	
}

void UMainMenu::OpenMainMenu()
{
	if(!ensure(MainMenu!=nullptr)) return;
	if(!ensure(MenuSwitcher!=nullptr)) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::ExitGame()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr))
		return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr))
		return;

	PlayerController->ConsoleCommand("quit");
}
