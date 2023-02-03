// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_VectoralChase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFPS_API UBTTask_VectoralChase : public UBTTaskNode
{
	GENERATED_BODY()
	public:
	UBTTask_VectoralChase();

	protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) override;
	
};
