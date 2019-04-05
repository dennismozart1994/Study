// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomVariables.h"
#include "Pickup.generated.h"

class UBoxComponent;

UCLASS()
class STUDY_API APickup : public AActor
{
	GENERATED_BODY()
public: 
	// Gameplay
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup Info", meta = (AllowPrivateAccess = "true"))
	UDataTable* ItemDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup Info", meta = (AllowPrivateAccess = "true"))
	FName ItemIndexDataTable;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TriggerBox;

public:	
	// Sets default values for this actor's properties
	APickup();

	UFUNCTION()
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void SERVER_OnClicked(FKey ButtonPressed);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	bool bIsNearBy;
	UPROPERTY(Replicated)
	TArray<AActor*> ActorsNearBy;

	UFUNCTION()
	void OnMouseOver(UPrimitiveComponent* TouchedActor);
	UFUNCTION()
	void OnMouseLeave(UPrimitiveComponent* TouchedActor);
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Interactable Area")
	void Server_ActivateItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION(Server, Reliable, WithValidation, Category = "Interactable Area")
	void Server_DeactivateItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
