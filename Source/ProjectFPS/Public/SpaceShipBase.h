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
#include "SpaceShipBase.generated.h"

UCLASS()
class PROJECTFPS_API ASpaceShipBase : public APawn, public IPawnWidgetInterface,public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASpaceShipBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Val);

	void MoveRight(float Val);

	void Turn(float Rate);

	void LookUp(float Rate);

	void Tilt(float Rate);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	URotationComponent* RotationComponent;

	UPROPERTY()
	UShipMovementComponent* ShipMovementComponent;

	UPROPERTY(EditAnywhere)
	UProjectileCreationComponent* ProjectileCreationComponent;
	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PlaneMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	//A pointer to Previous pawn to possess back
	UPROPERTY()
	APawn* PreviousPossessedPawn;

	//A pointer to Previous pawn to possess back
	UPROPERTY()
	APawn* CurrentPossessedPawn;
	
	void AccelerateForward(float Value);
	void AccelerateBackwards(float Value);
	
	void StartFire();
	void StopFire();

	void Handle_Interact();
	UFUNCTION(Reliable,Server)
	void Server_Interact();
	void Interact();
	
	
	/*
	UFUNCTION(Reliable,Server)
	void Server_AccelerateForward(float Value);
	UFUNCTION(Reliable,Server)
	void Server_AccelerateBackwards(float Value);
	*/
	UPROPERTY(EditAnywhere)
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

