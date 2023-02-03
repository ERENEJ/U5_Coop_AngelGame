// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RotationComponent.h"

// Sets default values for this component's properties
URotationComponent::URotationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void URotationComponent::BeginPlay()
{
	Super::BeginPlay();

}

void URotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void URotationComponent::TiltSceneComponent(float Rate, USceneComponent* SceneComponent)
{
	
	if(GetNetMode()==ENetMode::NM_Client)
	{
		FRotator TurnRotation (0.f, 0.f, 1.f);
		SceneComponent->AddLocalRotation(TurnRotation*Rate*-1);
	}

	Server_TiltSceneComponent(Rate, SceneComponent);
}

void URotationComponent::Server_TiltSceneComponent_Implementation(float Rate, USceneComponent* SceneComponent)
{
	FRotator TurnRotation (0.f, 0.f, 1.f);
	SceneComponent->AddLocalRotation(TurnRotation*Rate*-1);
}

void URotationComponent::TurnSceneComponent(float Rate, USceneComponent* SceneComponent)
{
	if(GetNetMode()==ENetMode::NM_Client)
	{
		FRotator TurnRotation (0.f, 1.f, 0.f);
		SceneComponent->AddLocalRotation(TurnRotation*Rate);
	}
	Server_TurnSceneComponent(Rate, SceneComponent);

}

void URotationComponent::Server_TurnSceneComponent_Implementation(float Rate, USceneComponent* SceneComponent)
{
	FRotator TurnRotation (0.f, 1.f, 0.f);
	SceneComponent->AddLocalRotation(TurnRotation*Rate);
}

void URotationComponent::LookUpSceneComponent(float Rate, USceneComponent* SceneComponent)
{
	if(GetNetMode()==ENetMode::NM_Client)
	{
		FRotator TurnRotation (1.f, 0.f, 0.f);
		SceneComponent->AddLocalRotation(TurnRotation*Rate*-1);	
	}

	Server_LookUpSceneComponent(Rate, SceneComponent);
}

void URotationComponent::Server_LookUpSceneComponent_Implementation(float Rate, USceneComponent* SceneComponent)
{
	FRotator TurnRotation (1.f, 0.f, 0.f);
	SceneComponent->AddLocalRotation(TurnRotation*Rate*-1);
}

