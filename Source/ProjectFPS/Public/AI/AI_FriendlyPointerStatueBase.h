// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AI_CharacterBase.h"
#include "Items/TargetItemBase.h"
#include "AI_FriendlyPointerStatueBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFPS_API AAI_FriendlyPointerStatueBase : public AAI_CharacterBase
{
	GENERATED_BODY()

	AAI_FriendlyPointerStatueBase();
		
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual  void Tick(float DeltaSeconds) override;

public:

	FVector PreviousPlayerLocation;
	float tempDistanceHolder;

	UPROPERTY(Replicated)
	ATargetItemBase* CurrentTargetItem;

	UPROPERTY(Replicated)
	TArray<ACharacterBase*> CharacterBaseRefArray;

	// these methods were server methods if not works make it server method
	void GetAllPlayerRefsFromLevel();
	void StartStoringPlayerLocation();
	
	UFUNCTION(BlueprintCallable)
	void SetInitialValues();
	
	void FollowPlayerandTurnFaceToTarget();
	void FaceTowardsTargetItem();
	bool IsThereAEyeContactWithPlayers();
	void SetBaseDistanceToTargetItem();
	void SelectClosestTargetItem();
	void UpdateFriendlyAngel();
	
	
	
};
