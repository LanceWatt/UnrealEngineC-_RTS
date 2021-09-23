// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../EnumLibrary.h"
#include "BuildManagingActor.generated.h"

UCLASS()
class CPP_RTS_API ABuildManagingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildManagingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



private:

	UPROPERTY()
	int32 UnitCost;

	UPROPERTY()
	int32 VehicleCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	int32 StructureCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	int32 StructureDeductionAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	int32 UnitDeductionAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	int32 VehicleDeductionAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	int32 StructurePowerUsage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	int32 Credits;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	ETeams ETeam;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EPlayerOrAI PlayerOrAIState;

	UPROPERTY()
	FTimerHandle StructureTimerHandler;

	UPROPERTY()
	FTimerHandle UnitTimerHandler;


	UPROPERTY()
	FTimerHandle VehicleTimerManager;


	UPROPERTY()
	FTimerHandle PauseUnitBuildHandler;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	class ARTSCameraPawn* CameraPawnReference;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	class ABaseBuildingAI* BaseBuildingAI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CreditManagement, meta = (AllowPrivateAccess = "true"))
	float UnitBuildSpeed;

	bool UnitBuildPressed;

	bool VehicleBuildPressed;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	void SetTeam(ETeams Team);

	void SetPlayerOrAIState(EPlayerOrAI PlayOrAI);

	UFUNCTION()
	void SetStructureDeductionsForPlayer(int32 Cost, int32 Deduction);

	UFUNCTION()
	void BuildStructurePlayer();

	UFUNCTION()
	void SetPlayerCameraPawnReference(class ARTSCameraPawn* CameraPawnRef);

	UFUNCTION()
	void SetAIBuilderReference(class ABaseBuildingAI* BaserBuildingAIRef);

	UFUNCTION(BlueprintCallable)
	void PlayerSetUnitDeductions(int32 Cost, int32 UnitDeductions);

	UFUNCTION(BlueprintCallable)
	void PlayerSetVehicleDeductions(int32 Cost, int32 VehicleDeductions);


	UFUNCTION()
	void PlayerBuildUnit();

	UFUNCTION()
	void PlayerBuildVehicle();

	UFUNCTION()
	void AISetUnitDeductions(int32 Cost, int32 UnitDeduction);

	UFUNCTION()
	void AIBuildUnit();

	FORCEINLINE int32 GetCredits() const { return Credits; }

	UFUNCTION()
	void AddCredits(int32 CreditsToAdd);

	UFUNCTION()
	void SubtractCredits(int32 CreditsToASubtract);

};
