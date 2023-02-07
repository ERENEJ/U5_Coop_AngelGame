// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AI_FriendlyPointerStatueBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CharacterBase.h"
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
	
	if(GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
	{
		FTimerHandle TempTimerHandle;
		GetWorldTimerManager().SetTimer(TempTimerHandle, this, &AAI_FriendlyPointerStatueBase::StartStoringPlayerLocation, 3.5f, true);
		
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
			// if new location is closer further 15 meters update              
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

//This function Does 2 different job currently will be changed with next AI implementation
//FaceTowardsTargetItem will changed 
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
				// the eye connection place might be useful at the future for cosmetic events
			}
			
			return;
		}
	}
}

void AAI_FriendlyPointerStatueBase::FaceTowardsTargetItem()
{
	
	if(IsValid(CurrentTargetItem))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Current Target Item is Valid"));
		//FRotator Rotation = (GetActorLocation() - ( CurrentTargetItem->GetActorRotation()); //alternative
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

		FVector DirectionVec = (GetActorLocation() - CharBasePtr->GetActorLocation());
		DirectionVec.Normalize(0);
		
		FVector CharForwardVector = CharBasePtr->GetActorForwardVector();
		float AngleToCharacter = UKismetMathLibrary::DegAcos((DirectionVec - CharForwardVector).Size());

		float SightAngleCos = 30.0f;
		ACharacterBase* CharacterBase = Cast<ACharacterBase>(HitResult.GetActor());
		
		if(HitResult.bBlockingHit && CharacterBase && AngleToCharacter > SightAngleCos)
		{
			
			//UE_LOG(LogTemp, Warning, TEXT("player SEES the chaser AI"));
			return  true;
		}
	
	}

	return false;
}

void AAI_FriendlyPointerStatueBase::SetBaseDistanceToTargetItem()
{
	// base distance is called at tick because when the target object is obtained friendly angel needs to turn next target item
	//Observer statue/extra new enemy AI implementation will change this logic as well
	tempDistanceHolder = GetSquaredDistanceTo(UGameplayStatics::GetActorOfClass(GetWorld(), ATargetItemBase::StaticClass()));
	
}

void AAI_FriendlyPointerStatueBase::SelectClosestTargetItem()
{
	TArray<AActor*> tempArray;

	// TODO this line gets called at the tick and definitely needs to be changed with next Observer statue/extra new enemy AI implementation
	//currently there is not much actor in the level test map (320) and most of the actors are close to each other (in the memory) 
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
