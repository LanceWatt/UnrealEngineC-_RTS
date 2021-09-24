// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetNavigation.h"
#include "BTTask_SetCharacterState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "RTS_AI_Controller.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "../RTSGameMode.h"
#include "../EnumLibrary.h"
#include "../NavigationNode.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/Character_Harvester.h"
#include "../Character/CharacterBase.h"

EBTNodeResult::Type UBTTask_SetNavigation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    auto AIController = Cast<ARTS_AI_Controller>(OwnerComp.GetAIOwner());
    auto Blackboard = OwnerComp.GetBlackboardComponent();

    if (AIController)
    {
        APawn* AIPawn = AIController->GetPawn();
        ACharacterBase* AICharacter = Cast<ACharacterBase>(AIPawn);

        if (AICharacter)
        {
            // GameMode has all navigation nodes
            GameMode = Cast<ARTSGameMode>(GetWorld()->GetAuthGameMode());

            // Determine which navigation to filter for based on the CharState
            ECharacterState CharacterState = AICharacter->GetCharacterState();

            // Get all NavigationNodes
            TArray<ANavigationNode*> NavigationArray = GameMode->GetNavigationNodes();
            TArray<ANavigationNode*> BaseDefenseNodes;
            TArray<ANavigationNode*> OffenseNodes;

            // Guard the base
            if (CharacterState == ECharacterState::ECS_Patrolling)
            {

                int32 AssignedIndex = AICharacter->GetSquadData().SquadNumber;

                // Receive Base Defense Nodes
                for (int32 i = 0; i < NavigationArray.Num(); ++i)
                {
                    if (NavigationArray[i]->GetENavigationMode() == EAINavigation::EAN_BaseDefense)
                    {
                        BaseDefenseNodes.Add(NavigationArray[i]);
                    }
                }

                if (AssignedIndex < BaseDefenseNodes.Num())
                {
                    ANavigationNode* NavigationNode = BaseDefenseNodes[AssignedIndex];

                    FVector PatrolPointLocation = NavigationNode->GetActorLocation();
                    FVector MoveLocation = UNavigationSystemV1::GetRandomPointInNavigableRadius(GetWorld(), PatrolPointLocation, 2000.0f);
                    OwnerComp.GetBlackboardComponent()->SetValueAsVector("PatrolPoint", MoveLocation);
                    return EBTNodeResult::Succeeded;
                }
                
                // If base points covered, guard the Harvester
                if (AssignedIndex == BaseDefenseNodes.Num())
                {
                    TArray<AActor*> FoundHarvesters;
                    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Harvester::StaticClass(), FoundHarvesters);

                    for (int i = 0; i < FoundHarvesters.Num(); i++)
                    {
                        ACharacter_Harvester* Harvester = Cast<ACharacter_Harvester>(FoundHarvesters[i]);
                        if (Harvester->GetTeam() == AICharacter->GetTeam())
                        {
                            ACharacter_Harvester* HarvesterToDefend = Harvester;
                            FVector HarvesterPoint = HarvesterToDefend->GetActorLocation();
                            FVector MoveLocation = UNavigationSystemV1::GetRandomPointInNavigableRadius(GetWorld(), HarvesterPoint, 2000.0f);
                            OwnerComp.GetBlackboardComponent()->SetValueAsVector("PatrolPoint", MoveLocation);
                            return EBTNodeResult::Succeeded;
                        }
                    }
                }


                // Receive Offense Nodes
                for (int32 i = 0; i < NavigationArray.Num(); ++i)
                {
                    if (NavigationArray[i]->GetENavigationMode() == EAINavigation::EAN_Offense)
                    {
                        OffenseNodes.Add(NavigationArray[i]);
                    }
                }


                if (AssignedIndex > BaseDefenseNodes.Num())
                {
                        int RandomDefenseIndex = rand() % OffenseNodes.Num();
                        ANavigationNode* NavigationNode = OffenseNodes[RandomDefenseIndex];
                        FVector PatrolPointLocation = NavigationNode->GetActorLocation();

                        FVector MoveLocation = UNavigationSystemV1::GetRandomPointInNavigableRadius(GetWorld(), PatrolPointLocation, 2000.0f);
                        OwnerComp.GetBlackboardComponent()->SetValueAsVector("PatrolPoint", MoveLocation);
                        return EBTNodeResult::Succeeded;
                }
                
                
                
                
                // MoveToRandom Defense Nav
                //else if (BaseDefenseNodes.Num() <= AssignedIndex)
                //{
                //    int RandomDefenseIndex = rand() % BaseDefenseNodes.Num();
                //    ANavigationNode* NavigationNode = BaseDefenseNodes[RandomDefenseIndex];
                //    FVector PatrolPointLocation = NavigationNode->GetActorLocation();

                //    FVector MoveLocation = UNavigationSystemV1::GetRandomPointInNavigableRadius(GetWorld(), PatrolPointLocation, 1000.0f);
                //    OwnerComp.GetBlackboardComponent()->SetValueAsVector("PatrolPoint", MoveLocation);
                //}
            }
        }

        return EBTNodeResult::Succeeded;
    }
    return EBTNodeResult::Failed;
}



