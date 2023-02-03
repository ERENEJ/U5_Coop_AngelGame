// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/UsableItemBase.h"

#include "CharacterBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AUsableItemBase::AUsableItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	bIsPlaced = false;


	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	
}

void AUsableItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	

}

void AUsableItemBase::Server_PlaceVoodooDoll_Implementation(const FVector& Location, const FRotator& Rotation)
{


//Set Spawn Collision Handling Override
FActorSpawnParameters ActorSpawnParams;
ActorSpawnParams.Owner = GetOwner();
ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ACharacterBase* CharacterBase = Cast<ACharacterBase>(GetOwner());
	if(CharacterBase)
	{
		//AUsableItemBase* SpawnedDoll = GetWorld()->SpawnActor<AUsableItemBase>(InventoryComponent->UsableItemInventoryArray[WeaponIndex-2], SpawnLocation, SpawnRotation, ActorSpawnParams);
		APlaceableItemBase* SpawnedDoll = GetWorld()->SpawnActor<APlaceableItemBase>(PlaceableItem, Location, Rotation, ActorSpawnParams);

		//Since the item is placed that means we should add that item to SpawnPointStack
		CharacterBase->SpawnedDollStack.Push(SpawnedDoll);
		
		SpawnedDoll->SetActorScale3D(FVector(2.f, 2.f, 2.f));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("spawndoll has problem"));
	}
	

}

// Called when the game starts or when spawned
void AUsableItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUsableItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


/*
void AUsableItemBase::HandleStartUseItem()
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
*/
