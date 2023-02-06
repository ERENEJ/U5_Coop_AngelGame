// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "WeaponBase.h"
#include "AI/AIControllerBase.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentInventoryItemIndex = -1;
	CraftMaterialAmount1 = 0;
	CraftMaterialAmount2 = 0;
	VoodooTotemAmount = 1;

	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->SetIsReplicated(true);
	
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a CameraComponent	
	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	ThirdPersonCameraComponent->SetRelativeLocation(FVector(-190.f, 42.f, 96.f)); // Position the camera
	ThirdPersonCameraComponent->bUsePawnControlRotation = true;
	ThirdPersonCameraComponent->SetIsReplicated(true);

	bFirstPersonCameraActive = true;
	bThirdPersonCameraActive = false;

	ProjectileCreationComponent = CreateDefaultSubobject<UProjectileCreationComponent>(TEXT("ProjectileCreationComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharacterBase, Health);
	DOREPLIFETIME(ACharacterBase, IsAlive);

	//uncomment bottom lines for Weapon oriented approach
	DOREPLIFETIME(ACharacterBase, CurrentWeapon);
	DOREPLIFETIME(ACharacterBase, CurrentUsableItem);
	DOREPLIFETIME(ACharacterBase, SpawnedDollStack);
	DOREPLIFETIME(ACharacterBase, CraftMaterialAmount1);
	DOREPLIFETIME(ACharacterBase, CraftMaterialAmount2);
	DOREPLIFETIME(ACharacterBase, TotalFoundTargetItemAmount);
			
}

void ACharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACharacterBase::StopFire);
	//PlayerInputComponent->BindAction("ChangeView", IE_Pressed, this, &ACharacterBase::ChangeView);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterBase::LookUpAtRate);
	
	PlayerInputComponent->BindAction("ChangeToInventoryItem0", EInputEvent::IE_Pressed, this, &ACharacterBase::ChangeToInventoryItem0);
	PlayerInputComponent->BindAction("ChangeToInventoryItem1", EInputEvent::IE_Pressed, this, &ACharacterBase::ChangeToInventoryItem1);
	PlayerInputComponent->BindAction("ChangeToInventoryItem2", EInputEvent::IE_Pressed, this, &ACharacterBase::ChangeToInventoryItem2);
	PlayerInputComponent->BindAction("Escape",IE_Pressed,this, &ACharacterBase::Escape);
	PlayerInputComponent->BindAction("Interact",IE_Pressed,this, &ACharacterBase::Handle_Interact);
	PlayerInputComponent->BindAction("KeyQAction" ,IE_Pressed, this, &ACharacterBase::KeyQAction);
}

//Escape button func causes crash commented out
void ACharacterBase::Escape()
{

	/*
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	
	IngameMenu = CreateWidget<UIngameMenu>(PlayerController);
	if(!IngameMenu)return;
	IngameMenu->Setup();
	*/
}



// Called when the game starts or when SPAWNED
// There must be SCP type check to set the logic according to current SCP or SCP's
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	//trying to find first valid weapon to attach at hand at the start of the game
	//decided at runtime because Inventory component might not be initialized by designer
	if (!(InventoryComponent->WeaponInventoryArray.IsEmpty()))
	{
		// todo make it range loop
		for(int32 WeaponIndex = 0 ; WeaponIndex <= InventoryComponent->WeaponInventoryArray.Num()-1; WeaponIndex++)
		{
			if(InventoryComponent->WeaponInventoryArray[WeaponIndex])
			{
				Server_AttachInventoryItemToHand(WeaponIndex);
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("inventory component is empty at character base at begin play!!!"));
	}

}

void ACharacterBase::Server_AttachInventoryItemToHand_Implementation(int32 WeaponIndex)
{

	AttachWeaponToHand(WeaponIndex);
	
}

void ACharacterBase::MC_AttachInventoryItemToHand_Implementation(int32 WeaponIndex)
{
	AttachWeaponToHand(WeaponIndex);
}

void ACharacterBase::AttachWeaponToHand(int32 WeaponIndex)
{
	//	todo if the selected weapon is same with the current weapon do nothing
	if(CurrentInventoryItemIndex == WeaponIndex)return;
	
	const FRotator SpawnRotation = GetActorRotation(); //GetControlRotation();
	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector SpawnLocation = (Mesh1P->GetSocketLocation("GripPoint"));

	//Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Owner = GetOwner();
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// need to destroy previous weapon for freeing memory

	
		if (CurrentWeapon)
		{
			CurrentWeapon->Destroy();
		}

		if (CurrentUsableItem)
		{
			CurrentUsableItem->Destroy();
		}
	
		if (WeaponIndex < 2)
		{
			if (InventoryComponent->WeaponInventoryArray[WeaponIndex])
			{
				CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(InventoryComponent->WeaponInventoryArray[WeaponIndex], SpawnLocation, SpawnRotation, ActorSpawnParams);
				
			}
			//Weapon inventory will setted here null check add inventory
			if (CurrentWeapon)
			{
				CurrentWeapon->SetOwner(this);
				CurrentWeapon->AttachToComponent(Mesh1P,FAttachmentTransformRules::SnapToTargetIncludingScale,"GripPoint");
				CurrentInventoryItemIndex = WeaponIndex;
				UE_LOG(LogTemp, Warning, TEXT("Spawn called for weapon AttachWeaponToHand to hand"));
			}
		}
		// if it is not weapon that means it is usable item
		else
		{
			if (InventoryComponent->UsableItemInventoryArray[WeaponIndex-2])
			{
				CurrentUsableItem = GetWorld()->SpawnActor<AUsableItemBase>(InventoryComponent->UsableItemInventoryArray[WeaponIndex-2], SpawnLocation, SpawnRotation, ActorSpawnParams);
			}

			if (CurrentUsableItem)
			{
				CurrentUsableItem->SetOwner(this);
				CurrentInventoryItemIndex = WeaponIndex;
				CurrentUsableItem->AttachToComponent(Mesh1P,FAttachmentTransformRules::SnapToTargetIncludingScale,"GripPoint");
				UE_LOG(LogTemp, Warning, TEXT("Spawn called for usable item AttachWeaponToHand to hand"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("FAILED: usable item creation"));
			}
		}

}

void ACharacterBase::ChangeToInventoryItem0()
{
	Server_AttachInventoryItemToHand(0);
}

void ACharacterBase::ChangeToInventoryItem1()
{
	Server_AttachInventoryItemToHand(1);
}

void ACharacterBase::ChangeToInventoryItem2()
{
	Server_AttachInventoryItemToHand(2);
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACharacterBase::StartFire()
{
	//previous weapon oriented and character oriented shooting

	//without IsValid inconsistently crashes pending kill or non null after fast weapon switch causes crash? 
	if(IsValid(CurrentWeapon))
	{
		CurrentWeapon->bFireButtonPushedDown = true;
		//Uncomment this line for weapon oriented firing
		CurrentWeapon->HandleStartFireWeapon();

		UE_LOG(LogTemp, Warning, TEXT("Current weapon is  VALID at void ACharacterBase::StartFire()"));
	}
	else
	{
		if(IsValid(CurrentUsableItem) && VoodooTotemAmount>0)
		{
			if(CurrentUsableItem->bSpawnAllowed)
			{
				CurrentUsableItem->bIsPlaced = true;
				CurrentUsableItem->Server_PlaceVoodooDoll(CurrentUsableItem->VoodooSpawnLocation, CurrentUsableItem->VoodooSpawnRotation);

				//not sure if the ownership must be setted at server or here is ok
				CurrentUsableItem->SetOwner(this);
				CurrentUsableItem->bSpawnAllowed = false;
				--VoodooTotemAmount;
		
				
				// after the placement of voodoo doll it should gets the first item
				//todo create a Fist weapon which you can relay on as default inventory item?
				// todo make it range loop same code from beginplay
				for(int32 WeaponIndex = 0 ; WeaponIndex <= InventoryComponent->WeaponInventoryArray.Num()-1; WeaponIndex++)
				{
					if(InventoryComponent->WeaponInventoryArray[WeaponIndex])
					{
						Server_AttachInventoryItemToHand(WeaponIndex);
						break;
					}
				}

			}

		}
		else
		{

			UE_LOG(LogTemp, Warning, TEXT("Current usable item is not valid at void ACharacterBase::StartFire()"));
		}
		
	}

}

void ACharacterBase::StopFire()
{
	if(CurrentWeapon)
	{
		CurrentWeapon->bFireButtonPushedDown = false;
		CurrentWeapon->HandleStopFireWeapon();
	}
	
}

//changes fps to tps view of character
void ACharacterBase::ChangeView()
{
	UE_LOG(LogTemp, Warning, TEXT("changeviewFunction called"));

	//activate the third person
	if(bFirstPersonCameraActive)
	{
		ThirdPersonCameraComponent->SetActive(true);
		FirstPersonCameraComponent->SetActive(false);
		Mesh1P->SetOwnerNoSee(true);
		GetMesh()->SetOwnerNoSee(false);
		ThirdPersonCameraComponent->bUsePawnControlRotation = true;
		Mesh1P->SetVisibility(false);

		// need to add set visibility for weapon

		UE_LOG(LogTemp, Warning, TEXT("Tried to activate 3rd person cam"));
		
		bFirstPersonCameraActive = false;
		bThirdPersonCameraActive = true;

		return;
	}
	//activate the first person
	if(bThirdPersonCameraActive)
	{
		ThirdPersonCameraComponent->SetActive(false);
		FirstPersonCameraComponent->SetActive(true);
		Mesh1P->SetOwnerNoSee(false);
		GetMesh()->SetOwnerNoSee(true);
		FirstPersonCameraComponent->bUsePawnControlRotation = true;
		Mesh1P->SetVisibility(true);


		UE_LOG(LogTemp, Warning, TEXT("Tried to activate first person cam"));
		
		bFirstPersonCameraActive = true;
		bThirdPersonCameraActive = false;

	}
}

void ACharacterBase::Handle_Interact()
{
	// if caller is client server call Server_Interact
	//else caller is server call Interact
	if(GetNetMode()==ENetMode::NM_Client)
	{
		Server_Interact();
	}
	// for this project there is only client and listen server
	else
	{
		Interact();	
	}
}

void ACharacterBase::KeyQAction()
{
	if(CraftMaterialAmount1 >0 && CraftMaterialAmount2 >0)
	{
		++VoodooTotemAmount;
		--CraftMaterialAmount1;
		--CraftMaterialAmount2;
	}
}

void ACharacterBase::Server_Interact_Implementation()
{
	Interact();
}

void ACharacterBase::Interact()
{
	FHitResult HitResult;
	FVector TraceStart = FirstPersonCameraComponent->GetComponentLocation()+FirstPersonCameraComponent->GetForwardVector()*100;
	FVector TraceEnd = TraceStart + FirstPersonCameraComponent->GetForwardVector()*1000;
	GetWorld()->LineTraceSingleByChannel(HitResult,TraceStart,TraceEnd,ECollisionChannel::ECC_Visibility);
	
	IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(HitResult.GetActor());

	if(IsValid(HitResult.GetActor()) &&	HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		if(InteractionInterface->Execute_IsPossessable(HitResult.GetActor()))
		{
			// Possess process need to set this pawn reference to possessed actor
			//and set this actor as Previous Pawn
			APawn* PawnToPossess = Cast<APawn>(HitResult.GetActor());
			if(PawnToPossess)
			{
				CurrentPossessedPawn = PawnToPossess;
				//need to attach character to seat or save his position it will be needed after the possesion
				InteractionInterface->Execute_SetPreviousPossessedPawn(HitResult.GetActor(),this);
				GetController()->Possess(PawnToPossess);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Pawn is not possed even though isPossable interface calls returns true "));
			}
		}
		else
		{
			InteractionInterface->Execute_InteractWithActor(HitResult.GetActor(), this);
			UE_LOG(LogTemp, Warning, TEXT(" Execute InteractWithActor  called"));
		}

	}
	else// if line trace failed that means controller want to leave seat and return the previous Pawn
	{
		// Character should not have previous owner for Project Angel
	}
	
	// There is no else for character base either interact with object or possesing
	// but for SpaceShip you may want to add extra abilities such as leave the steering wheel of the ship/unposses or attach the ship 

}

UCameraComponent* ACharacterBase::GetCurrentCameraComponent()
{
	if(bFirstPersonCameraActive)
	{
		return FirstPersonCameraComponent;
	}
	else//bThirdPersonCameraActive
	{
		return ThirdPersonCameraComponent;
	}
}

int ACharacterBase::GetTotalFoundTargetItemAmount()
{
	return TotalFoundTargetItemAmount;
}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                 AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Warning, TEXT("Hit damage is: %f "), DamageToApply);

	
	DamageToApply = FMath::Min(Health, DamageToApply);

	Health -= DamageToApply;

	if (Health <= 0 )
	{
		if(!SpawnedDollStack.IsEmpty())
		{
			APlaceableItemBase* Temp =SpawnedDollStack.Pop();

			if(IsValid(Temp))
			{
				TeleportThisCharacter(Temp->GetActorLocation());
				Temp->Destroy();

				Health = 100.f;
			}
			}
			//no respawn doll death condition
			else
			{
				//need to call this on owber
				Client_HandleDeathEvents();
			}
		}
	
	return DamageToApply;
	
}

void ACharacterBase::Server_HandleDeathEvents_Implementation()
{
	HandleDeathEvents();
}

void ACharacterBase::Client_HandleDeathEvents_Implementation()
{
	HandleDeathEvents();
}

void ACharacterBase::HandleReviveEvents()
{
	EnableInput(GetLocalViewingPlayerController());
	GetCharacterMovement()->Activate();
	Server_SetHealth(100.0f);
	GetMesh()->SetMaterial(0, DefaultMaterial);
	ChangeView();
}

TSubclassOf<UUserWidget> ACharacterBase::GetPawnWidgetClass()
{
	return CharacterHudWidget;
}

void ACharacterBase::Server_SetHealth_Implementation(float newHealthValue)
{
	Health = newHealthValue;
}

void ACharacterBase::TeleportThisCharacter(const FVector& Location)
{
	SetActorLocation(Location);
}

void ACharacterBase::Server_HandleReviveEvents_Implementation()
{
	HandleReviveEvents();
}

//Hidding player mesh and disabling collision of deadbody 
void ACharacterBase::HandleDeathEvents()
{
	Server_SetHealth(0.f);
	
	//disable movement
	GetCharacterMovement()->Deactivate();
			
	// disables dead player input
	DisableInput(GetLocalViewingPlayerController());
	
	GetMesh()->SetMaterial(0, DeadMaterial);

	Server_SetIsAlive(false);

	ChangeView();
	
}

void ACharacterBase::Client_HandleReviveEvents_Implementation()
{
	HandleReviveEvents();
}

void ACharacterBase::Server_SetIsAlive_Implementation(bool In_IsAlive)
{
	IsAlive = In_IsAlive;
	
}

void ACharacterBase::Server_AddPitchValue_Implementation(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	if (FirstPersonCameraComponent)
	{
		FirstPersonCameraComponent->SetWorldRotation(GetController()->GetControlRotation());	
	}
}

AWeaponBase* ACharacterBase::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

void ACharacterBase::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		if(Value > 0.0f)
		{
			//moving forward faster than moving backwards
			AddMovementInput(GetActorForwardVector(), Value*8);
		}
		else
		{
			//moving backwards will be significantly slower that moving forward
			//TODO change this values 
			AddMovementInput(GetActorForwardVector(), Value * 0.3);
		}
		
	}
}

void ACharacterBase::MoveRight(float Value)
{
	//reduced moving right and left values as well
	if (Value != 0.0f)
	{	//TODO change this values 
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value * 0.3);
	}
}

void ACharacterBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::LookUpAtRate(float Rate)
{
	// old logic for single player before implementing AddPitchValue
	//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());

	if(HasAuthority())
	{
		//Multi_AddPitchValue(Rate); causes crash on beginplay
		// Get controller needs to controlled for on possess action on other pawns
		//the left over lookup rate causes problem on possess(This controller ref becomes invalid)
		if(GetController())
		{
			AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
			if (FirstPersonCameraComponent)
			{
				FirstPersonCameraComponent->SetWorldRotation(GetController()->GetControlRotation());	
			}
		}
	}
	else
	{
		if(GetController())
		{
			Server_AddPitchValue(Rate);
		}
	}
}

bool ACharacterBase::IsPossessable_Implementation()
{
	return false;
}

void ACharacterBase::InteractWithActor_Implementation(APawn* InteractionCaller)
{
	ACharacterBase* CharacterBase= Cast<ACharacterBase>(InteractionCaller);
	
	if( CharacterBase )
	{
		UE_LOG(LogTemp, Warning, TEXT("interact with actor called1"));
		if(!IsAlive)
		{
			Client_HandleReviveEvents();
		}
	
		Server_SetIsAlive(true);
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("interact with actor failed"));
	}
}

void ACharacterBase::PossesByInteraction_Implementation(AController* CallerController)
{
	// This method not need for Project Angel
	// used for pawn possession logic of Project Space Raid
}

void ACharacterBase::SetPreviousPossessedPawn_Implementation(APawn* PreviousPawnReference)
{
	IInteractionInterface::SetPreviousPossessedPawn_Implementation(PreviousPawnReference);
}

float ACharacterBase::GetHealthPercentage()
{
	return Health/100;
}

float ACharacterBase::GetCraftMaterial1Amount()
{
	return CraftMaterialAmount1;
}

float ACharacterBase::GetCraftMaterial2Amount()
{
	return CraftMaterialAmount2;
}


