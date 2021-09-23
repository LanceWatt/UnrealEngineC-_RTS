// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_EngageEnemy.generated.h"

/**
 * 
 */
UCLASS()
class CPP_RTS_API UBTTaskNode_EngageEnemy : public UBTTaskNode
{
	GENERATED_BODY()

public:

		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

		UPROPERTY()
		class ARTSStructure* EnemyStructure;
	
};
