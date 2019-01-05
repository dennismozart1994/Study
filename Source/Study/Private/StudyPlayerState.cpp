// Fill out your copyright notice in the Description page of Project Settings.

#include "StudyPlayerState.h"

AStudyPlayerState::AStudyPlayerState()
{
    // Gameplay Variables
	 CharacterStats.CharacterName =  "TEST CHARACTER";
	 CharacterStats.ActualLife = 100.f;
	 CharacterStats.FullLife = 100.f;
	 CharacterStats.ActualMana = 1000.f;
	 CharacterStats.FullMana = 1000.f;
	 CharacterStats.ActualStamina = 100.f;
	 CharacterStats.FullStamina = 100.f;
	 CharacterStats.CurrentLevel = 1;
	 CharacterStats.GoldAmount = 1;
	 CharacterStats.GoNextLevelWhen = 100;
}

void AStudyPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
 {
     Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
     DOREPLIFETIME(AStudyPlayerState, CharacterStats);
 }