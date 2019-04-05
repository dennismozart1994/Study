// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"
#include "StudyPlayerState.h"
#include "StudyCharacter.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	SetIsReplicated(true);
	DefaultLife = 100.f;
	CurrentLife = 100.f;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentLife = DefaultLife;

	if (GetOwnerRole() == ROLE_Authority)
	{
		MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::DealDamage);
		}
	}
}

void USHealthComponent::DealDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	// Deal Damage on Player if it´s receiving damage
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
			UE_LOG(LogTemp, Error, TEXT("Player State is invalid"));
		}
	}
	// otherwise deal damage to AI and anyother objects
	else
	{
		CurrentLife = FMath::Clamp(CurrentLife - Damage, 0.f, DefaultLife);
		OnHealthChanged.Broadcast(this, int32(CurrentLife), int32(Damage), DamageType, InstigatedBy, DamageCauser);
		UE_LOG(LogTemp, Warning, TEXT("Owner is not Main Character but the object or AI has suffered damage"));
	}
}
