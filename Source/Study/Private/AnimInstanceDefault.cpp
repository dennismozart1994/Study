// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimInstanceDefault.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UAnimInstanceDefault::UAnimInstanceDefault()
{
	bDoOnce = true;
	bIsAlive = true;
}

void UAnimInstanceDefault::UpdateAnimProperties()
{
	APawn* Pawn = TryGetPawnOwner();
	
	if (Pawn) {
		CharacterSpeed = Pawn->GetVelocity().Size();
		PlayerStateRef = Cast<AStudyPlayerState>(Pawn->GetPlayerState());
		if(PlayerStateRef)
		{
			bIsAlive = PlayerStateRef->bIsAlive;
		}
	}
}

// Reset status so the player can attack again
void UAnimInstanceDefault::ResetCanAttack()
{
	AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(TryGetPawnOwner());
	if (PlayerRef)
	{
		PlayerRef->bIsReceivingDamage = false;
		PlayerRef->bCanAttack = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error trying to get player reference during cast on function ResetCanAttack() at the Anim Instance"));
	}
}

void UAnimInstanceDefault::DoOnce()
{
	if(bDoOnce)
	{
		DeathIndex = UKismetMathLibrary::RandomIntegerInRange(0, 4);
	}
}

void UAnimInstanceDefault::ResetDoOnce()
{
	bDoOnce = true;
}