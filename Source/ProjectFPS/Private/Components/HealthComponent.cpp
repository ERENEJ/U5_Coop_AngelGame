// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Health = 100;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::TakeDamage(float DamageReceived)
{
	Health -=DamageReceived;
	if(Health <= 0)
	{
		ExecuteDeathEvents();
	}
}

void UHealthComponent::ExecuteDeathEvents()
{
	UE_LOG(LogTemp, Warning, TEXT("Execute death events called from health component"));


	 
	if(GetWorld() && GetOwner()) //&& ItemToSpawn)
	{
		//commented out for Angel Game
		//GetWorld()->SpawnActor<APickableItemBase>(ItemToSpawn, GetOwner()->GetActorLocation(),GetOwner()->GetActorRotation());
		//Server_SpawnItem();
		GetOwner()->Destroy();
	
	}
	
}

void UHealthComponent::Server_SpawnItem_Implementation()
{
	GetWorld()->SpawnActor<APickableItemBase>(ItemToSpawn, GetOwner()->GetActorLocation(),GetOwner()->GetActorRotation());
}


