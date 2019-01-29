// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimInstanceDefault.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UAnimInstanceDefault::UAnimInstanceDefault()
{
	bDoOnce = true;
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