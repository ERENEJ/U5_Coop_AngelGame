// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ShyGuyVectoralChase.h"
#include "CharacterBase.h"
#include "AIController.h"
#include "AI/AI_CharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ShyGuyVectoralChase::UBTTask_ShyGuyVectoralChase()
{
	NodeName = "ShyGuyVectoralChase";
	
}

EBTNodeResult::Type UBTTask_ShyGuyVectoralChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	if (OwnerComp.GetOwner()==nullptr)
		return EBTNodeResult::Failed;
	if (OwnerComp.GetBlackboardComponent() == nullptr)
		return EBTNodeResult::Failed;	

	AAI_CharacterBase* AICharacter = Cast <AAI_CharacterBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));
	ACharacterBase* Character = Cast <ACharacterBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));

	if ( !(AICharacter && Character))
	{
		return EBTNodeResult::Failed;
	}

	FVector OwnerLocation = AICharacter->GetActorLocation();
	FVector TargetLocation = Character->GetActorLocation();
	FVector NextVectoralDirection = (TargetLocation - OwnerLocation);
	NextVectoralDirection.Normalize();


	FVector Destination = OwnerLocation + NextVectoralDirection*500;
	Destination.Z = 0;

	EPathFollowingRequestResult::Type  MovementResultData = OwnerComp.GetAIOwner()->MoveToLocation(Destination);

	if (MovementResultData == EPathFollowingRequestResult::Failed)
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}