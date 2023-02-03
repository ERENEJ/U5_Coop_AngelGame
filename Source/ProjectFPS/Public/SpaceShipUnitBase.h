// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/ProjectileCreationComponent.h"
#include "Components/RotationComponent.h"
#include "Components/ShipMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/InteractionInterface.h"
#include "Interfaces/PawnWidgetInterface.h"
#include "SpaceShipUnitBase.generated.h"

UCLASS()
class PROJECTFPS_API ASpaceShipUnitBase : public APawn, public IPawnWidgetInterface,public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASpaceShipUnitBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* UnitMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Turn(float Rate);

	void LookUp(float Rate);

	void Tilt(float Rate);

	void AccelerateForward(float Rate);

	void AccelerateBackwards(float Rate);
	
	UPROPERTY(Replicated)
	URotationComponent* RotationComponent;

	UPROPERTY(Replicated)
	UShipMovementComponent* ShipMovementComponent;
	
	UPROPERTY()
	UProjectileCreationComponent* ProjectileCreationComponent;

	//sets Rotation component for Runtime
	UFUNCTION(BlueprintCallable)
	void SetRotationComponent(URotationComponent* RotationComponentRef);

	//sets Rotation component for Runtime
	UFUNCTION(BlueprintCallable)
	void AddShipMovementComponent();
	

	UFUNCTION(BlueprintCallable)
	void SetShipMovementComponent(UShipMovementComponent* ShipMovementComponentRef);

	UFUNCTION(BlueprintCallable)
	void SetProjectileCreationComponent(UProjectileCreationComponent* ProjectileCreationComponentRef);


	//A pointer to Previos pawn to possess back
	UPROPERTY()
	APawn* PreviousPossessedPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> ShipWidget;

	//function of IPawnWidgetInterface Returns ship widget to display
	virtual TSubclassOf<UUserWidget> GetPawnWidgetClass() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsPossessable();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPreviousPossessedPawn(APawn* PreviousPawnReference);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractWithActor(APawn* InteractionCaller);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PossesByInteraction(AController* CallerController);

};



