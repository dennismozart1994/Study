// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSkill.h"

#include "StudyCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ABuffSkill::ABuffSkill()
{
   ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System"));
   ParticleComponent->SetupAttachment(RootComponent);
   ParticleComponent->bAutoActivate = false;
   BuffCountDown = SkillDetails.BuffingTime;
}

void ABuffSkill::BeginPlay()
{
   Super::BeginPlay();
}

void ABuffSkill::CoolDown()
{
   Super::CoolDown();
   const UWorld* World = GetWorld();
   AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());
   if(PlayerRef && World)
   {
      if(GetLocalRole() == ROLE_Authority)
      {
         if(CoolDownTimeLine)
         {
            if(SkillDetails.Particle)
            {
               ParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(World, SkillDetails.Particle, PlayerRef->GetActorLocation(),
                  PlayerRef->GetActorRotation(), FVector(1.f), true, EPSCPoolMethod::None,
                  true);
            } else
            {
               UE_LOG(LogTemp, Warning, TEXT("Invalid Particle set on the buff skill"))
            }

            PlayerRef->BoostPlayerStats(SkillDetails.BuffType, SkillDetails.BuffingValue);
            BuffCountDown = SkillDetails.BuffingTime;
            World->GetTimerManager().SetTimer(_BuffTimer, this, &ABuffSkill::BuffTimeout, 1.f, false);
         } else
         {
            UE_LOG(LogTemp, Error, TEXT("Invalid Cool Down inside the Buff Skill"))
         }
      }
   } else
   {
      UE_LOG(LogTemp, Error, TEXT("Owner of the Buff Skill is not a player"))
   }
}

void ABuffSkill::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);
   DOREPLIFETIME(ABuffSkill, BuffCountDown);
   DOREPLIFETIME(ABuffSkill, _BuffTimer);
}

void ABuffSkill::Multicast_BuffCountDown_Implementation()
{
   const UWorld* World = GetWorld();
   AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());

   if(World && PlayerRef)
   {
      if(BuffCountDown <= 0)
      {
         BuffCountDown = SkillDetails.BuffingTime;
         World->GetTimerManager().ClearTimer(_BuffTimer);
         World->GetTimerManager().ClearAllTimersForObject(this);
         UE_LOG(LogTemp, Log, TEXT("Reset Buff to it's default value and Cleared out timers"))
      } else
      {
         UE_LOG(LogTemp, Log, TEXT("Buff is still operating, countdown on %s"), *FString::SanitizeFloat(BuffCountDown))
         BuffCountDown -= 1.f;
         World->GetTimerManager().SetTimer(_BuffTimer, this, &ABuffSkill::BuffTimeout, 1.f, false);
      }
   }
}

bool ABuffSkill::Multicast_BuffCountDown_Validate()
{
   return true;
}

// Reset buf to default values
void ABuffSkill::BuffTimeout()
{
   AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());
   if(PlayerRef) PlayerRef->BoostPlayerStats(SkillDetails.BuffType, SkillDetails.BuffingValue * -1);
   Multicast_BuffCountDown();
}

void ABuffSkill::OnTimelineUpdate()
{
   Super::OnTimelineUpdate();
   AStudyCharacter* PlayerRef = Cast<AStudyCharacter>(GetOwner());
   if(PlayerRef && ParticleComponent)
   {
      if(PlayerRef->bCanWalk)
      {
         if(GetLocalRole() == ROLE_Authority)
         {
            ParticleComponent->Deactivate();
         }
      }
   }
}

void ABuffSkill::OnTimelineFinished()
{
   Super::OnTimelineFinished();
   if(ParticleComponent)
   {
      if(GetLocalRole() == ROLE_Authority)
      {
         ParticleComponent->Deactivate();
      }
   }
}