// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "../EnumLibrary.h"
#include "../GameActors/BaseBuildingAI.h"
#include "BTService_AIChar.generated.h"

/**
 * 
 */

UCLASS()
class CPP_RTS_API UBTService_AIChar : public UBTService
{
	GENERATED_BODY()
	
public:
    UBTService_AIChar();

    /** update next tick interval
   * this function should be considered as const (don't modify state of object) if node is not instanced! */
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:

    UPROPERTY()
    class ACharacterBase* AICharacter;

	UPROPERTY(VisibleAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	class ACharacterBase* EnemyCharacter;

	UPROPERTY(VisibleAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	class AActor* EnemyActor;

	UPROPERTY(VisibleAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState;

	UPROPERTY(VisibleAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	ECharacterState StateBeforeCombat;

	UPROPERTY(VisibleAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	ETeams ETeam;

	UPROPERTY(VisibleAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	EPlayerOrAI PlayerOrAIState;

	UPROPERTY(VisibleAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	ECharacterType CharacterType;

	UPROPERTY(VisibleAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	ECharacterType EnemyCharacterType;

	UPROPERTY(VisibleAnywhere, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	FName TeamTag;

	UPROPERTY()
	FSquadData SquadData;

	UFUNCTION()
	void GetFoundCharactersAndStructures(TArray<AActor*> FoundChars, TArray<ACharacterBase*>& FoundCharacterArray, TArray<ARTSStructure*>& FoundStructureArray);

	UFUNCTION()
	void SetBarracksState();

	UFUNCTION()
	void SetIdleState();

	UFUNCTION()
	void SetAttackingState();

	UFUNCTION()
	void SetCombatState();

	UFUNCTION()
	void SetDeadState();

	UFUNCTION()
	void SetPatrollingState();

	UFUNCTION()
	void SetAdvancingState();

	UFUNCTION()
	void SetPreviousState();

	UFUNCTION()
	float CalculateEnemyDistance();

	UFUNCTION()
	void SetPlayerControlledCombat();
	
	UFUNCTION()
	void SetECS_PlayerControlledMoving();

	UFUNCTION()
	float CalculateMoveToDistance();

	UPROPERTY()
	class ARTSGameMode* GameMode;

	UPROPERTY()
	class ARTSStructure* EnemyStructure;

private:

	bool bEnemyFound;
	bool bCharacterIsDead;
	bool bEnemyWasPresent;
	bool bEnemyCharacterFound;
	bool bEnemyStructureFound;
	bool bInCombat;
	bool bEnemyPresent;
	bool bValuesSet;
	float AIFiringRange;

};
