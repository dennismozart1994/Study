// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Recording3DPreviewActor.generated.h"

UCLASS()
class STUDY_API ARecording3DPreviewActor : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneCaptureComponent2D* SceneCapture2DComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* PointLightComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Scene3DComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh;

public:	
	// Sets default values for this actor's properties
	ARecording3DPreviewActor();

	UPROPERTY(BlueprintReadWrite, Category = "Size")
	FVector CurrentExtendedMesh;

	UPROPERTY(BlueprintReadWrite, Category = "Size")
	FVector DesiredExtend;

	UPROPERTY(BlueprintReadOnly, Category = "Size")
	FVector2D OnStartRotateLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Size")
	bool bRotationEnabled;

public:	
	UFUNCTION()
	void SetNewMeshPreview(class USkeletalMesh* NewMesh, FVector DesireExtent);

	UFUNCTION()
	void OnClickedStart(FVector2D MouseLocation);

	UFUNCTION()
	void OnClickedEnd();

	UFUNCTION()
	void OnMouseMove(FVector2D MouseLocation);
};