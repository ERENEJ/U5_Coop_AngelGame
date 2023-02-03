// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AI_CharacterBase.h"
#include "CharacterBase.h"
#include "Kismet/GameplayStatics.h"



// Called when the game starts or when spawned
void AAI_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

	//CollisionBox->OnComponentBeginOverlap.AddDynamic(this,&AAI_CharacterBase::OnBoxCollisionBeginOverlap);

}

// Called to bind functionality to input
void AAI_CharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAI_CharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

// Called every frame
void AAI_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
