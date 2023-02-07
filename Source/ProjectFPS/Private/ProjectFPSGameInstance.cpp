// Fill out your copyright notice in the Description page of Project Settings.
#include "ProjectFPSGameInstance.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "MenuSystem/MainMenu.h"

const static FName GSESSION_NAME = TEXT("My session game");
const static FName GSERVER_NAME_SETTING_KEY = TEXT("ServerName");

UProjectFPSGameInstance::UProjectFPSGameInstance(const FObjectInitializer& ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Constructor "));

	static ConstructorHelpers::FClassFinder<UUserWidget> MenuWBPClass(TEXT("/Game/MenuSystem/WBP_MenuBlueprint"));
	if (MenuWBPClass.Class)
	{
		MenuClass = MenuWBPClass.Class;
	}
}

void UProjectFPSGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Init"));
	
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem  as %s") , *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Found session interface"));
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UProjectFPSGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UProjectFPSGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UProjectFPSGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UProjectFPSGameInstance::OnJoinSessionComplete);
			
			

			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			if (SessionSearch.IsValid())
			{

				SessionSearch->bIsLanQuery = true;
				UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));
				SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
			}
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("not found subsytem"));


	if (GEngine != nullptr)
	{
		GEngine->OnNetworkFailure().AddUObject(this,&UProjectFPSGameInstance::OnNetworkFailure);
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Found class  as %s") ,*MenuClass->GetName());
}

void UProjectFPSGameInstance::OnNetworkFailure(UWorld * World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
		LoadMenuWidget();
}

void UProjectFPSGameInstance::LoadMenuWidget()
{
	if (!MenuClass) return;

	Menu = CreateWidget<UMainMenu>(this,MenuClass);
	Menu->Setup();
	Menu->SetMenuInterface(this);
}

void UProjectFPSGameInstance::Host(FString& ServerName)
{
	DesiredServerName = ServerName;
	
	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(FName(*ServerName));
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(FName(*ServerName));
		}
		else
		{
			CreateSession();	
		}
	}
}

void UProjectFPSGameInstance::Join(const FString& Address)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, (TEXT("Joining %s"), Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController -> ClientTravel(Address,TRAVEL_Absolute);
	}
}

void UProjectFPSGameInstance::JoinWithIndex( uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid())return;
	SessionInterface->JoinSession(0, GSESSION_NAME, SessionSearch->SearchResults[Index]);
}


void UProjectFPSGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	UE_LOG(LogTemp, Warning, TEXT("Start of oncreate Session complete"));
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create Session"));
		return;
	}
	
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("On create seasion complete message!"));

	UWorld* World = GetWorld();
	
	if (World)
	{
		
		World->ServerTravel("/Game/Platforms/Maps/TestMap?listen");
	}
}

void UProjectFPSGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	UE_LOG(LogTemp, Warning, TEXT("Start of ondestroy Session complete"));

	if (Success)
	{
		UE_LOG(LogTemp, Warning, TEXT(" destroyed will create new one Session"));
		CreateSession();
	}
}

void UProjectFPSGameInstance::RefreshServerList()
{
	if(Menu != nullptr)
	{
		SessionSearch = MakeShareable(new FOnlineSessionSearch());
		if (SessionSearch.IsValid() && Menu != nullptr)
		{
			SessionSearch->MaxSearchResults = 100;
			SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
			UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));
			SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
		}
	}
}


void UProjectFPSGameInstance::OnFindSessionComplete( bool Success)
{
	UE_LOG(LogTemp, Warning, TEXT(" begin of on find session Complete function"));
	if(Success && SessionSearch.IsValid() && Menu !=nullptr)
	{
		TArray<FServerData> ServerNames;

		for(FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT(" new found Session is %s"), *SearchResult.GetSessionIdStr());

			FServerData Data;
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections+1;
			Data.HostUserName = SearchResult.Session.OwningUserName;
			//Data.Name = SearchResult.GetSessionIdStr();
			
			FString OutServerName;
			if (SearchResult.Session.SessionSettings.Get(GSERVER_NAME_SETTING_KEY,OutServerName))
			{
				UE_LOG(LogTemp, Warning, TEXT("Data found in settings: %s"), *OutServerName);
				Data.Name = OutServerName;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Data Not found found in  at (OnFindSessionComplete- Onlinesessionsettings.cpp) "));
				Data.Name = "Could not get servername";
			}
			
			ServerNames.Add(Data);
		}

		UE_LOG(LogTemp, Warning, TEXT("End of find sesion complete function"));
		Menu->SetServerList(ServerNames);
	}	

}

void UProjectFPSGameInstance::OnJoinSessionComplete(FName SeassionName, EOnJoinSessionCompleteResult::Type JoinSessionCompleteResult)
{
	if(!SessionInterface.IsValid()) return;

	FString Address;
	if( !SessionInterface->GetResolvedConnectString(SeassionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT(" Could not get connect string"));
		return;
	}
	
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, (TEXT("Joining %s"), Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController -> ClientTravel(Address,TRAVEL_Absolute);
	}
}

void UProjectFPSGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings OnlineSessionSettings;

		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			OnlineSessionSettings.bIsLANMatch=true;
		}
		else
		{
			OnlineSessionSettings.bIsLANMatch = false;
		}
		
		OnlineSessionSettings.NumPublicConnections = 2;
		OnlineSessionSettings.bShouldAdvertise = true;
		OnlineSessionSettings.bUsesPresence = true;
		OnlineSessionSettings.bUseLobbiesIfAvailable = true;

		OnlineSessionSettings.Set(GSERVER_NAME_SETTING_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		FString OutData;
		if (OnlineSessionSettings.Get(GSERVER_NAME_SETTING_KEY, OutData))
		{
			SessionInterface->CreateSession(0, FName(*OutData), OnlineSessionSettings);	
		}
	}
}

