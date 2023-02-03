// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShipBase.h"

#include "Kismet/KismetArrayLibrary.h"

// Sets default values
ASpaceShipBase::ASpaceShipBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
	PlaneMesh->SetupAttachment(RootComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(PlaneMesh);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = true;
	
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->bEnableCameraRotationLag = true;

	SpringArmComponent->TargetArmLength = 500.f;

	RotationComponent = CreateDefaultSubobject<URotationComponent>(TEXT("RotationComponent"));
	
	ProjectileCreationComponent = CreateDefaultSubobject<UProjectileCreationComponent>(TEXT("ProjectileCreationComponent"));

	ShipMovementComponent = CreateDefaultSubobject<UShipMovementComponent>(TEXT("ShipMovementComponent"));


}

// Called when the game starts or when spawned
void ASpaceShipBase::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void ASpaceShipBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

// Called to bind functionality to input
void ASpaceShipBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &ASpaceShipBase::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ASpaceShipBase::LookUp);

	PlayerInputComponent->BindAxis("Sprint", this, &ASpaceShipBase::AccelerateForward);
	PlayerInputComponent->BindAxis("LeftControl", this, &ASpaceShipBase::AccelerateBackwards);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASpaceShipBase::LookUp);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpaceShipBase::Turn);

	PlayerInputComponent->BindAxis("KeyQ", this, &ASpaceShipBase::Tilt);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASpaceShipBase::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASpaceShipBase::StopFire);

	PlayerInputComponent->BindAction("Interact",IE_Pressed,this, &ASpaceShipBase::Handle_Interact);

}

void ASpaceShipBase::LookUp(float Rate)
{
	if(Rate != 0)
	{
		RotationComponent->LookUpSceneComponent(Rate, PlaneMesh);
	}

}

void ASpaceShipBase::Turn(float Rate)
{
	if(Rate != 0)
	{
		RotationComponent->TurnSceneComponent(Rate, PlaneMesh);
	}
}

void ASpaceShipBase::Tilt(float Rate)
{
	if(Rate != 0)
	{
		RotationComponent->TiltSceneComponent(Rate, PlaneMesh);
	}
}

void ASpaceShipBase::StopFire()
{

}

void ASpaceShipBase::Handle_Interact()
{
	// if caller is client server call Server_Interact
	//else caller is server call Interact
	if(GetNetMode()==ENetMode::NM_Client)
	{
		Server_Interact();
	}
	else
	{
		Interact();	
	}
}
void ASpaceShipBase::Server_Interact_Implementation()
{
	Interact();
}

void ASpaceShipBase::Interact()
{
	
	//TODO LineTrace logic will include after interaction interface
	FHitResult HitResult;
	//TODO Trace start position could be better place it was colliding  with owner either ignore owner or pick better start
	
	//FVector TraceStart = FirstPersonCameraComponent->GetComponentLocation()+FirstPersonCameraComponent->GetForwardVector()*100;
	//FVector TraceEnd = TraceStart + FirstPersonCameraComponent->GetForwardVector()*1000;
	//GetWorld()->LineTraceSingleByChannel(HitResult,TraceStart,TraceEnd,ECollisionChannel::ECC_Visibility);
	IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(HitResult.GetActor());

	/* this part is only worked for inherited interfaces tried with Implements<UInteractionInterface>() method as well
	 
	not worked for BP interface and interfaces that are add runtime it did not come up as valid interfaces 

	isValid checks for 1- object trace 2- İf object is there does is implements the interface

	works for both BP and İnherited interfaces, still need to cast for calling function over the interface

	/*
	if(TestInterFace)
	{
		TestInterFace->InteractWithActor();
	}
	*/

	
	if(IsValid(HitResult.GetActor()) &&	HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		if(InteractionInterface->Execute_IsPossessable(HitResult.GetActor()))
		{
			// Possess process need to set this pawn reference to possessed actor
			//and set this actor as Previous Pawn
			APawn* PawnToPossess = Cast<APawn>(HitResult.GetActor());
			if(PawnToPossess)
			{
				//need to attach character to seat or save his position it will be needed after the possesion
				InteractionInterface->Execute_SetPreviousPossessedPawn(HitResult.GetActor(),this);
				GetController()->Possess(PawnToPossess);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Pawn is not possed even though isPossable interface calls returns true "));
			}
		}
		else//non possession interaction
		{
			//if want to change/add extra condition this is the place
			
			//Currently empty call which is logical for current needs
			InteractionInterface->Execute_InteractWithActor(HitResult.GetActor(), this);
			UE_LOG(LogTemp, Warning, TEXT(" Execute InteractWithActor  called"));
		}

	}
	else// if line trace failed that means controller want to leave seat and return the previous Pawn
	{
		if(PreviousPossessedPawn)
		{
			UE_LOG(LogTemp, Warning, TEXT(" PreviousPossessedPawn must be possed right now"));
			GetController()->Possess(PreviousPossessedPawn);
			//we possessed previous Pawn at this point to prevent double possession
			//this nullptr will ensure the IsPossesable interface method to return true
			PreviousPossessedPawn = nullptr;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT(" PreviousPossessedPawn is not valid "));
		}
		
	}
	
	// There is no else for character base either interact with object or possesing
	// but for SpaceShip you may want to add extra abilities such as leave the steering wheel of the ship/unposses or attach the ship 
}


void ASpaceShipBase::AccelerateBackwards(float Value)
{
	if (Value != 0.0f)
	{
		/*
		PlaneMesh->AddWorldOffset(PlaneMesh->GetForwardVector()*-20);
		Server_AccelerateBackwards(Value);
		*/

		ShipMovementComponent->AccelerateBackwards(Value, PlaneMesh);
	}
}


void ASpaceShipBase::AccelerateForward(float Value)
{

	if (Value != 0.0f)
	{
		ShipMovementComponent->AccelerateForward(Value, PlaneMesh);
		/*
		PlaneMesh->AddWorldOffset(PlaneMesh->GetForwardVector()*20);
		Server_AccelerateForward(Value);
		*/
	}
	
}

TSubclassOf<UUserWidget> ASpaceShipBase::GetPawnWidgetClass()
{
	//UE_LOG(LogTemp, Warning, TEXT("widget interface is called"));
	return ShipWidget;
}

void ASpaceShipBase::SetPreviousPossessedPawn_Implementation(APawn* PreviousPawnReference)
{
	//IInteractionInterface::SetPreviousPawn_Implementation(PreviousPawnReference);
	if(PreviousPawnReference)
	{
		PreviousPossessedPawn = PreviousPawnReference;	
	}
	else
	{
		PreviousPossessedPawn = PreviousPawnReference;	
		UE_LOG(LogTemp, Warning, TEXT(" PreviousPawnReference is invalid at ASpaceShipBase::SetPreviousPossessedPawn_Implementation "));
	}
	
}

void ASpaceShipBase::PossesByInteraction_Implementation(AController* CallerController)
{
	//IInteractionInterface::PossesByInteraction_Implementation(CallerController);
}

void ASpaceShipBase::InteractWithActor_Implementation(APawn* InteractionCaller)
{
	UE_LOG(LogTemp, Warning, TEXT(" INTERFACE CALL TO ASpaceShipBase::InteractWithActor which is empty right now "));

	//This needs to be destroyed on server because of replication reasons
	//Destroy();  
}

bool ASpaceShipBase::IsPossessable_Implementation()
{
	if(PreviousPossessedPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("IsPossesable == FALSE because this actor is already possessed "));
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IsPossesable == TRUE "));
		return true; //IInteractionInterface::bShouldPosses_Implementation();
	}

}

//A Test function to test projectile creation component 
void ASpaceShipBase::StartFire()
{

	//TODO these location and rotation are world positions not relative position
	// they are here for testing reasons will change function parameter from void to to world socket location
	if(ProjectileCreationComponent)
	{
		FVector LocationOfBarrel;
		LocationOfBarrel.X=0;
		LocationOfBarrel.Y=0;
		LocationOfBarrel.Z=500;

		FRotator RotationOfBarrel;
		RotationOfBarrel.Pitch = 0;
		RotationOfBarrel.Yaw = 0;
		RotationOfBarrel.Roll = 0;

		
		//ProjectileCreationComponent->Server_CreateProjectile(LocationOfBarrel,RotationOfBarrel);
		UE_LOG(LogTemp, Warning, TEXT("fire function called"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("failed TO CREATE PROJECTİLE from projectile creation component"));
	}
	
	
}

// These functions are not used might need to delete them
void ASpaceShipBase::MoveForward(float Val)
{
}

void ASpaceShipBase::MoveRight(float Val)
{
}

