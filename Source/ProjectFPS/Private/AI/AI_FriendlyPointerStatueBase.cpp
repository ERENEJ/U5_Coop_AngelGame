// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AI_FriendlyPointerStatueBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CharacterBase.h"
#include "ProjectFPSGameMode.h"
#include "AI/AIControllerBase.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

AAI_FriendlyPointerStatueBase::AAI_FriendlyPointerStatueBase()
{
	SetActorTickEnabled(false);
}

void AAI_FriendlyPointerStatueBase::BeginPlay()
{
	Super::BeginPlay();

	/*
	if( GetNetMode() == ENetMode::NM_ListenServer)
	{

		//FTimerHandle TempTimerHandle;
		//GetWorldTimerManager().SetTimer(TempTimerHandle, this, &AAI_FriendlyPointerStatueBase::SetInitialValues, 4.0f, false);

		FTimerHandle TempTimerHandle2;
		GetWorldTimerManager().SetTimer(TempTimerHandle2, this, &AAI_FriendlyPointerStatueBase::StartStoringPlayerLocation, 3.5f, true);
		
		SetActorTickEnabled((true));	
	}

	if(GetNetMode() == ENetMode::NM_Standalone)
	{
		//FTimerHandle TempTimerHandle;
		//GetWorldTimerManager().SetTimer(TempTimerHandle, this, &AAI_FriendlyPointerStatueBase::SetInitialValues, 4.0f, false);

		FTimerHandle TempTimerHandle2;
		GetWorldTimerManager().SetTimer(TempTimerHandle2, this, &AAI_FriendlyPointerStatueBase::StartStoringPlayerLocation, 3.5f, true);
		
		SetActorTickEnabled((true));	
	}
		*/
	// added a call a gamemode  bp that adds a character to array when it is connected (OnPostLogin)
	if(GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
	{
		FTimerHandle TempTimerHandle2;
		GetWorldTimerManager().SetTimer(TempTimerHandle2, this, &AAI_FriendlyPointerStatueBase::StartStoringPlayerLocation, 3.5f, true);
		
		SetActorTickEnabled((true));	
	}
	

}

void AAI_FriendlyPointerStatueBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAI_FriendlyPointerStatueBase, CurrentTargetItem);
	DOREPLIFETIME(AAI_FriendlyPointerStatueBase, CharacterBaseRefArray);
}

void AAI_FriendlyPointerStatueBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateFriendlyAngel();
	
	
}

void AAI_FriendlyPointerStatueBase::GetAllPlayerRefsFromLevel()
{
		TArray<AActor*> tempArray;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterBase::StaticClass(), tempArray);
		for(AActor* ActorPtr : tempArray)
		{
			ACharacterBase* tempCharptr = Cast<ACharacterBase>(ActorPtr);
			if(tempCharptr)
			{
				//UE_LOG(LogTemp, Warning, TEXT("char added"));
				CharacterBaseRefArray.Push(tempCharptr);
			}
		}
}

void AAI_FriendlyPointerStatueBase::StartStoringPlayerLocation()
{
	for(ACharacterBase* CharBasePtr : CharacterBaseRefArray)
	{
		if(IsValid(CharBasePtr))
		{

			if( !(CharBasePtr->GetActorLocation().Equals(PreviousPlayerLocation, 1500.f)))
			{

				PreviousPlayerLocation = CharBasePtr->GetActorLocation();
			}

			AAIControllerBase* AIControllerBase	= Cast<AAIControllerBase>(GetController());
	
			if (AIControllerBase && AIControllerBase->GetBlackboardComponent())
			{
				AIControllerBase->GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"), PreviousPlayerLocation);
			}
			
			return;
		}
	}
}

void AAI_FriendlyPointerStatueBase::SetInitialValues()
{
	
	GetAllPlayerRefsFromLevel();

	PreviousPlayerLocation = GetActorLocation();
	
	for(ACharacterBase* CharBasePtr : CharacterBaseRefArray)
	{
		if(IsValid(CharBasePtr))
		{
			AAIControllerBase* AIControllerBase	= Cast<AAIControllerBase>(GetController());
			if (AIControllerBase && AIControllerBase->GetBlackboardComponent())
			{
				AIControllerBase->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), CharBasePtr);
				break;
			}
		}
	}

}

void AAI_FriendlyPointerStatueBase::FollowPlayerandTurnFaceToTarget()
{
	for(ACharacterBase* CharBasePtr : CharacterBaseRefArray)
	{
		if(IsValid(CharBasePtr))
		{

			FaceTowardsTargetItem();
			
			bool bTempEyecontact = !IsThereAEyeContactWithPlayers();

			AAIControllerBase* AIControllerBase	= Cast<AAIControllerBase>(GetController());
	
			if (AIControllerBase && AIControllerBase->GetBlackboardComponent())
			{
				AIControllerBase->GetBlackboardComponent()->SetValueAsBool(TEXT("IsObserved"), bTempEyecontact);
			}

			if (bTempEyecontact)
			{
				// the eye connection place might be useful at the future
				
			}
			
			return;
		}
	}
}

void AAI_FriendlyPointerStatueBase::FaceTowardsTargetItem()
{
	//UE_LOG(LogTemp, Warning, TEXT("Face Towords target item start"));
	if(IsValid(CurrentTargetItem))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Current Target Item is Valid"));
		//FRotator Rotation = (GetActorLocation() - ( CurrentTargetItem->GetActorRotation());
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CurrentTargetItem->GetActorLocation()));
	}
}

bool AAI_FriendlyPointerStatueBase::IsThereAEyeContactWithPlayers()
{
	for(ACharacterBase* CharBasePtr : CharacterBaseRefArray)
	{

		FHitResult HitResult;
		FVector EyeLocation;
		FRotator notused;
		
		//had crash
		if(!CharBasePtr)
			return false;
		CharBasePtr->GetActorEyesViewPoint(EyeLocation,notused );
		GetWorld()->LineTraceSingleByChannel(HitResult,GetActorLocation(),EyeLocation,ECC_Visibility);

		FVector first = (GetActorLocation() - CharBasePtr->GetActorLocation());
		first.Normalize(0);
		
		FVector charFW = CharBasePtr->GetActorForwardVector();
		//this is wrong
		float AngleToCharacter = UKismetMathLibrary::DegAcos((first - charFW).Size());

		ACharacterBase* CharacterBase = Cast<ACharacterBase>(HitResult.GetActor());
		
		if(HitResult.bBlockingHit && CharacterBase &&AngleToCharacter > 30)
		{
			//there is a sight
			//UE_LOG(LogTemp, Warning, TEXT("player SEES the chaser AI"));
			return  true;
		}
	
	}


	return false;
}

void AAI_FriendlyPointerStatueBase::SetBaseDistanceToTargetItem()
{
	tempDistanceHolder = GetSquaredDistanceTo(UGameplayStatics::GetActorOfClass(GetWorld(), ATargetItemBase::StaticClass()));
	
}

void AAI_FriendlyPointerStatueBase::SelectClosestTargetItem()
{
	TArray<AActor*> tempArray;

	// TODO this line gets called at the tick definitely needs to be changed when you decide the the pick up logic  
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetItemBase::StaticClass(), tempArray);

	for(AActor* ActorPtr : tempArray)
	{
		ATargetItemBase* tempTargetItemBase = Cast<ATargetItemBase>(ActorPtr);

		if(tempTargetItemBase)
		{
			
			FVector tempDistance = (tempTargetItemBase->GetActorLocation()) - GetActorLocation();

			//UE::Math::TVector tempDistance = UKismetMathLibrary::Vector_DistanceSquared(tempTargetItemBase->GetActorLocation(), PreviousPlayerLocation);
			
			if( (tempDistance.Size() * tempDistance.Size()) < tempDistanceHolder )
			{
				tempDistanceHolder = tempDistance.Size() * tempDistance.Size();
				CurrentTargetItem = tempTargetItemBase;
			}
		}
	}

	

}

void AAI_FriendlyPointerStatueBase::UpdateFriendlyAngel()
{
	SetBaseDistanceToTargetItem();
	SelectClosestTargetItem();
	FollowPlayerandTurnFaceToTarget();
}
