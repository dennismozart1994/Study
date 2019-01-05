// Fill out your copyright notice in the Description page of Project Settings.

#include "StudyPC.h"
#include "Engine/Engine.h"

AStudyPC::AStudyPC()
{
    bReplicates = true;
}

void AStudyPC::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
 {
     Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
     DOREPLIFETIME(AStudyPC, Inventory);
 }