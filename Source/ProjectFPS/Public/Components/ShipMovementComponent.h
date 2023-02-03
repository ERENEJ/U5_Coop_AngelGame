// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShipMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTFPS_API UShipMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShipMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AccelerateForward(float Rate, USceneComponent* SceneComponent);
	
	void AccelerateBackwards(float Rate, USceneComponent* SceneComponent);

	UFUNCTION(Reliable,Server)
	void Server_AccelerateForward(float Rate, USceneComponent* SceneComponent);

	UFUNCTION(Reliable,Server)
	void Server_AccelerateBackwards(float Rate, USceneComponent* SceneComponent);
		
};
