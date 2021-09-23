// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_AIChar.h"
#include "RTS_AI_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "../EnumLibrary.h"
#include "../RTSGameMode.h"
#include "../Character/CharacterBase.h"
#include "../GameActors/RTSStructure.h"
#include "DrawDebugHelpers.h"


/*   Service ticks every 0.5 seconds.Updates the state of the AI Character and set the states in the behavior tree.
     The behaviors are categorised using the ECharacter type enum, setting Idle, spawned, player controlled or combat states.*/

UBTService_AIChar::UBTService_AIChar()
{
    bCreateNodeInstance = true;
    bValuesSet = false;
    EnemyCharacterType = ECharacterType::ECT_Soldier;
    bCharacterIsDead = false;
}

void UBTService_AIChar::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // STATE INITIALISATION
    if (!bValuesSet)
    {
        auto CharacterAIController = Cast<ARTS_AI_Controller>(OwnerComp.GetAIOwner());
        if (CharacterAIController)
        {
            APawn* AIPawn = CharacterAIController->GetPawn();
            AICharacter = Cast<ACharacterBase>(AIPawn);
            if (AICharacter)
            {
                GameMode = Cast<ARTSGameMode>(GetWorld()->GetAuthGameMode());

                ETeam = AICharacter->GetTeam();
                OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ETeams", (uint8)ETeam);
                PlayerOrAIState = AICharacter->GetPlayerOrAIState();
                OwnerComp.GetBlackboardComponent()->SetValueAsEnum("EPlayerOrAI", (uint8)PlayerOrAIState);
                CharacterState = AICharacter->GetCharacterState();
                StateBeforeCombat = CharacterState;
                OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                CharacterType = AICharacter->GetCharacterType();
                OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterType", (uint8)CharacterType);

                OwnerComp.GetBlackboardComponent()->SetValueAsObject("SelfActor", AICharacter);

                bValuesSet = true;
                bInCombat = false;               
            }
        }
    }

    if (AICharacter)
    {
        // SET THE TEAM
        if (AICharacter->Tags.Num() > 0)
        {
            TeamTag = AICharacter->Tags[0]; // GET THE TAG
        }

        if (AICharacter->GetIsDead())
        {
            SetDeadState(); // DISABLE AI
            OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
            return;
        }

        // PLAYER CONTROLLED AI
        /////////////////////////////////
        if (AICharacter->GetTeam() == ETeams::ET_Team01)
        {
            if (IsValid(AICharacter->GetEnemyActor()))
            {
                if (AICharacter->GetCharacterState() == ECharacterState::ECS_PlayerControlledCombat
                    || AICharacter->GetCharacterState() == ECharacterState::ECS_PlayerControlledMoving)
                {

                float EnemyDistance = CalculateEnemyDistance();
                if (EnemyDistance <= AICharacter->GetFiringDistance())

                {
                    SetCombatState(); // If a sensed enemy exists, set the combat state

                    // Stop Player Movement
                    UAIBlueprintHelperLibrary::SimpleMoveToLocation(AICharacter->GetController(), AICharacter->GetActorLocation());
                    UE_LOG(LogTemp, Warning, TEXT("STOPPING MOVEMENT"));

                    // Update the behavior tree
                    OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                    OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", AICharacter->GetEnemyActor());
                    return;
                }
                else if (EnemyDistance > AICharacter->GetFiringDistance())
                {
                    SetECS_PlayerControlledMoving();
                    UE_LOG(LogTemp, Warning, TEXT("Enemy Distance greater than firing range"));

                    OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                    OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", AICharacter->GetEnemyActor());
                    return;
                }
            }
        }
            if (!IsValid(AICharacter->GetEnemyActor()))
            {
                if (AICharacter->GetCharacterState() != ECharacterState::ECS_SpawnedBarracks)
                {
                    SetECS_PlayerControlledMoving();
                    OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                }

            }

        }
        // PLAYER AI END
        ////////////////////////////////////////



        ////////////////////////////////////////
        // TEAM 2 START
        if (AICharacter->GetTeam() == ETeams::ET_Team02)
        {
            if (!IsValid(AICharacter->GetEnemyActor()))
            {

                if (AICharacter->GetSquadData().SquadArray.IsValidIndex(0)) // Check if the new squad has been instantiated
                {
                    SetPatrollingState();
                    OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                }
            }

            if (IsValid(AICharacter->GetEnemyActor()))
            {
                UE_LOG(LogTemp, Warning, TEXT("Enemy valid"));
                float EnemyDistance = CalculateEnemyDistance();
                if (EnemyDistance > AICharacter->GetFiringDistance())
                {
                    SetAttackingState();
                    OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", AICharacter->GetEnemyActor());
                    OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                    return;
                }
                else if (EnemyDistance < AICharacter->GetFiringDistance())
                {
                    SetCombatState();

                    OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", AICharacter->GetEnemyActor());
                    OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                    return;
                }

                if (!IsValid(AICharacter->GetEnemyActor()))
                {
                    SetPreviousState();
                    OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                }
            }
        }
        ////// TEAM 02 END
        //////////////////////////////////////////



        ////   SEARCH FOR ENEMY
        /////////////////////////////////////
        TArray<AActor* > ignoreActors;
        TArray<AActor*> outActors;
        TArray<ACharacterBase* > CharactersFound;
        FVector SelfLocation = AICharacter->GetActorLocation();

        TArray< ACharacterBase*> FoundCharacterArray;
        TArray< ARTSStructure*> FoundStructureArray;


        if (!IsValid(AICharacter->GetEnemyActor()))
        {

            // TRACE FOR ENEMIES
            TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
            traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
            traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));  
            
            UKismetSystemLibrary::SphereOverlapActors
            (AICharacter->GetWorld(), SelfLocation, 2000.0f, traceObjectTypes, nullptr, ignoreActors, outActors);

            if (outActors.Num() > 0)
            {
                for (int32 i = 0; i < outActors.Num(); i++)
                {
                    if (outActors[i]->Tags.Num() > 0) // Ensure found actor has assigned tag
                    {
                        if (TeamTag != outActors[i]->Tags[0]) // Filter for different team tag
                        {

                            if (AICharacter->GetTeam() == ETeams::ET_Team01) // Team one needs to stop movement of player assigned locations
                            {
                                if (!AICharacter->GetSelected())                               
                                {
                                    // Set Target To Found Enemy
                                    AICharacter->SetEnemyActor(outActors[i]);
                                    OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", outActors[i]);

                                    // Stop Player Movement
                                    UAIBlueprintHelperLibrary::SimpleMoveToLocation(AICharacter->GetController(), AICharacter->GetActorLocation());

                                    // Set States To Combat
                                    SetCombatState();
                                    OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);

                                    EnemyCharacterType = ECharacterType::ECT_Soldier;
                                    AICharacter->SetEnemyCharacterType(EnemyCharacterType);
                                    return;
                                    break;
                                }
                            }

                            if (AICharacter->GetTeam() == ETeams::ET_Team02)
                            {
                                // Set Target To Found Enemy
                                AICharacter->SetEnemyActor(outActors[i]);
                                OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", outActors[i]);

                                // Set States To Combat
                                SetCombatState();
                                OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);

                                EnemyCharacterType = ECharacterType::ECT_Soldier;
                                AICharacter->SetEnemyCharacterType(EnemyCharacterType);
                                return;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

// HELPER FUNCTIONS
void UBTService_AIChar::GetFoundCharactersAndStructures(TArray<AActor*> FoundChars, TArray<ACharacterBase*>& FoundCharacterArray, TArray<ARTSStructure*>& FoundStructureArray)
{
    TArray<AActor*> outActors = FoundChars;
    // Filter Actors based on type
    if (outActors.Num() > 0)
    {
        for (int32 i = 0; i < outActors.Num(); i++)
        {
            ACharacterBase* OtherCharacter = Cast<ACharacterBase>(outActors[i]);
            if (OtherCharacter)
            {
                FoundCharacterArray.Add(OtherCharacter); // Character Array
            }
        }
        for (int32 i = 0; i < outActors.Num(); i++)
        {
            ARTSStructure* OtherStructure = Cast<ARTSStructure>(outActors[i]);
            if (OtherStructure)
            {
                FoundStructureArray.Add(OtherStructure); // Structure Array
            }
        }
    }
}


float UBTService_AIChar::CalculateEnemyDistance()
{
    if (AICharacter->GetEnemyActor())
    {
        FVector EnemyActorLocation = AICharacter->GetEnemyActor()->GetActorLocation();
        AIFiringRange = AICharacter->GetPlayerControlledFiringDistance();

        float EnemyDistance = (AICharacter->GetActorLocation() - EnemyActorLocation).Size();
        return EnemyDistance;
    }
    return 0.0;
}



void UBTService_AIChar::SetBarracksState()
{
    bInCombat = false;
    AICharacter->SetEnemySpotted(false);
    AICharacter->SetCharacterState(ECharacterState::ECS_SpawnedBarracks);
    CharacterState = AICharacter->GetCharacterState();

    AICharacter->EndTimersAndFireAnimations();
    AICharacter->SetEnemySpotted(false);
    AICharacter->SetFireWeapon(false);

    StateBeforeCombat = CharacterState;
}

void UBTService_AIChar::SetIdleState()
{
    bInCombat = false;
    AICharacter->SetEnemySpotted(false);

    AICharacter->SetCharacterState(ECharacterState::ECS_Idle);
    CharacterState = AICharacter->GetCharacterState();

    AICharacter->EndTimersAndFireAnimations();
    AICharacter->SetEnemySpotted(false);
    AICharacter->SetFireWeapon(false);
    StateBeforeCombat = CharacterState;
    
}

void UBTService_AIChar::SetAttackingState()
{
    bInCombat = true;
    AICharacter->EndTimersAndFireAnimations();
    EnemyActor = AICharacter->GetEnemyActor();
    AICharacter->SetCharacterState(ECharacterState::ECS_Attacking);
    CharacterState = AICharacter->GetCharacterState();

}

void UBTService_AIChar::SetPlayerControlledCombat()
{
    EnemyActor = AICharacter->GetEnemyActor();
    bInCombat = true;
    
    AICharacter->SetCharacterState(ECharacterState::ECS_PlayerControlledCombat);
    CharacterState = AICharacter->GetCharacterState();
}

void UBTService_AIChar::SetECS_PlayerControlledMoving()
{
    /*AICharacter->SetEnemyActor(nullptr);*/
    /*bInCombat = false;*/
    AICharacter->SetCharacterState(ECharacterState::ECS_PlayerControlledMoving);
    CharacterState = AICharacter->GetCharacterState();
}

void UBTService_AIChar::SetCombatState()
{
    bInCombat = true;
    EnemyActor = AICharacter->GetEnemyActor();
    AICharacter->SetCharacterState(ECharacterState::ECS_InCombat);
    CharacterState = AICharacter->GetCharacterState();
}


void UBTService_AIChar::SetDeadState()
{
    AICharacter->SetEnemySpotted(false);
    AICharacter->SetCharacterState(ECharacterState::ECS_Dead);
    bCharacterIsDead = true;
    AICharacter->EndTimersAndFireAnimations();
    CharacterState = AICharacter->GetCharacterState();
}

void UBTService_AIChar::SetPatrollingState()
{
    bInCombat = false;
    AICharacter->SetFireWeapon(false);
    AICharacter->EndTimersAndFireAnimations();
    AICharacter->SetCharacterState(ECharacterState::ECS_Patrolling);
    CharacterState = AICharacter->GetCharacterState();
    StateBeforeCombat = CharacterState;
}


void UBTService_AIChar::SetAdvancingState()
{
    bInCombat = false;
    AICharacter->SetFireWeapon(false);
    AICharacter->EndTimersAndFireAnimations();
    AICharacter->SetCharacterState(ECharacterState::ECS_Advancing);
    CharacterState = AICharacter->GetCharacterState();
    StateBeforeCombat = CharacterState;
}

void UBTService_AIChar::SetPreviousState()
{
    bInCombat = false;
    AICharacter->SetEnemySpotted(false);
    AICharacter->SetEnemyActor(nullptr);
    AICharacter->EndTimersAndFireAnimations();
    AICharacter->SetCharacterState(StateBeforeCombat);
    CharacterState = StateBeforeCombat;
    AICharacter->SetFireWeapon(false);
}

float UBTService_AIChar::CalculateMoveToDistance()
{
    FVector MoveLocation = AICharacter->GetPlayerControlledMoveLocation();
    float MoveToDistance = (AICharacter->GetActorLocation() - MoveLocation).Size();
    return MoveToDistance;
}

//if (AICharacter->GetTeam() == ETeams::ET_Team01)
//{
//    traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_EngineTraceChannel2));
//}
//if (AICharacter->GetTeam() == ETeams::ET_Team02)
//{
//    traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_EngineTraceChannel1));
//}