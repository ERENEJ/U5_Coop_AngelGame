// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterBase.h"
#include "ProjectileBase.h"
#include "WeaponBase.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Projectile)
	TSubclassOf<AProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	float MagazineCapacity;

	UPROPERTY(EditAnywhere, Category=Projectile)
	UAnimMontage* ReloadAnim;
	
	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	
	/** defaults */
	FWeaponData()
	{
		ProjectileClass = nullptr;
		MagazineCapacity = 30;
		ReloadAnim = nullptr;
		FireSound = nullptr;
		FireAnimation = nullptr;
	
	}
};
UCLASS()
class PROJECTFPS_API AWeaponBase : public AActor
{
	GENERATED_BODY()

		
public:	
	// Sets default values for this actor's properties
	AWeaponBase();
	
	void HandleStartFireWeapon();
	void HandleStopFireWeapon();
	
	UFUNCTION()
	void FireBullet();

	void StopFiringBullet();

	//for traditional approach
	UFUNCTION(Reliable,Server)
	void Server_StartFireWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	USkeletalMesh* GetSkeletalMeshOfWeapon();
	FWeaponData GetWeaponDataStruct();
	FVector GetMuzzleLocationOfWeapon();

	FTimerHandle WeaponCanFireTimerHandle;
	FTimerHandle FireRateTimerHandle;
	FTimerDelegate TimerDelegate;
	bool bWeaponCanFire = true;
	bool bFireButtonPushedDown = false;

	void EnableWeaponFire();
	
	bool bPressedBefore = false;

	UPROPERTY(BlueprintReadWrite, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category=Config)
	FWeaponData WeaponDataStruct;

	// Sphere component used to test collision.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USphereComponent* SphereComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	USkeletalMeshComponent* SkeletalMeshComponent;

	// Movement component for handling projectile movement.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UProjectileMovementComponent* ProjectileMovementComponent;

	//The damage type and damage that will be done by this projectile
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;

	//The damage dealt by this projectile.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	float Damage;

	// Particle used when the projectile impacts against another object and explodes.
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* ExplosionEffect;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

};

