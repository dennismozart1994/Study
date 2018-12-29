// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimInstanceDefault.h"

void UAnimInstanceDefault::UpdateAnimProperties()
{
	APawn* Pawn = TryGetPawnOwner();

	if (Pawn) {
		CharacterSpeed = Pawn->GetVelocity().Size();
	}
}