// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_HarvesterNavigationTask.h"
#include "BTTask_SetCharacterState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "RTS_AI_Controller.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "../NavigationNode.h"
#include "NavigationSystem.h"
#include "../GameActors/Crystals.h"
#include "Kismet/GameplayStatics.h"
#include "../GameActors/RTSStructure.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "../Character/Character_Harvester.h"

/*	This  Behavior Tree Task sets the places to move to for resource collecting tasks.
*/
EBTNodeResult::Type UBTTask_HarvesterNavigationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    auto AIController = Cast<ARTS_AI_Controller>(OwnerComp.GetAIOwner());
    auto Blackboard = OwnerComp.GetBlackboardComponent();

    if (AIController)
    {
        APawn* AIPawn = AIController->GetPawn();
        Harvester = Cast<ACharacter_Harvester>(AIPawn);

        if (Harvester)
        {
            HarvesterState = Harvester->GetHarvesterState();

            switch (HarvesterState)
            {
				case EHarvesterStates::EHT_MoveToHarvest:

				if (FindCrystals(NearestMine))
				{				
					MeasureDistance = FVector::Dist(Harvester->GetActorLocation(), ClosestMine->GetActorLocation());
					 
					if (MeasureDistance < 1000.f)
					{
						HarvesterState = (EHarvesterStates::EHT_Mining);
						Harvester->SetHarvesterState(EHarvesterStates::EHT_Mining);
						return EBTNodeResult::Succeeded;
					}
					else
					{
						FVector MiningLoc = ClosestMine->GetActorLocation();
						OwnerComp.GetBlackboardComponent()->SetValueAsVector("PatrolPoint", MiningLoc);
						return EBTNodeResult::Succeeded;
					}
				}

				break;
				case EHarvesterStates::EHT_Mining:

						FindCrystals(NearestMine);
						if (NearestMine)
						{
							Harvester->StartMining(NearestMine);
							return EBTNodeResult::Succeeded;
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("no mine"));
						}				
				break;
				case EHarvesterStates::EHT_MoveToRefinery:

					if ((Harvester->GetRefineryReference()))
					{
						SetRefineryLocation();

						MeasureDistance = FVector::Dist(Harvester->GetActorLocation(), UnloadLocation);
						UE_LOG(LogTemp, Warning, TEXT("MeasureDistance %f"), MeasureDistance);
						if (MeasureDistance < 800)
						{
							Harvester->SetHarvesterState(EHarvesterStates::EHT_Unloading);						
						}
						OwnerComp.GetBlackboardComponent()->SetValueAsVector("PatrolPoint", UnloadLocation);
						return EBTNodeResult::Succeeded;
					}
					else
					{
						Harvester->SetHarvesterState(EHarvesterStates::EHT_RefineryDestroyed);
						return EBTNodeResult::Succeeded;
					}

				break;

				case EHarvesterStates::EHT_Unloading:

					if ((Harvester->GetRefineryReference()))
					{
						Harvester->StartUnloadingTimer();
						return EBTNodeResult::Succeeded;
					}
					else
					{
						Harvester->SetHarvesterState(EHarvesterStates::EHT_RefineryDestroyed);
						return EBTNodeResult::Succeeded;
						
					}

				break;
            } 
        }
    }
    return EBTNodeResult::Failed;
}

bool UBTTask_HarvesterNavigationTask::FindCrystals(AActor* &NearestCrystal)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrystals::StaticClass(), FoundActors);

	if (FindNearestActor(FoundActors, NearestCrystal))
	{
		return true;
	}
	return false;
}

bool UBTTask_HarvesterNavigationTask::FindNearestActor(TArray<AActor* >& ActorArray, AActor*& NearestActor)
{
	if (ActorArray.Num() <= 0) return false;
	float currentClosestDistance = TNumericLimits<float>::Max();

	for (int i = 0; i < ActorArray.Num(); i++)
	{

		float distance = FVector::DistSquared(Harvester->GetActorLocation(), ActorArray[i]->GetActorLocation());
		if (distance < currentClosestDistance)
		{
			currentClosestDistance = distance;
			NearestActor = ActorArray[i];
			ClosestMine = NearestActor;
		}
	}
	return true;
}

void UBTTask_HarvesterNavigationTask::SetRefineryLocation()
{
	FRotator UnloadRotation;

	if (Harvester->GetRefineryReference())
	{
		Harvester->GetRefineryReference()->GetHarvesterSpawnLocation(UnloadLocation, UnloadRotation);
	}
}