// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ProjectFPSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFPS_API UProjectFPSGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()
	
public:
	UProjectFPSGameInstance(const FObjectInitializer & ObjectInitializer);
	virtual void Init();
	
	UFUNCTION(BlueprintCallable)
	void LoadMenuWidget();
	
	UFUNCTION(Exec)
	void Host(FString& HostName) override;

	UFUNCTION(Exec)
	void Join(const FString& Address) override;
	UFUNCTION(Exec)
	void JoinWithIndex(uint32 Index ) override;


	void RefreshServerList() override;

	private:
	
	TSubclassOf<UUserWidget> MenuClass;
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
	
	FString DesiredServerName;
	
	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionComplete(bool Success);
	void OnJoinSessionComplete(FName SeassionName, EOnJoinSessionCompleteResult::Type JoinSessionCompleteResult);
	void CreateSession();
	void OnNetworkFailure(UWorld * World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString = TEXT(""));

	UPROPERTY()
	class UMainMenu* Menu;

	
	
};





