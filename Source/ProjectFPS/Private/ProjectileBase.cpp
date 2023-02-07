// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "Components/HealthComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);

	
}

AProjectileBase::AProjectileBase() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
	bReplicates = true;

	CollisionComp->SetCanEverAffectNavigation(false);
}

void AProjectileBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT(" onoverlap begin"));
}



void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics	OtherComp->IsSimulatingPhysics()
	if ((OtherActor != nullptr) && (OtherActor != this ) && (OtherActor != GetOwner()) && (OtherComp != nullptr))
	{
		FVector ShotDirection;
		FPointDamageEvent DamageEvent(ProjectileDamage, Hit, ShotDirection, nullptr);
		
		UHealthComponent* HealthComponent = OtherActor->FindComponentByClass<UHealthComponent>();
		if (HealthComponent)
		{
			HealthComponent->TakeDamage(ProjectileDamage);
			Destroy();
			return;
		}
		//UGameplayStatics::ApplyPointDamage(OtherActor,10.f, ShotDirection,Hit, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);
		OtherActor->TakeDamage(ProjectileDamage, DamageEvent, nullptr, this);
		Destroy();
	}
}