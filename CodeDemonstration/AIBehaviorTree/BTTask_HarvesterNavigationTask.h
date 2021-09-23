// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "../EnumLibrary.h"
#include "BTTask_HarvesterNavigationTask.generated.h"

/**
 * 
 */
UCLASS()
class CPP_RTS_API UBTTask_HarvesterNavigationTask : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	

public:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	class ACharacter_Harvester* Harvester;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	EHarvesterStates HarvesterState;

	UFUNCTION()
	bool FindCrystals(AActor* &NearestCrystal);

	UFUNCTION()
	bool FindNearestActor(TArray<AActor*> &ActorArray, AActor* &NearestActor);


	UFUNCTION()
	void SetRefineryLocation();
	
	UPROPERTY()
	FVector UnloadLocation;

	UPROPERTY()
	float MeasureDistance;

	UPROPERTY()
	AActor* NearestMine;

	UPROPERTY()
	AActor* ClosestMine;
};
