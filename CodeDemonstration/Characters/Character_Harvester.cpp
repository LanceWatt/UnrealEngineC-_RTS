// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_Harvester.h"
#include "../GameActors/RTSCameraPawn.h"
#include "../GameActors/Crystals.h"
#include "../GameActors/RTSStructure.h"
#include "../GameActors/BuildManagingActor.h"
#include "../GameActors/BaseBuildingAI.h"

ACharacter_Harvester::ACharacter_Harvester()
{
	HarvesterState = EHarvesterStates::EHT_MoveToHarvest;

	bIsDestroyed = false;
	HarvestExtractionSpeed = 5;
	MineralLoadCurrent = 0;
	MineralLoadMax = 700;
}

void ACharacter_Harvester::StartMining(AActor* NearestMiningActor)
{
	NearestMine = NearestMiningActor;

	GetWorldTimerManager().SetTimer
	(ExctractMineralsTimer,
		this,
		&ACharacter_Harvester::StartMiningTimer,
		.1f,
		true);
}

void ACharacter_Harvester::StartMiningTimer()
{
	//UE_LOG(LogTemp, Warning, TEXT("Timer should be running: %d"));
	ACrystals* Crystal = Cast<ACrystals>(NearestMine);

	if (Crystal)
	{
		int32 TransferredAmount;

		// Still left in the tank
		if (MineralLoadCurrent < MineralLoadMax)
		{
			TransferredAmount = Crystal->TransferMineralsToHarvester(HarvestExtractionSpeed * 2);

			//UE_LOG(LogTemp, Warning, TEXT("MineralLoadCurrent: %d"), MineralLoadCurrent);

			if (TransferredAmount > 0)
			{
				MineralLoadCurrent += TransferredAmount; // Lets fill up the tank
			}
			else // If extracted amount is 0, then the Crystal is depleted
			{

				Crystal->Destroy(); // Destroy the crystal
				GetWorldTimerManager().ClearTimer(ExctractMineralsTimer);

				if (MineralLoadCurrent < MineralLoadMax) // Another check here on the tank
				{
					HarvesterState = EHarvesterStates::EHT_MoveToHarvest;
					// Behavior Tree Will Take it from here
				}
				else
				{
					HarvesterState = EHarvesterStates::EHT_MoveToRefinery;

				}

			}
		}
		else // Full now, move back to the Refinery
		{
			HarvesterState = EHarvesterStates::EHT_MoveToRefinery;
			GetWorldTimerManager().ClearTimer(ExctractMineralsTimer);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Crystal invalid: %d"));
	}
}

void ACharacter_Harvester::StartUnloadingTimer()
{
	GetWorldTimerManager().SetTimer
	(RefineryHandle,
		this,
		&ACharacter_Harvester::StartUnloading,
		.1f,
		true);
}

void ACharacter_Harvester::StartUnloading()
{

	// PLAYER REFERENCE
	if (CameraPawnReference)
	{
		if (MineralLoadCurrent > 0)
		{
			MineralLoadCurrent -= HarvestExtractionSpeed;			
			CameraPawnReference->GetBuildManagerReference()->AddCredits(HarvestExtractionSpeed);

			if (MineralLoadCurrent == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Unloading Completed"));

				GetWorldTimerManager().ClearTimer(RefineryHandle);
				HarvesterState = EHarvesterStates::EHT_MoveToHarvest;
			}
		}
	}

	// AI REFERENCE
	if(AIBaseBuilderRef)
	{
		if (MineralLoadCurrent > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("MineralLoadCurrent: %d"), MineralLoadCurrent);

			MineralLoadCurrent -= HarvestExtractionSpeed;

			AIBaseBuilderRef->GetBuildManagerReference()->AddCredits(HarvestExtractionSpeed);


			if (MineralLoadCurrent == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Unloading Completed"));

				GetWorldTimerManager().ClearTimer(RefineryHandle);
				HarvesterState = EHarvesterStates::EHT_MoveToHarvest;
			}
		}

	}
}

void ACharacter_Harvester::UpdateHealth(float Current, AActor* DamageCauser)
{
	CurrentHealth = Current;

	UE_LOG(LogTemp, Log, TEXT("structure health : %f"), CurrentHealth);

	if (!bIsDestroyed)
	{
		if (CurrentHealth <= 0)
		{
			bIsDestroyed = true;

			CurrentHealth = 0.f;
			GetWorldTimerManager().SetTimer
			(DestructionTimer,
				this,
				&ACharacter_Harvester::DestroyActor,
				3.0f,
				false);
		}
	}

}

void ACharacter_Harvester::DestroyActor()
{
	Destroy(false, true);
}

void ACharacter_Harvester::SetHarvesterState(EHarvesterStates HarvState)
{
	HarvesterState = HarvState;
}

void ACharacter_Harvester::SetHarvesterReferences(ARTSStructure* BoundToStructure, ARTSCameraPawn* CameraPawnRef)
{
	RefineryReference = BoundToStructure;
	CameraPawnReference = CameraPawnRef;
}

void ACharacter_Harvester::SetHarvesterReferencesAI(ARTSStructure* BoundToStructure, ABaseBuildingAI* BaseBuilderRef)
{
	RefineryReference = BoundToStructure;
	AIBaseBuilderRef = BaseBuilderRef;
}