// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShipUnitBase.h"

#include "Net/UnrealNetwork.h"


// Sets default values
ASpaceShipUnitBase::ASpaceShipUnitBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipUnitCamera"));
	UnitMesh->SetupAttachment(RootComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(UnitMesh);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = true;
	
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->bEnableCameraRotationLag = true;

	SpringArmComponent->TargetArmLength = 500.f;
	
	//RotationComponent = CreateDefaultSubobject<URotationComponent>(TEXT("RotationComponent"));
}

// Called when the game starts or when spawned
void ASpaceShipUnitBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpaceShipUnitBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASpaceShipUnitBase, ShipMovementComponent);
	DOREPLIFETIME(ASpaceShipUnitBase, RotationComponent);
}

// Called every frame
void ASpaceShipUnitBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASpaceShipUnitBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &ASpaceShipUnitBase::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ASpaceShipUnitBase::LookUp);
		
	PlayerInputComponent->BindAxis("MoveForward", this, &ASpaceShipUnitBase::LookUp);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpaceShipUnitBase::Turn);
	PlayerInputComponent->BindAxis("KeyQ", this, &ASpaceShipUnitBase::Tilt);

	PlayerInputComponent->BindAxis("Sprint", this, &ASpaceShipUnitBase::AccelerateForward);
	PlayerInputComponent->BindAxis("LeftControl", this, &ASpaceShipUnitBase::AccelerateBackwards);


}

void ASpaceShipUnitBase::AccelerateForward(float Rate)
{
	if(Rate != 0)
	{
		if(ShipMovementComponent == nullptr)return;
		
		ShipMovementComponent->AccelerateForward(Rate, UnitMesh);
		UE_LOG(LogTemp, Warning, TEXT("Accelarete forward called from unit"));
	}

	
}


void ASpaceShipUnitBase::AccelerateBackwards(float Rate)
{
	if(Rate != 0)
	{
		if(ShipMovementComponent == nullptr)return;
		
		ShipMovementComponent->AccelerateBackwards(Rate, UnitMesh);
	}
}

void ASpaceShipUnitBase::LookUp(float Rate)
{
	if(Rate != 0)
	{
		if(RotationComponent == nullptr)return;
		
		RotationComponent->LookUpSceneComponent(Rate, UnitMesh);
	}

}

void ASpaceShipUnitBase::Turn(float Rate)
{
	if(Rate != 0)
	{
		if(RotationComponent == nullptr)return;
		RotationComponent->TurnSceneComponent(Rate, UnitMesh);
	}
}

void ASpaceShipUnitBase::Tilt(float Rate)
{
	if(Rate != 0)
	{
		if(RotationComponent == nullptr)return;
		RotationComponent->TiltSceneComponent(Rate, UnitMesh);
	}
}


void ASpaceShipUnitBase::SetRotationComponent(URotationComponent* RotationComponentRef)
{
	RotationComponent = RotationComponentRef;
}

void ASpaceShipUnitBase::AddShipMovementComponent()
{
	ShipMovementComponent = NewObject<UShipMovementComponent>(this);
	ShipMovementComponent->RegisterComponent();
}

void ASpaceShipUnitBase::SetShipMovementComponent(UShipMovementComponent* ShipMovementComponentRef)
{
	ShipMovementComponent = ShipMovementComponentRef;
}

void ASpaceShipUnitBase::SetProjectileCreationComponent(UProjectileCreationComponent* ProjectileCreationComponentRef)
{
	ProjectileCreationComponent = ProjectileCreationComponentRef;
}


TSubclassOf<UUserWidget> ASpaceShipUnitBase::GetPawnWidgetClass()
{
	//UE_LOG(LogTemp, Warning, TEXT("widget interface is called"));
	return ShipWidget;
}

void ASpaceShipUnitBase::SetPreviousPossessedPawn_Implementation(APawn* PreviousPawnReference)
{
	//IInteractionInterface::SetPreviousPawn_Implementation(PreviousPawnReference);
}

void ASpaceShipUnitBase::PossesByInteraction_Implementation(AController* CallerController)
{
	//IInteractionInterface::PossesByInteraction_Implementation(CallerController);
}

void ASpaceShipUnitBase::InteractWithActor_Implementation(APawn* InteractionCaller)
{
	//IInteractionInterface::InteractWithActor_Implementation();
}

bool ASpaceShipUnitBase::IsPossessable_Implementation()
{
	return false; //IInteractionInterface::bShouldPosses_Implementation();
}
