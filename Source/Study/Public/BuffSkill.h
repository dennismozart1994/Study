// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MasterSkill.h"
#include "BuffSkill.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API ABuffSkill : public AMasterSkill
{
	GENERATED_BODY()
	
	public:
	ABuffSkill();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowProvateAccess = "true"))
	class UParticleSystemComponent* ParticleComponent;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "BuffTime")
	FTimerHandle _BuffTimer;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "BuffTime")
	float BuffCountDown;

	protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BuffTimeout();

	public:
	virtual void CoolDown() override;
	virtual void OnTimelineUpdate() override;
	virtual void OnTimelineFinished() override;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Networking")
	void Server_BuffCountDown();
	
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Networking")
	void Client_BuffCountDown();
};
