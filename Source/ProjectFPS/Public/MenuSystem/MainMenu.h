// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MainMenu.generated.h"

class UButton;
class UWidgetSwitcher;
class UEditableTextBox;

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	FString HostUserName;
	uint16  MaxPlayers;
	uint16  CurrentPlayers;
};

/**
 * 
 */
UCLASS()
class PROJECTFPS_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetMenuInterface(IMenuInterface* I_MenuInterface);

	UMainMenu(const FObjectInitializer& ObjectInitializer);

	void SetServerList(TArray<FServerData> ServerNames);
	
	void Setup();

	void SelectIndex(uint32 Index);

	
	protected:

	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
	virtual bool Initialize() override;

	private:

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* SessionListScrollBox;

	UPROPERTY(meta = (BindWidget))
	UWidget* SessionList;

	TSubclassOf<UUserWidget> SessionListClass;
	
	UPROPERTY(meta = (BindWidget))
	UButton* SinglePlayerButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* MultiplayerButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* HostMenu_HostButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinSessionButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* JoinWithSeassionButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton;

	UPROPERTY(meta = (BindWidget))
	UButton* MultiplayerMenu_CancelButton;

	UPROPERTY(meta = (BindWidget))
	UButton* HostMenu_CancelButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelSessionMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinIPButton;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	UWidget* JoinWithIPMenu;

	UPROPERTY(meta = (BindWidget))
	UWidget* MultiplayerMenu;

	UPROPERTY(meta = (BindWidget))
	UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* IPAddressBox;
	
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* HostMenu_HostNameBox;
	

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void SetHostServerVariables();
	
	UFUNCTION()
	void JoinServerWithIPAdress();
	UFUNCTION()	
	void JoinServerWithIndex();

	UFUNCTION()
	void StartSinglePlayerGame();

	UFUNCTION()
	void OpenMultiplayerMenu();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void OpenJoinwithSeassionMenu();

	UFUNCTION()
	void OpenMainMenu();
	
	UFUNCTION()
	void ExitGame();
	
	TOptional<uint32> SelectedIndex;
	
	IMenuInterface* MenuInterface;

	FString HostName;

	void UpdateChildren();
};
