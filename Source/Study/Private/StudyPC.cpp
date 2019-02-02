// Fill out your copyright notice in the Description page of Project Settings.

#include "StudyPC.h"
#include "Net/UnrealNetwork.h"
#include "StudyPlayerState.h"
#include "Engine/Engine.h"

AStudyPC::AStudyPC()
{
    bReplicates = true;
}

AStudyPlayerState* AStudyPC::GetPersonalPlayerState()
{
    AStudyPlayerState* MyPlayerState = Cast<AStudyPlayerState>(PlayerState);
    return MyPlayerState;
}

void AStudyPC::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
 {
     Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
     DOREPLIFETIME(AStudyPC, Inventory);
 }