//// Fill out your copyright notice in the Description page of Project Settings.
//
//
#include "BaseBuildingAI.h"
#include "Kismet/GameplayStatics.h"
#include "RTSStructure.h"
#include "../Character/CharacterBase.h"
#include "../Character/Character_Harvester.h"
#include "BuildManagingActor.h"
#include "../RTSGameMode.h"


// Sets default values
ABaseBuildingAI::ABaseBuildingAI()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("USceneComponent"));
	RootComponent = RootSceneComponent;

	SpawnHQ = CreateDefaultSubobject<UBillboardComponent>(TEXT("SpawnHQ"));
	SpawnHQ->SetupAttachment(RootSceneComponent);

	SpawnPowerPlant = CreateDefaultSubobject<UBillboardComponent>(TEXT("SpawnPowerPlant"));
	SpawnPowerPlant->SetupAttachment(RootSceneComponent);

	SpawnBarracks = CreateDefaultSubobject<UBillboardComponent>(TEXT("SpawnBarracks"));
	SpawnBarracks->SetupAttachment(RootSceneComponent);

	SpawnRefinery = CreateDefaultSubobject<UBillboardComponent>(TEXT("SpawnRefinery"));
	SpawnRefinery->SetupAttachment(RootSceneComponent);

	StructureRotation = (FRotator(0.0f, 90.0f, 0.0f));
	AssaultRifleCost = 100;
	TotalCredits = 300;
	UnitBuildSpeed = 5;
	bBuildingUnit = false;
	ETeam = ETeams::ET_Team02;
	PlayerOrAIState = EPlayerOrAI::EPA_AIControlled;

	UnitBuildPauseTime = 10.f;

	BeginSpawnAmount = 6;

}

void ABaseBuildingAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBuildingAI::BeginPlay()
{
	Super::BeginPlay();
	CurrentCredits = TotalCredits;

	GameMode = Cast<ARTSGameMode>(GetWorld()->GetAuthGameMode());

	SpawnHarvester();

	UnitBuildTimerManager();




	for (int32 i = 0; i < BeginSpawnAmount; i++)
	{
		SpawnUnit();
	}


	// Start building Units and Structures on Timer
	//GetWorldTimerManager().SetTimer
	//(BaseBuildingTimer,
	//	this,
	//	&ABaseBuildingAI::AIBaseBuilding,
	//	1.f,
	//	true);
}

//	 BUILD UNITS FROM BARRACKS AND FACTORIES
void ABaseBuildingAI::AIBaseBuilding()
{
		UnitBuild();
}

void ABaseBuildingAI::UnitBuildTimerManager()
{
	GetWorldTimerManager().SetTimer
	(TimerHandleClock,
	this,
	&ABaseBuildingAI::UnitBuild,
	15.f,
	true);

}

// THE BUILD MANAGER HANDLES AI ECONOMY, CREDIT INTAKES, DEDUCTIONS
void ABaseBuildingAI::UnitBuild()
{
	if (!bBuildingUnit)
	{
		bBuildingUnit = true;
		if (BuildManagerReference)
		{
			BuildManagerReference->AISetUnitDeductions(AssaultRifleCost, UnitBuildSpeed);
		}
	}

		
}


// THIS DETERMINES EVERY SPAWNED ACTOR'S TEAM
void ABaseBuildingAI::SetTeam(ETeams Team)
{
	ETeam = Team;
}

// USEFUL FOR AI BEHAVIOR TREES
void ABaseBuildingAI::SetPlayerOrAIState(EPlayerOrAI PlayOrAIState)
{
	PlayerOrAIState = PlayOrAIState;
}


// RECEIVE THE CLASSES AND ASSIGN AS REQUIRED TO SPAWN UNITS. THE CLASSES ARE PASSED FROM THIS BLUEPRINT SUBCLASS
void ABaseBuildingAI::ReceiveStructureSubclasses(TSubclassOf<ARTSStructure> HQRef, TSubclassOf<ARTSStructure> PowerPlantRef, TSubclassOf<ARTSStructure> BarracksRef, TSubclassOf<ARTSStructure> RefineryRef)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (HQRef)
	{
		HQClassReference = HQRef;
		HQReference = GetWorld()->SpawnActor<ARTSStructure>(HQClassReference, SpawnHQ->GetComponentLocation(), StructureRotation, SpawnInfo);
		HQReference->SetTeam(ETeam);
		HQReference->SetTeamTag(ETeam);
	}

	if (PowerPlantRef)
	{
		PowerPlantClassReference = PowerPlantRef;
		PowerPlantReference = GetWorld()->SpawnActor<ARTSStructure>(PowerPlantClassReference, SpawnPowerPlant->GetComponentLocation(), StructureRotation, SpawnInfo);
		PowerPlantReference->SetTeam(ETeam);
		PowerPlantReference->SetTeamTag(ETeam);
	}

	if (BarracksRef)
	{
		BarracksClassReference = BarracksRef;
		BarracksReference = GetWorld()->SpawnActor<ARTSStructure>(BarracksClassReference, SpawnBarracks->GetComponentLocation(), StructureRotation, SpawnInfo);
		BarracksReference->SetTeam(ETeam);
		BarracksReference->SetTeamTag(ETeam);
	}
	if (RefineryRef)
	{
		RefineryClassReference = RefineryRef;
		RefineryReference = GetWorld()->SpawnActor<ARTSStructure>(RefineryClassReference, SpawnRefinery->GetComponentLocation(), StructureRotation, SpawnInfo);
		RefineryReference->SetTeam(ETeam);
		RefineryReference->SetTeamTag(ETeam);
	}
}

// BUILDING MANAGER ACTS AS AN ACTOR COMPONENT. USED BY ALL TEAMS INCLUDING PLAYER TO MANAGE UNIT AND STRUCTURE BUILD SPEEDS,
// CREDIT INTAKE AND DEDUCTIONS. DISABLES BUILDING IF CREDITS APPROACH ZERO.
void ABaseBuildingAI::ReceiveBuildManagerSubclasses(TSubclassOf<ABuildManagingActor> BuildingManagerClassRef)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	BuildManagerClass = BuildingManagerClassRef;
	BuildManagerReference = GetWorld()->SpawnActor<ABuildManagingActor>(BuildManagerClass, FVector(0.0, 0.0, 0.0), FRotator(0.0, 0.0, 0.0), SpawnInfo);
	BuildManagerReference->SetTeam(ETeams::ET_Team02);
	BuildManagerReference->SetPlayerOrAIState(EPlayerOrAI::EPA_AIControlled);
	BuildManagerReference->SetAIBuilderReference(this);
	BuildManagerReference->AddCredits(TotalCredits);
}

void ABaseBuildingAI::ReceiveCharacterSubclasses(TSubclassOf<ACharacterBase> CharacterRef)
{
	CharacterClass = CharacterRef;
}

void ABaseBuildingAI::ReceiveVehicleSubclasses(TSubclassOf<ACharacter_Harvester> HarvesterClassRef)
{
	HarvesterClass = HarvesterClassRef;
	
}


// BUILD UNITS
void ABaseBuildingAI::AI_UnitBuildTimerManager()
{
	if (!bBuildingUnit)
	{
		BuildManagerReference->AISetUnitDeductions(AssaultRifleCost, UnitBuildSpeed);
		bBuildingUnit = true;
	}
}

void ABaseBuildingAI::BaseBuildingEndUnitBuild()
{
	UE_LOG(LogTemp, Warning, TEXT("FinishingBuild "));
	
	SpawnUnit();

}

// SPAWNS THE BUILT AI INTO THE WORLD
void ABaseBuildingAI::SpawnUnit()
{
	if (BarracksReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Barracks Valid"));
		FVector BarracksSpawnLocation = BarracksReference->GetSpawnBillboardLoction();

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ACharacterBase* SpawnedCharacter = GetWorld()->SpawnActor<ACharacterBase>(CharacterClass, BarracksSpawnLocation, FRotator(0.0, 0.0, 0.0), SpawnInfo);

		if (SpawnedCharacter)
		{
			SpawnedCharacter->SetTeam(ETeams::ET_Team02);
			SpawnedCharacter->SetPlayerOrAIState(EPlayerOrAI::EPA_AIControlled);
			SpawnedCharacter->SetCharacterState(ECharacterState::ECS_SpawnedBarracks);
			SpawnedCharacter->SpawnDefaultController();
			SpawnedCharacter->SetBarracksReference(BarracksReference);
			SpawnedCharacter->SetCharacterType(ECharacterType::ECT_Soldier);
			SpawnedCharacter->SetTeamTag(ETeam);

			// Assign Group Leader
			AssignAIToSquads(SpawnedCharacter);
		}


		// Time between unit builds
		GetWorldTimerManager().SetTimer
		(PauseUnitBuildHandler,
			this,
			&ABaseBuildingAI::PauseTimerEnd,
			UnitBuildPauseTime,
			false);
	}
}

// MANAGE THE FLOW OF TIME BETWEEN UNIT BUILDS
void ABaseBuildingAI::PauseTimerEnd()
{
	bBuildingUnit = false;
	GetWorldTimerManager().ClearTimer(PauseUnitBuildHandler);
	UE_LOG(LogTemp, Warning, TEXT("Building Now"));
}

// HARVESTER GATHERS RESOURCES
void ABaseBuildingAI::SpawnHarvester()
{
	if (HarvesterClass)
	{
		UClass* ActorClass = HarvesterReference->GetClass();

		// Get Spawn Location
		FVector Location(0.0f, 0.0f, 0.0f);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		RefineryReference->GetHarvesterSpawnLocation(Location, Rotation);

		FRotator SpawnRotation(0.0f, 0.0f, 0.0f);

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ACharacter_Harvester* SpawnedVehicle = GetWorld()->SpawnActor<ACharacter_Harvester>(HarvesterClass, Location, SpawnRotation, SpawnInfo);

			SpawnedVehicle->SpawnDefaultController();
			SpawnedVehicle->SetHarvesterReferencesAI(RefineryReference, this);
			SpawnedVehicle->SetTeam(ETeam);
			SpawnedVehicle->SetPlayerOrAIState(PlayerOrAIState);
			SpawnedVehicle->SetTeamTag(ETeam);
	}
}




void ABaseBuildingAI::AssignAIToSquads(ACharacterBase* CharacterToAdd)
{
	ACharacterBase* CharacterToAssign = CharacterToAdd;

	

	// IMPLEMENT NESTED TARRAYS IN BLUEPRINT
	AddSquadsToStruct(CharacterToAssign, SpawnNumber);

	IncrementSpawnNumber(); // INREMENT CYCLES BY 6



	//if (SpawnNumber == 0)
	//{
	//	if (SpawnNumber <= 6) // 6 units to a squad
	//	{
	//		if (SpawnNumber / 6) // Assign the non dividing index to squad leader
	//		{
	//			SquadArray.Add(CharacterToAssign);
	//			CharacterToAssign->SetGroupLeader(true);
	//		}
	//		else
	//		{
	//			SquadArray.Add(CharacterToAssign);
	//		}
	//	}
	//}




}

void ABaseBuildingAI::IncrementSpawnNumber()
{
	SpawnNumber++;
	if (SpawnNumber >= 6)
	{
		SpawnNumber = 0;
	}
}



void ABaseBuildingAI::AssignSquadArray()
{

}


void ABaseBuildingAI::AssignSquadGroupArray()
{

}