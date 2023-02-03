// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ShipMovementComponent.h"

// Sets default values for this component's properties
UShipMovementComponent::UShipMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UShipMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UShipMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UShipMovementComponent::AccelerateForward(float Rate, USceneComponent* SceneComponent)
{
	if(SceneComponent == nullptr)return;
	
	SceneComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	if(GetNetMode()==ENetMode::NM_Client)
	{
		SceneComponent->AddWorldOffset(SceneComponent->GetForwardVector()*20);
	}

	Server_AccelerateForward(Rate, SceneComponent);
	

}

void UShipMovementComponent::Server_AccelerateForward_Implementation(float Rate, USceneComponent* SceneComponent)
{	
	if(SceneComponent == nullptr)return;
	SceneComponent->AddWorldOffset(SceneComponent->GetForwardVector()*20);
}

void UShipMovementComponent::AccelerateBackwards(float Rate, USceneComponent* SceneComponent)
{
	if(SceneComponent == nullptr)return;

	if(GetNetMode()==ENetMode::NM_Client)
	{
		SceneComponent->AddWorldOffset(SceneComponent->GetForwardVector()*-20);
	}
	SceneComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Server_AccelerateBackwards(Rate, SceneComponent);
	
}

void UShipMovementComponent::Server_AccelerateBackwards_Implementation(float Rate, USceneComponent* SceneComponent)
{
	if(SceneComponent == nullptr)return;
	SceneComponent->AddWorldOffset(SceneComponent->GetForwardVector()*-20);
}



