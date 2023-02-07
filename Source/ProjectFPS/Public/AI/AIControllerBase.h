// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerBase.generated.h"

UCLASS()
class PROJECTFPS_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()
		
	private:
	AAIControllerBase();
	virtual void Tick(float DeltaSeconds) override;
	

	protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	UBehaviorTree* AIBehavior;

};
