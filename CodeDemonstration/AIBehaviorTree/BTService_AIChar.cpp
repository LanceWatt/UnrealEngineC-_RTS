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
#include "GameFramework/CharacterMovementComponent.h"
#include "../Character/CharacterBase.h"
#include "../GameActors/RTSStructure.h"
#include "DrawDebugHelpers.h"


/*   This service updates the state of the AI Character and set the states in the behavior tree.
     The behaviors are determined by changes to values triggered in the ACharacterBase and RTSPlayerController classes,
     as well as AI population conditions.*/

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

        AILocation = AICharacter->GetActorLocation();


        // PLAYER CONTROLLED AI
        /////////////////////////////////
        if (AICharacter->GetTeam() == ETeams::ET_Team01)
        {
            if (IsValid(AICharacter->GetEnemyActor()))
            {
                OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", (EnemyActor));
            }

            //  COMBAT INSTRUCTION
            if (AICharacter->GetPlayerMoveCombatInstruction())
            {
                AICharacter->SetCharacterState(ECharacterState::ECS_PlayerControlledCombat);
                float ControlledCombatDistance = (AILocation - AICharacter->GetPlayerControlledCombatLocation()).Size();
                if (AICharacter->GetEnemyActor())
                {
                    float EnemyDist = (AILocation - AICharacter->GetEnemyActor()->GetActorLocation()).Size();

                    // Get the lateral speed of the Character
                    FVector Velocity{ AICharacter->GetVelocity() };
                    Velocity.Z = 0;
                    float Speed = Velocity.Size();

                    if (EnemyDist <= 2500.0 && Speed < 100.f)
                    {
                        // Set Target To Found Enemy         
                        EnemyActor = AICharacter->GetEnemyActor();
                        OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", (EnemyActor));

                        //// Stop Player Movement
                        //UAIBlueprintHelperLibrary::SimpleMoveToLocation(AICharacter->GetController(), AICharacter->GetActorLocation());

                        AICharacter->SetMoveCombatInstruction(false);

                        // Set States To Combat
                        AICharacter->SetCharacterState(ECharacterState::ECS_PlayerControlledCombatInRange);
                        CharacterState = AICharacter->GetCharacterState();
                        bInCombat = true;
                        OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                        return;

                    }
                    if (EnemyDist > 2500.f)
                    {
                        // Set Target To Found Enemy         
                        EnemyActor = AICharacter->GetEnemyActor();
                        OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", (EnemyActor));

                        // Set States To Combat
                        AICharacter->SetCharacterState(ECharacterState::ECS_Advancing);
                        CharacterState = AICharacter->GetCharacterState();
                        bInCombat = true;

                        OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);

                        return;

                    }
                }
            }
            if (AICharacter->GetPlayerMoveToInstruction())
            {
                AICharacter->SetMoveLocationInstruction(false);

                float ControlledMoveToDistance = (AILocation - AICharacter->GetPlayerControlledMoveLocation()).Size();

                // Get the lateral speed of the Character
                FVector Velocity{ AICharacter->GetVelocity() };
                Velocity.Z = 0;
                float Speed = Velocity.Size();

                if (AICharacter->GetEnemyActor() && Speed < 100.f)
                {

                    UE_LOG(LogTemp, Warning, TEXT("Speed less thatn 100, enemy actor set. Now in Combat"));
                    // Set Target To Found Enemy         
                    EnemyActor = AICharacter->GetEnemyActor();
                    OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", (EnemyActor));

                    // Set States To Combat
                    AICharacter->SetCharacterState(ECharacterState::ECS_InCombat);
                    CharacterState = AICharacter->GetCharacterState();
                    bInCombat = true;
                    OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                    EnemyCharacterType = ECharacterType::ECT_Soldier;
                    return;
                }
                else
                {
                    SetECS_PlayerControlledMoving();
                    UE_LOG(LogTemp, Warning, TEXT("Controlled Movement Active"));
                    OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);

                    AICharacter->SetMoveLocationInstruction(true);
                }
            }
        }

       
        ////////////////////////////////////////
        // TEAM 2 START
        if (AICharacter->GetTeam() == ETeams::ET_Team02)
        {
            if (!IsValid(AICharacter->GetEnemyActor()))
            {
                if (!AICharacter->GetIsPatrolUnit()) // Bypass navigation state if is a patrol unit
                {
                    if (AICharacter->GetSquadData().SquadArray.IsValidIndex(0)) // Check if the new squad has been instantiated. This Occurs when all AI spawned in squad.
                    {
                        UE_LOG(LogTemp, Warning, TEXT("In Advancing State"));
                        SetPatrollingState();
                        OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                    }
                }
                if (AICharacter->GetIsPatrolUnit())
                {
                    UE_LOG(LogTemp, Warning, TEXT("In Patrol State"));
                    if (GameMode->GetMinutes() >= AICharacter->GetMinutesToAttack())
                    {
                        SetPatrollingState();
                        OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                    }
                    else
                    {
                        SetIdleState();
                        OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);

                    }
                }

            }

            if (IsValid(AICharacter->GetEnemyActor()))
            {
                UE_LOG(LogTemp, Warning, TEXT("Enemy valid"));
                float EnemyDistance = CalculateEnemyDistance();
                if (EnemyDistance > 5000.f)
                {
                    SetPreviousState();
                    OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                }

                if (EnemyDistance >= 3000.f)
                {
                    SetAttackingState();
                    OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", AICharacter->GetEnemyActor());
                    OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);
                    return;
                }
                if (EnemyDistance < 3000.f)
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
        if (IsValid(AICharacter->GetEnemyActor()))
        {

            if (AICharacter->GetTeam() == ETeams::ET_Team01) // Team one needs to stop movement of player assigned locations
            {
                if (!AICharacter->GetSelected() && !AICharacter->GetPlayerMoveCombatInstruction())
                {
                    // Set Target To Found Enemy
                    AICharacter->SetEnemyActor(AICharacter->GetEnemyActor());
                    OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", AICharacter->GetEnemyActor());

                    // Stop Player Movement
                    UAIBlueprintHelperLibrary::SimpleMoveToLocation(AICharacter->GetController(), AICharacter->GetActorLocation());

                    // Set States To Combat
                    SetCombatState();
                    OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);

                    EnemyCharacterType = ECharacterType::ECT_Soldier;

                    return;
                }
            }

            if (AICharacter->GetTeam() == ETeams::ET_Team02)
            {                
                // Set Target To Found Enemy
                AICharacter->SetEnemyActor(AICharacter->GetEnemyActor());
                OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", AICharacter->GetEnemyActor());

                // Set States To Combat
                SetCombatState();
                OwnerComp.GetBlackboardComponent()->SetValueAsEnum("ECharacterState", (uint8)CharacterState);

                EnemyCharacterType = ECharacterType::ECT_Soldier;

                return;
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
    return 100000.f;
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

    AICharacter->SetCharacterState(ECharacterState::ECS_Idle);
    CharacterState = AICharacter->GetCharacterState();
    AICharacter->EndTimersAndFireAnimations();
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
    bInCombat = false;
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

void UBTService_AIChar::SetInRangeCombatState()
{
    bInCombat = true;
    EnemyActor = AICharacter->GetEnemyActor();
    AICharacter->SetCharacterState(ECharacterState::ECS_PlayerControlledCombatInRange);
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