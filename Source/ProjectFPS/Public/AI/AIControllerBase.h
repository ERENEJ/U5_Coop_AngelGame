// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Perception/PawnSensingComponent.h"
#include "AIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFPS_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()
		
	private:
	AAIControllerBase();
	virtual void Tick(float DeltaSeconds) override;
	
	/*A Pawn Sensing Component, responsible for sensing other Pawns*/
	/*
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComp;
	*/
	/*Hearing function - will be executed when we hear a Pawn*/
	UFUNCTION()
	void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);

	protected:
	virtual void BeginPlay() override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	virtual FPathFollowingResult GetOnMovementCompletedResult();

	
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;
	

	public:

	
	FPathFollowingResult MovementResult;
	bool IsDead() const;
	void FinishTask(UBTTaskNode Node);
	
};
