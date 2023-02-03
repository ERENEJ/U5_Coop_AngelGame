// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/SessionListWidget.h"

#include "MenuSystem/MainMenu.h"
#include "Components/Button.h"

void USessionListWidget::Setup(UMainMenu* InParent, uint32 InIndex)
{
	Parent = InParent;
	Index = InIndex;
	
	ServerNameButton->OnClicked.AddDynamic(this,&USessionListWidget::OnClicked);
}

void USessionListWidget::OnClicked()
{
	Parent->SelectIndex(Index);
}
