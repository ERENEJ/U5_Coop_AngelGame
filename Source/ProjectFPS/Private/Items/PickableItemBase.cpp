// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickableItemBase.h"
#include "CharacterBase.h"

// Sets default values
APickableItemBase::APickableItemBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(Root);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(BoxCollision);

	BoxCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
	
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

}

// Called when the game starts or when spawned
void APickableItemBase::BeginPlay()
{
	Super::BeginPlay();
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this,&APickableItemBase::OnOverlapBegin);

}

// Called every frame
void APickableItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickableItemBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	
	ACharacterBase* Character = Cast <ACharacterBase>(OtherActor);
	if(Character)
	{
		Destroy();
	}

	/*		
		//Here checks if Character is alive without this functionality dead body capsule consumes pickable item
		if(Character && Character->GetHealth() > 0 && Character->IsPlayerControlled())
		{
			//TODO pickup logic will be here currently effects variable Not inventory component could be changed later
			Character->SetSCP_TotemAmount(+1);
			
			//UE_LOG(LogTemp, Warning, TEXT("component begin overlap2"));
			
			Destroy();
		}
	*/
	
}


