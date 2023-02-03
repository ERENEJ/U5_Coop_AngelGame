// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngameMenu.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFPS_API UIngameMenu : public UUserWidget
{
	GENERATED_BODY()

	public:
	void Setup();

	protected:
	virtual bool Initialize() override;

	private:
	UPROPERTY(meta = (BindWidget))
	class UButton* ReturnMainMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ContinueButton;
	

	UFUNCTION()
	void ReturnMainMenu();

	UFUNCTION()
	void Continue();

	


};
