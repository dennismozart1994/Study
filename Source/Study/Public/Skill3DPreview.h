// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Skill3DPreview.generated.h"

UCLASS()
class STUDY_API ASkill3DPreview : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UStaticMeshComponent* SceneGround;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneCaptureComponent2D* RenderTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* MainBody;
	
public:	
	// Sets default values for this actor's properties
	ASkill3DPreview();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class USkeletalMeshComponent* HeadMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class USkeletalMeshComponent* ChestMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class USkeletalMeshComponent* OffWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class USkeletalMeshComponent* DeffWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class USkeletalMeshComponent* HandsMesh;

	UPROPERTY( EditAnywhere, BlueprintReadWrite)
    class USkeletalMeshComponent* LegsMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class USkeletalMeshComponent* FootsMesh;

	void UpdateArmorSet();

	void PlaySkillAnimation(UAnimMontage* MontageToPlay);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
