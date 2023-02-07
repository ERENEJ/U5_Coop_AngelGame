// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI_FriendlyPointerStatueBase.h"
#include "AI/AI_CharacterBase.h"
#include "AI_EnemyChaserStatueBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFPS_API AAI_EnemyChaserStatueBase : public AAI_CharacterBase
{
	GENERATED_BODY()

	AAI_EnemyChaserStatueBase();
	
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* CollisionBox;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION()
	void OnBoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
public:
	void ActivateEnemyStatue();
	void ClearCurrentTargetPlayer(ACharacterBase* CharacterBase);
	bool IsThereAEyeContactWithPlayers();
	
	UFUNCTION(BlueprintCallable)
	void GetAllPlayerRefsFromLevel();

	UFUNCTION(BlueprintCallable)
	void AddTargetPlayerToChaserQueue(ACharacterBase* CharacterBase);

	UPROPERTY(EditAnywhere)
	TArray<ACharacterBase*> CharacterBaseArray;

	UPROPERTY()
	AAI_FriendlyPointerStatueBase* FriendlyPointerStatueRef;

	
	TArray<ACharacterBase*> KillListArray;

};
