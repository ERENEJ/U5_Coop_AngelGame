// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MainMenu.generated.h"


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
	class UPanelWidget* SessionListScrollBox;

	UPROPERTY(meta = (BindWidget))
	class UWidget* SessionList;

	TSubclassOf<UUserWidget> SessionListClass;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* SinglePlayerButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* MultiplayerButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenu_HostButton;
	
	/*   text is renamed as join with ip address  button name stayed same   */
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinSessionButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinWithSeassionButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;
	UPROPERTY(meta = (BindWidget))
	
	class UButton* MultiplayerMenu_CancelButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenu_CancelButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelSessionMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinIPButton;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinWithIPMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* MultiplayerMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* IPAddressBox;
	
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* HostMenu_HostNameBox;
	

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

	/*
	UFUNCTION()
	void JoinSeassion();
	*/
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
