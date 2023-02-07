// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionListWidget.generated.h"


class UMainMenu;
class UTextBlock;
UCLASS()
class PROJECTFPS_API USessionListWidget : public UUserWidget
{
	GENERATED_BODY()

	public:
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ServerName;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HostUser;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerNumber;
	
	void Setup(class UMainMenu* Parent, uint32 index);

	UPROPERTY(BlueprintReadOnly)
	bool Selected = false;
	
private:
	UPROPERTY(meta = (BindWidget))
	class UButton* ServerNameButton;

	UFUNCTION()
	void OnClicked();

	UPROPERTY()
	UMainMenu* Parent;

	uint32 Index;

	
	
};
