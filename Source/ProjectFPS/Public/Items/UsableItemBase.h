// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlaceableItemBase.h"
#include "GameFramework/Actor.h"
#include "UsableItemBase.generated.h"

UCLASS()
class PROJECTFPS_API AUsableItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUsableItemBase();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Voodoo Variables")
	bool bIsPlaced;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Voodoo Variables")
	bool bSpawnAllowed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Voodoo Variables")
	FRotator VoodooSpawnRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Voodoo Variables")
	FVector VoodooSpawnLocation;

	UPROPERTY(EditAnywhere, Category="Voodoo Variables")
	TSubclassOf<APlaceableItemBase> PlaceableItem;
	
	UFUNCTION(Reliable,Server)
	void Server_PlaceVoodooDoll(const FVector& Location, const FRotator& Rotation);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	 USkeletalMeshComponent* SkeletalMeshComponent;

};
