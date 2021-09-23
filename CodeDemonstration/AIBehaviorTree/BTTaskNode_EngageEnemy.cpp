// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_EngageEnemy.h"
#include "BTTask_SetCharacterState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "RTS_AI_Controller.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "../RTSGameMode.h"
#include "../EnumLibrary.h"
#include "../NavigationNode.h"
#include "NavigationSystem.h"
#include "../GameActors/RTSStructure.h"
#include "../Character/CharacterBase.h"


EBTNodeResult::Type UBTTaskNode_EngageEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    auto AIController = Cast<ARTS_AI_Controller>(OwnerComp.GetAIOwner());
    auto Blackboard = OwnerComp.GetBlackboardComponent();

    if (AIController)
    {
        APawn* AIPawn = AIController->GetPawn();
        ACharacterBase* AICharacter = Cast<ACharacterBase>(AIPawn);

        if (AICharacter)
        {
            AActor* EnemyActor = AICharacter->GetEnemyActor();

            // Check To See if Character Or Structure. This is to ensure firing is compatible with differing mesh types
            ACharacterBase* TargetEnemy = Cast<ACharacterBase>(EnemyActor);

            if (IsValid(TargetEnemy))
            {
                AICharacter->SetEnemyCharacter(TargetEnemy);
                AICharacter->EngageEnemy();
                return EBTNodeResult::Succeeded;
            }

            // If Cast To ACharacterBase fails, try Structure
            ARTSStructure* TargetStructure = Cast<ARTSStructure>(EnemyActor);

            if (IsValid(TargetStructure))
            {
                AICharacter->SetEnemyStructure(TargetStructure);
                AICharacter->EngageStructure();
                return EBTNodeResult::Succeeded;
            }
        }
    }    
    return EBTNodeResult::Failed;
}

  

