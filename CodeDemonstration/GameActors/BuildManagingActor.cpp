// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildManagingActor.h"
#include "RTSCameraPawn.h"
#include "BaseBuildingAI.h"

// Sets default values
ABuildManagingActor::ABuildManagingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Credits = 0;
	StructureDeductionAmount = 2;
	UnitDeductionAmount = 4;
	UnitBuildSpeed = 0.15f;
	

	UnitBuildPressed = false;

}

// Called when the game starts or when spawned
void ABuildManagingActor::BeginPlay()
{
	Super::BeginPlay();
	
}



void ABuildManagingActor::AddCredits(int32 CreditsToAdd)
{
	Credits += CreditsToAdd;
	if (CameraPawnReference)
	{
		CameraPawnReference->UpdateCreditsForHUD(Credits);
	}
	
}

void ABuildManagingActor::SubtractCredits(int32 CreditsToASubtract)
{
	Credits -= CreditsToASubtract;
	if (CameraPawnReference)
	{
		CameraPawnReference->UpdateCreditsForHUD(Credits);

	}
}


// Called every frame
void ABuildManagingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABuildManagingActor::SetTeam(ETeams Team)
{
	ETeam = Team;
}


void ABuildManagingActor::SetPlayerOrAIState(EPlayerOrAI PlayOrAI)
{
	PlayerOrAIState = PlayOrAI;
}


void ABuildManagingActor::SetStructureDeductionsForPlayer(int32 Cost, int32 Deduction)
{
	if(!UnitBuildPressed)
	{
		StructureCost = Cost;
		StructureDeductionAmount = Deduction;
		StructureCost = Cost;
		GetWorldTimerManager().SetTimer
		(StructureTimerHandler,
			this,
			&ABuildManagingActor::BuildStructurePlayer,
			UnitBuildSpeed,
			true);
	}
	else
	{
		CameraPawnReference->CameraPawnCancelStructureBuild();
		
	}

}

void ABuildManagingActor::BuildStructurePlayer()
{

	if (StructureCost > 0)
	{
		if (Credits >= StructureDeductionAmount)
		{
			StructureCost -= StructureDeductionAmount;
			Credits -= StructureDeductionAmount;
			int32 CurrentCostStatus = StructureCost;
			
			CameraPawnReference->UpdateCreditsForHUD(Credits);

			CameraPawnReference->ReceiveDeductionsUpatesStructure(CurrentCostStatus, Credits);

		}
	}
	else
	{
		CameraPawnReference->UpdateCreditsForHUD(Credits);
		
		CameraPawnReference->CameraPawnEndStructureBuild();
		GetWorldTimerManager().ClearTimer(StructureTimerHandler);

	}
}

void ABuildManagingActor::SetPlayerCameraPawnReference(ARTSCameraPawn* CameraPawnRef)
{
	CameraPawnReference = CameraPawnRef;
}

void ABuildManagingActor::SetAIBuilderReference(ABaseBuildingAI* BaserBuildingAIRef)
{
	BaseBuildingAI = BaserBuildingAIRef;
}

void ABuildManagingActor::PlayerSetUnitDeductions(int32 Cost, int32 UnitDeductions)
{
	{
		UnitDeductionAmount = UnitDeductions;
		UnitCost = Cost;

		UE_LOG(LogTemp, Warning, TEXT("UNIT COST IS: %d"), UnitCost);
		UnitBuildPressed = true;
		GetWorldTimerManager().SetTimer
		(UnitTimerHandler,
			this,
			&ABuildManagingActor::PlayerBuildUnit,
			UnitBuildSpeed,
			true);
	}
}

void ABuildManagingActor::PlayerSetVehicleDeductions(int32 Cost, int32 VehicleDeductions)
{
	{
		VehicleDeductionAmount = VehicleDeductions;
		VehicleCost = Cost;
		GetWorldTimerManager().SetTimer
		(VehicleTimerManager,
			this,
			&ABuildManagingActor::PlayerBuildVehicle,
			UnitBuildSpeed,
			true);
	}
}


void ABuildManagingActor::PlayerBuildUnit()
{
	UnitBuildPressed = false;
	if (UnitCost > 0)
	{
		if (Credits >= UnitDeductionAmount)
		{
			UnitCost -= UnitDeductionAmount;
			Credits -= UnitDeductionAmount;
			int32 CurrentCostStatus = UnitCost;
			CameraPawnReference->BuildingProgressUnits(CurrentCostStatus);
			CameraPawnReference->ReceiveDeductionsUpatesUnit(CurrentCostStatus, Credits);
			CameraPawnReference->UpdateCreditsForHUD(Credits);
			
		}
	}
	else
	{

		GetWorldTimerManager().ClearTimer(UnitTimerHandler);
		CameraPawnReference->UpdateCreditsForHUD(Credits);
		CameraPawnReference->CameraPawnEndUnitBuild();
	}
}

void ABuildManagingActor::PlayerBuildVehicle()
{

	if (VehicleCost > 0)
	{
		if (Credits >= VehicleDeductionAmount)
		{
			VehicleCost -= VehicleDeductionAmount;
			Credits -= VehicleDeductionAmount;
			int32 CurrentCostStatus = VehicleCost;
			CameraPawnReference->BuildingProgressVehicles(CurrentCostStatus);
			CameraPawnReference->ReceiveDeductionsUpatesVehicles(CurrentCostStatus, Credits);
			CameraPawnReference->UpdateCreditsForHUD(Credits);

		}
	}
	else
	{

		GetWorldTimerManager().ClearTimer(VehicleTimerManager);
		CameraPawnReference->UpdateCreditsForHUD(Credits);
		CameraPawnReference->CameraPawnEndVehicleBuild();
	}
}



//	AI
void ABuildManagingActor::AISetUnitDeductions(int32 Cost, int32 UnitDeduction)
{
	{
		UnitCost = Cost;
		GetWorldTimerManager().SetTimer
		(UnitTimerHandler,
			this,
			&ABuildManagingActor::AIBuildUnit,
			UnitBuildSpeed,
			true);
	}
}

void ABuildManagingActor::AIBuildUnit()
{
	if (UnitCost > 0)
	{
		if (Credits >= UnitDeductionAmount)
		{
			UnitCost -= UnitDeductionAmount;
			Credits -= UnitDeductionAmount;
			/*UE_LOG(LogTemp, Warning, TEXT("Deducting from unit AI: %d"), UnitCost);*/
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unit Ready"));
		GetWorldTimerManager().ClearTimer(UnitTimerHandler);
		BaseBuildingAI->BaseBuildingEndUnitBuild();
	}
}

