// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InventoryComponent.h"
#include "Components/ProjectileCreationComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractionInterface.h"
#include "Items/UsableItemBase.h"
#include "MenuSystem/IngameMenu.h"
#include "Interfaces/PawnWidgetInterface.h"
#include "AI/AI_EnemyChaserStatueBase.h"
#include "CharacterBase.generated.h"


class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

class AWeaponBase;

UCLASS()
class PROJECTFPS_API ACharacterBase : public ACharacter ,  public IInteractionInterface, public IPawnWidgetInterface
{
	GENERATED_BODY()

	UFUNCTION(Reliable, Server)
	void Server_AttachInventoryItemToHand(int32 WeaponIndex);

	UFUNCTION(Reliable, NetMulticast)
	void MC_AttachInventoryItemToHand(int32 WeaponIndex);

	void AttachWeaponToHand(int32 WeaponIndex);
	
	void ChangeToInventoryItem0();
	void ChangeToInventoryItem1();
	void ChangeToInventoryItem2();
	
	AWeaponBase* GetCurrentWeapon() const;
	void TeleportThisCharacter(const FVector& Location);


	UPROPERTY()
	AAI_EnemyChaserStatueBase* EnemyChaserActorRef;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DeadMaterial;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DefaultMaterial;

	UFUNCTION(Reliable,Server)
	void Server_AddPitchValue( float Rate);

	UPROPERTY(Replicated)
	float Health = 100;


	UPROPERTY(EditAnywhere, Category = "Inventory Component")
	UInventoryComponent* InventoryComponent;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	
	void StartFire();
	void StopFire();
	void ChangeView();

	void Handle_Interact();
	void KeyQAction();
	
	UFUNCTION(Reliable,Server)
	void Server_Interact();
	void Interact();
	
	bool bFirstPersonCameraActive;
	bool bThirdPersonCameraActive;
	
	int CurrentInventoryItemIndex;

	
	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* ThirdPersonCameraComponent;
	
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Escape();

public:
	// Sets default values for this character's properties
	ACharacterBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	float GetHealthPercentage();

	UFUNCTION(BlueprintPure)
	float GetCraftMaterial1Amount();

	UFUNCTION(BlueprintPure)
	float GetCraftMaterial2Amount();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int CraftMaterialAmount1;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int CraftMaterialAmount2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int VoodooTotemAmount;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int TotalFoundTargetItemAmount;
	
	// uncomment for weapon oriented approach
	UPROPERTY(Replicated)
	//UPROPERTY()
	AWeaponBase* CurrentWeapon;

	UPROPERTY(Replicated)
	AUsableItemBase* CurrentUsableItem;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(Replicated, VisibleAnywhere)
	bool IsAlive = true;

	UCameraComponent* GetCurrentCameraComponent();

	UFUNCTION(BlueprintCallable)
	int GetTotalFoundTargetItemAmount();

	
	//handling Death Cosmetics such as hiding mesh and weapon of player
	void HandleDeathEvents();
	
	//handling Death Cosmetics such as hiding mesh and weapon of player
	UFUNCTION(Reliable,Server)
	void Server_HandleReviveEvents();

	UFUNCTION(Reliable, Client)
	void Client_HandleReviveEvents();

	UFUNCTION(Reliable,Server)
	void Server_HandleDeathEvents();

	UFUNCTION(Reliable, Client)
	void Client_HandleDeathEvents();
	
	
	void HandleReviveEvents();

	FVector LastKnownPosition;

	//Spawn location will be get from this items location
	UPROPERTY(Replicated)
	TArray<APlaceableItemBase*> SpawnedDollStack;

	UFUNCTION(Reliable,Server)
	void Server_SetIsAlive(bool In_IsAlive);
	
	UFUNCTION(Reliable,Server)
	void Server_SetHealth(float newHealthValue);

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }


	FORCEINLINE void SetHealth(float NewHealthValue) { Health = NewHealthValue;}
	FORCEINLINE float GetHealth() const {return Health;}

	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UProjectileCreationComponent* ProjectileCreationComponent;
	
	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	//A pointer to possessed pawn (IE: vehicle)
	UPROPERTY()
	APawn* CurrentPossessedPawn;

	//will used GetPawnWidgetInterface
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> CharacterHudWidget;

	//function of IPawnWidgetInterface Returns Char widget to display
	virtual TSubclassOf<UUserWidget> GetPawnWidgetClass() override;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsPossessable();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractWithActor(APawn* InteractionCaller);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PossesByInteraction(AController* CallerController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPreviousPossessedPawn(APawn* PreviousPawnReference);

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;



};
