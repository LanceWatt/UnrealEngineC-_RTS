// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RTSStructure.h"
#include "../EnumLibrary.h"
#include "BaseBuildingAI.generated.h"



// STRUCTS
USTRUCT(BlueprintType)
struct FSquadData
{

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squads")
	TArray<ACharacterBase* > SquadArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squads")
	int32 SquadNumber;

};


UCLASS()
class CPP_RTS_API ABaseBuildingAI : public AActor
{
	GENERATED_BODY()
	
public:	

	ABaseBuildingAI();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StructureReferences, meta = (AllowPrivateAccess = "true"))
	ETeams ETeam;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StructureReferences, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ARTSStructure> HQClassReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StructureReferences, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ARTSStructure> PowerPlantClassReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StructureReferences, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ARTSStructure> BarracksClassReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StructureReferences, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ARTSStructure> RefineryClassReference;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StructureReferences, meta = (AllowPrivateAccess = "true"))
	ARTSStructure* HQReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StructureReferences, meta = (AllowPrivateAccess = "true"))
	ARTSStructure* PowerPlantReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StructureReferences, meta = (AllowPrivateAccess = "true"))
	ARTSStructure* BarracksReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StructureReferences, meta = (AllowPrivateAccess = "true"))
	ARTSStructure* RefineryReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StructureReferences, meta = (AllowPrivateAccess = "true"))
	int32 BeginSpawnAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StructureReferences, meta = (AllowPrivateAccess = "true"))
	FRotator StructureRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Interface", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACharacterBase> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Interface", meta = (AllowPrivateAccess = "true"))
	class ACharacterBase* CharacterReference;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UBillboardComponent* SpawnHQ;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UBillboardComponent* SpawnPowerPlant;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UBillboardComponent* SpawnBarracks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UBillboardComponent* SpawnRefinery;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootSceneComponent;

	UFUNCTION(BlueprintCallable)
	void ReceiveStructureSubclasses(TSubclassOf<ARTSStructure> HQRef, TSubclassOf<ARTSStructure> PowerPlantRef, TSubclassOf<ARTSStructure> BarracksRef, TSubclassOf<ARTSStructure> RefineryRef);


	//UFUNCTION(BlueprintImplementableEvent)
	//void SetTeamParams();

	// Build Manager Reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	class ABuildManagingActor* BuildManagerReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ABuildManagingActor> BuildManagerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	EPlayerOrAI PlayerOrAIState;

	UPROPERTY()
		int32 SpawnNumber;

	UFUNCTION()
	void AssignAIToSquads(ACharacterBase* CharacterToAdd);

	UFUNCTION()
	void IncrementSpawnNumber();


	UFUNCTION(BlueprintImplementableEvent)
	void AddSquadsToStruct(ACharacterBase* CharacterToAdd, int32 CurrentIteration);

	// SQUAD ARRAYS 
// TODO - MAKE ARRAYS DYNAMIC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squads")
		TArray<ACharacter* > SquadArray_01;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squads")
		TArray<ACharacter* > SquadArray_02;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squads")
		TArray<ACharacter* > SquadArray_03;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squads")
		TArray<ACharacter* > SquadArray_04;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squads")
		TArray<ACharacter* > SquadArray_05;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squads")
		TArray<ACharacter* > SquadArray_06;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squads")
		TArray<ACharacter* > SquadArray_07;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squads")
		TArray<ACharacter* > SquadArray_08;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squads")
		TArray<ACharacter* > SquadArray_09;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squads")
		TArray<ACharacter* > SquadArray_010;


	TArray<FSquadData> SquadDataArray;

private:


	UPROPERTY()
	FTimerHandle BaseBuildingTimer;

	UPROPERTY()
	FTimerHandle TimerHandleClock;

	UPROPERTY()
	class ARTSGameMode* GameMode;
	

	UFUNCTION()
	void UnitBuildTimerManager();

	UFUNCTION()
	void UnitBuild();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class ACharacter_Harvester* HarvesterReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACharacter_Harvester> HarvesterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	int32 CurrentCredits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	int32 TotalCredits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	int32 StructureCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	int32 AssaultRifleCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	bool bBuildingUnit;

	UPROPERTY()
	FTimerHandle PauseUnitBuildHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	int32 UnitBuildSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	int32 StructureBuildSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
		int32 SquadNumber;

	// SQUAD GROUPS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TArray<ACharacter* > SquadArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TArray<ACharacter*> SquadGroupsArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	float UnitBuildPauseTime;

public:	

	virtual void Tick(float DeltaTime) override;

	// SET SQUAD GROUPS
	UFUNCTION()
	void AssignSquadArray();

	UFUNCTION()
	void AssignSquadGroupArray();



	UFUNCTION()
	void SetTeam(ETeams Team);

	UFUNCTION()
	void SetPlayerOrAIState(EPlayerOrAI PlayOrAIState);

	UFUNCTION()
	void BaseBuildingEndUnitBuild();

	UFUNCTION(BlueprintCallable)
	void ReceiveBuildManagerSubclasses(TSubclassOf<class ABuildManagingActor> BuildingManagerClassRef);

	UFUNCTION()
	void AI_UnitBuildTimerManager();

	UFUNCTION()
	void SpawnUnit();

	UFUNCTION()
	void AIBaseBuilding();

	UFUNCTION(BlueprintCallable)
	void ReceiveCharacterSubclasses(TSubclassOf<class ACharacterBase> CharacterRef);

	UFUNCTION(BlueprintCallable)
	void ReceiveVehicleSubclasses(TSubclassOf<ACharacter_Harvester> HarvesterClassRef);

	UFUNCTION()
	void PauseTimerEnd();

	UFUNCTION()
	void SpawnHarvester();

	FORCEINLINE class ABuildManagingActor* GetBuildManagerReference() const { return BuildManagerReference;  }


};
