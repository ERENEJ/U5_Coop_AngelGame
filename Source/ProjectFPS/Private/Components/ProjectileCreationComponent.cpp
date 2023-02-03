// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ProjectileCreationComponent.h"

#include "ProjectileBase.h"


// Sets default values for this component's properties
UProjectileCreationComponent::UProjectileCreationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	// ...
}


// Called when the game starts
void UProjectileCreationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UProjectileCreationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UProjectileCreationComponent::CreateProjectile(AActor* ProjectileClass ,const FVector &MuzzleLocation,const FRotator &Direction)
{
	// you can call  client only functions at owner here
	
	Server_CreateProjectile(ProjectileClass, MuzzleLocation,Direction);

}

void UProjectileCreationComponent::Server_CreateProjectile_Implementation(AActor* ProjectileClass ,const FVector& MuzzleLocation,const FRotator& Direction)
{
	
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	if(ProjectileClass)
		GetWorld()->SpawnActor<AActor>(ProjectileClass->GetClass(), MuzzleLocation, Direction, ActorSpawnParams);

}


