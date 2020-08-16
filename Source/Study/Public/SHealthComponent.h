// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

// On Health Change Custom Component Event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, class USHealthComponent*, HealthComp, int32, Health, int32, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STUDY_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadOnly, Category = "Owner")
	AActor* MyOwner;

public:	
	/** Initial Life of the character when Spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Life")
	float DefaultLife;

	/** Updated Life value during gameplay */
	UPROPERTY(BlueprintReadWrite, Category = "Life")
	float CurrentLife;

	/** Damage factor implies on how much in percentage an enemy damage will be taken by the player*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Life")
	float DamageFactor;

	UFUNCTION()
	void DealDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

};
