// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"

#include "Components/ActorComponent.h"
#include "ProjectileCreationComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTFPS_API UProjectileCreationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectileCreationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category=Projectile)
	TSubclassOf<AProjectileBase> ProjectileClasse;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(Reliable,Server)
	void Server_CreateProjectile(AActor* ProjectileClass ,const FVector &MuzzleLocation , const FRotator &Direction );
	
	void CreateProjectile(AActor* ProjectileClass ,const FVector &MuzzleLocation,const FRotator &Direction);

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;	
};
