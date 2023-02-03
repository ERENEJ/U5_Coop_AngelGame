// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIControllerBase.h"

#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

AAIControllerBase::AAIControllerBase()
{
	//PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
}

void AAIControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

bool AAIControllerBase::IsDead() const
{
	return false;
}


void AAIControllerBase::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
}

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
	}

	/*
	if (PawnSensingComp)
	{
		//Registering the delegate which will fire when we hear something
		PawnSensingComp->OnHearNoise.AddDynamic(this, &AAIControllerBase::OnHearNoise);
	}
	*/
}

void AAIControllerBase::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
}
void AAIControllerBase::FinishTask(UBTTaskNode Node)
{
	//Node.FinishLatentTask(, EBTNodeResult::Succeeded);
}

FPathFollowingResult AAIControllerBase::GetOnMovementCompletedResult()
{
	return MovementResult;
}
