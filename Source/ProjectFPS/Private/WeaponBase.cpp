// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "../../../Plugins/Developer/RiderLink/Source/RD/thirdparty/clsocket/src/StatTimer.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//Definition for the Mesh that will serve as our visual representation.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	//Set the Static Mesh and its position/scale if we successfully found a mesh asset to use.
	if (DefaultMesh.Succeeded())
	{
		//SkeletalMesh->SetSkeletalMesh(DefaultMesh.);
		SkeletalMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -37.5f));
		SkeletalMeshComponent->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(SkeletalMeshComponent);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);
	GunOffset = FVector::ZeroVector;
	
	DamageType = UDamageType::StaticClass();
	Damage = 10.0f;

}



// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AWeaponBase::Server_StartFireWeapon_Implementation()
{
	ACharacterBase* CharacterBase = Cast<ACharacterBase>(GetOwner());
	if(CharacterBase)
	{
		FireBullet();	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Firebullet at weaponbase is not valid needed for location of shot direction (OWNER problem)"));
	}
}

void AWeaponBase::HandleStartFireWeapon()
{
	
	if(!WeaponCanFireTimerHandle.IsValid())
	{

		if(GetNetMode() == ENetMode::NM_Client)
		{
			//for initial fire
			Server_StartFireWeapon();
		
			TimerDelegate.BindUFunction(this, TEXT("Server_StartFireWeapon"));
			GetWorldTimerManager().SetTimer(FireRateTimerHandle, TimerDelegate, 0.7f, true);
		}
		else
		{
			FireBullet();
			TimerDelegate.BindUFunction(this, "FireBullet");
			GetWorldTimerManager().SetTimer(FireRateTimerHandle, TimerDelegate, 0.7f, true);

		}
	}

}

void AWeaponBase::HandleStopFireWeapon()
{

	if(GetNetMode() == ENetMode::NM_Client)
	{
		//CharacterBaseThatFiresTheWeapon->Server_StopFiringWeapon(CharacterBaseThatFiresTheWeapon);
		StopFiringBullet();
	}
	else
	{
		StopFiringBullet();
	}
}

void AWeaponBase::StopFiringBullet()
{

	// need to get this line only once
	if(!bPressedBefore || FireRateTimerHandle.IsValid())
	{
		float RemainingTime = GetWorldTimerManager().GetTimerRemaining(FireRateTimerHandle);
		GetWorldTimerManager().SetTimer(WeaponCanFireTimerHandle, this, &AWeaponBase::EnableWeaponFire, RemainingTime, false);
	}
	bPressedBefore = true;

	GetWorld()->GetTimerManager().ClearTimer(FireRateTimerHandle);
	TimerDelegate.GetHandle().Reset();
	TimerDelegate.Unbind();
	FireRateTimerHandle.Invalidate();
	
}


void AWeaponBase::EnableWeaponFire()
{
	bPressedBefore = false;
	WeaponCanFireTimerHandle.Invalidate();
}

void AWeaponBase::FireBullet()
{
	UWorld* World = GetWorld();
	if (World)
	{
		ACharacterBase* CharacterBase = Cast<ACharacterBase>(GetOwner());

		if(CharacterBase)
		{
			FRotator const WeaponDirection = CharacterBase->GetCurrentCameraComponent()->GetForwardVector().Rotation();
			FVector CurrentWeaponMuzzleLocation = GetMuzzleLocationOfWeapon();
			FActorSpawnParameters ActorSpawnParams;
			//ActorSpawnParams.Owner = CharacterBaseThatFiresTheWeapon;
			ActorSpawnParams.Owner = GetOwner();
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			//TODO auto can be replace by weapon config projectile class
			auto* Projectile = World->SpawnActor<AProjectileBase>(GetWeaponDataStruct().ProjectileClass, CurrentWeaponMuzzleLocation,WeaponDirection, ActorSpawnParams);
			//Projectile->ProjectileDamage = GetWeaponDataStruct().WeaponDamage;
		
			/*
			 *TODO Hardcoded cheap recoil values needs to be dynamic per weapon
			//AddControllerPitchInput(-0.15f);
			//AddControllerYawInput(-0.040f);
			*/
			bPressedBefore = false;
		}

	}
	
	/*
			// try and play the sound if specified
			if (GetWeaponDataStruct().FireSound != nullptr)
			{
			//UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			// try and play a firing animation if specified
			if (GetWeaponDataStruct().FireAnimation != nullptr  )
			{
			// Get the animation object for the arms mesh
		
			UAnimInstance* AnimInstance = GetMesh1P()->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
			}
			*/

}

// --------------------------------------------     HELPER METHODS ---------------------------------------------------
USkeletalMesh* AWeaponBase::GetSkeletalMeshOfWeapon() 
{
	return SkeletalMeshComponent->SkeletalMesh;
}

FWeaponData AWeaponBase::GetWeaponDataStruct()
{
	return WeaponDataStruct;
}

FVector AWeaponBase::GetMuzzleLocationOfWeapon()
{
	return ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation());
}

