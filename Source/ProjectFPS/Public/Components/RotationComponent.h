// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RotationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTFPS_API URotationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URotationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	virtual void Activate(bool bReset) override;
	virtual void OnRegister() override;
*/
	
	
	void TiltSceneComponent(float Rate, USceneComponent* SceneComponent);
	
	void TurnSceneComponent(float Rate, USceneComponent* SceneComponent);

	void LookUpSceneComponent(float Rate, USceneComponent* SceneComponent);

	UFUNCTION(Reliable,Server)
	void Server_TiltSceneComponent(float Rate, USceneComponent* SceneComponent);

	UFUNCTION(Reliable,Server)
	void Server_TurnSceneComponent(float Rate, USceneComponent* SceneComponent);

	UFUNCTION(Reliable,Server)
	void Server_LookUpSceneComponent(float Rate, USceneComponent* SceneComponent);
		
};
