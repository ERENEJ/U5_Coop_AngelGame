// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AI_EnemyChaserStatueBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "AI/AIControllerBase.h"
#include "Kismet/KismetMathLibrary.h"

AAI_EnemyChaserStatueBase::AAI_EnemyChaserStatueBase()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(GetMesh());

	SetActorTickEnabled(false);
	
}

void AAI_EnemyChaserStatueBase::BeginPlay()
{
	Super::BeginPlay();

	//setting this here because most of the time the initialization not happens due to CDO
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this,&AAI_EnemyChaserStatueBase::OnBoxCollisionBeginOverlap);

	
	if( GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone)
	{
		FriendlyPointerStatueRef = Cast<AAI_FriendlyPointerStatueBase>(UGameplayStatics::GetActorOfClass(GetWorld(), AAI_FriendlyPointerStatueBase::StaticClass()));
		SetActorTickEnabled(true);		
	}
	

	
	//SetActorTickEnabled(true);
}

void AAI_EnemyChaserStatueBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AAIControllerBase* AIControllerBase	= Cast<AAIControllerBase>(GetController());

	
	if (AIControllerBase && AIControllerBase->GetBlackboardComponent())
	{
		
		AIControllerBase->GetBlackboardComponent()->SetValueAsBool(TEXT("IsObserved"), !IsThereAEyeContactWithPlayers());
	}
}

void AAI_EnemyChaserStatueBase::OnBoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacterBase* CollidedCharacterBase = Cast<ACharacterBase>(OtherActor);

	AAIControllerBase* AIControllerBase	= Cast<AAIControllerBase>(GetController());
		
	if(CollidedCharacterBase && AIControllerBase && CollidedCharacterBase == AIControllerBase->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")))
	{
		//Applying a whole killing damage here
		UGameplayStatics::ApplyDamage(CollidedCharacterBase,100, nullptr, this, nullptr);
		ClearCurrentTargetPlayer(CollidedCharacterBase);
			
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not get the blackboard"));
	}
}

void AAI_EnemyChaserStatueBase::ActivateEnemyStatue()
{
	if (KillListArray.IsEmpty())
	{
		if(FriendlyPointerStatueRef)
		{
			FriendlyPointerStatueRef->SetActorHiddenInGame(false);
			FriendlyPointerStatueRef->SetActorEnableCollision(true);
		}
		
		return;
	}

	if(FriendlyPointerStatueRef)
	{
		FriendlyPointerStatueRef->SetActorHiddenInGame(true);
		FriendlyPointerStatueRef->SetActorEnableCollision(false);
	}
	
	
	AAIControllerBase* AIControllerBase	= Cast<AAIControllerBase>(GetController());
	
	if (AIControllerBase)
	{
		/*
		if(TargetCharacter->IsAlive == false)
		{s

		}
		*/
		
		
		if (AIControllerBase->GetBlackboardComponent()  &&  !IsValid(AIControllerBase->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor"))))
		{
			//need to find first element off the array since I am using remove function the indices not 
			for (ACharacterBase* TargetChar : KillListArray)
			{
				if(TargetChar && TargetChar->IsAlive)
				{	
					AIControllerBase->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), TargetChar);
					UE_LOG(LogTemp, Warning, TEXT("Setted BB Value"));
					break;
				}
			}
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant setted Target Actor value to BB component"));
	}
}

void AAI_EnemyChaserStatueBase::ClearCurrentTargetPlayer(ACharacterBase* CharacterBase)
{
	AAIControllerBase* AIControllerBase	= Cast<AAIControllerBase>(GetController());
	
	if (AIControllerBase && AIControllerBase->GetBlackboardComponent() )
	{
		// need to clear current target value from Array and BB
		//TODO this logic is not correct 
		AIControllerBase->GetBlackboardComponent()->ClearValue(TEXT("TargetActor"));
		KillListArray.Remove(CharacterBase);
		ActivateEnemyStatue();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant Accssees AIController Base"));
	}
}

void AAI_EnemyChaserStatueBase::AddTargetPlayerToChaserQueue(ACharacterBase* CharacterBase)
{
	if(CharacterBase && CharacterBase->IsAlive)
	{
		KillListArray.AddUnique(CharacterBase);
		ActivateEnemyStatue();
	}

}

void AAI_EnemyChaserStatueBase::GetAllPlayerRefsFromLevel()
{
	if(GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
	{
		TArray<AActor*> tempArray;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterBase::StaticClass(), tempArray);
		for(AActor* ActorPtr : tempArray)
		{
			ACharacterBase* tempCharptr = Cast<ACharacterBase>(ActorPtr);
			if(tempCharptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("char added"));
				CharacterBaseArray.Push(tempCharptr);
			}
		}
	}

}

bool AAI_EnemyChaserStatueBase::IsThereAEyeContactWithPlayers()
{
	for(ACharacterBase* CharBasePtr : CharacterBaseArray)
	{
	
		FHitResult HitResult;
		FVector EyeLocation;
		FRotator notused;

		// had crash
		if(!CharBasePtr)
			return  false;
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
