// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "../EnumLibrary.h"
#include "Character_Harvester.generated.h"

/**
 * 
 */
UCLASS()
class CPP_RTS_API ACharacter_Harvester : public ACharacterBase
{
	GENERATED_BODY()

protected:



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	EHarvesterStates HarvesterState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Harvesting, meta = (AllowPrivateAccess = "true"))
	class ARTSStructure* RefineryReference;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Harvesting, meta = (AllowPrivateAccess = "true"))
	ARTSCameraPawn* CameraPawnReference;

	FTimerHandle ExctractMineralsTimer;

	UPROPERTY()
	AActor* NearestMine;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	int32 MineralLoadCurrent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	int32 MineralLoadMax;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	int32 HarvestExtractionSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class ABaseBuildingAI* AIBaseBuilderRef;

	bool bIsDestroyed;

	FTimerHandle DestructionTimer;

public:

	ACharacter_Harvester();

	FORCEINLINE EHarvesterStates GetHarvesterState() const { return HarvesterState; }

	void SetHarvesterState(EHarvesterStates HarvState);
	
	UFUNCTION()
	void SetHarvesterReferences(ARTSStructure* BoundToStructure, ARTSCameraPawn* CameraPawnRef);

	UFUNCTION()
	void SetHarvesterReferencesAI(ARTSStructure* BoundToStructure, ABaseBuildingAI* BaseBuilderRef);

	FTimerHandle RefineryHandle;

	UFUNCTION()
	void StartMining(AActor* NearestMiningActor);

	UFUNCTION()
	void StartMiningTimer();

	UFUNCTION()
	void StartUnloadingTimer();

	UFUNCTION()
	void StartUnloading();

	UFUNCTION()
	virtual	void UpdateHealth(float Current, AActor* DamageCauser) override;



	UFUNCTION()
	void DestroyActor();

	FORCEINLINE ARTSStructure* GetRefineryReference() const { return RefineryReference; }
};
