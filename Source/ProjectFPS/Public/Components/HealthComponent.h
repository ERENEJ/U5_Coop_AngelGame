// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/PickableItemBase.h"
#include "HealthComponent.generated.h"


class APickableItemBase;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTFPS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	float Health;
	void TakeDamage(float DamageReceived);
	void ExecuteDeathEvents();

	UFUNCTION(Reliable,Server)
	void Server_SpawnItem();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APickableItemBase> ItemToSpawn;		
};
