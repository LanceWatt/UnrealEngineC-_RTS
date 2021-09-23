// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Harvester.h"

#include "RTS_AI_Controller.h"
#include "../Character/Character_Harvester.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "../RTSGameMode.h"
#include "../GameActors/RTSStructure.h"


UBTService_Harvester::UBTService_Harvester()
{
    HarvesterState = EHarvesterStates::EHT_MoveToHarvest;
    bCharacterIsDead = false;
}

void UBTService_Harvester::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    auto CharacterAIController = Cast<ARTS_AI_Controller>(OwnerComp.GetAIOwner());

    if (CharacterAIController)
    {
        APawn* AIPawn = CharacterAIController->GetPawn();
        AIHarvester = Cast<ACharacter_Harvester>(AIPawn);
        if (AIHarvester)
        {
            HarvesterState = AIHarvester->GetHarvesterState();      
            OwnerComp.GetBlackboardComponent()->SetValueAsEnum("EHarvesterStates", (uint8)HarvesterState);

        }
    }
}
