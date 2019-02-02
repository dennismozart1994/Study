// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"
#include "StudyPlayerState.h"
#include "StudyCharacter.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	SetIsReplicated(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::DealDamage);
		}
	}
}


// Called every frame
void USHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USHealthComponent::DealDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(MyOwner);
	if (PlayerRef)
	{
		AStudyPlayerState* PlayerStateRef = Cast<AStudyPlayerState>(PlayerRef->GetPlayerState());
		if (PlayerStateRef)
		{
			PlayerStateRef->CharacterStats.ActualLife = FMath::Clamp(PlayerStateRef->CharacterStats.ActualLife - int32(Damage), 0, PlayerStateRef->CharacterStats.ActualLife);
			OnHealthChanged.Broadcast(this, PlayerStateRef->CharacterStats.ActualLife, int32(Damage), DamageType, InstigatedBy, DamageCauser);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Player State is invalid"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Owner is not Main Character"));
	}
}
