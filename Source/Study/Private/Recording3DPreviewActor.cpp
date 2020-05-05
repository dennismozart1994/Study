// Fill out your copyright notice in the Description page of Project Settings.

#include "Recording3DPreviewActor.h"

// Cool libraries include
#include "Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Core/Public/Misc/OutputDeviceNull.h"

// Components Include
#include "Components/SceneComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/PointLightComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ARecording3DPreviewActor::ARecording3DPreviewActor()
{
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = DefaultSceneRoot;

	SceneCapture2DComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture2D"));
	SceneCapture2DComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
	SceneCapture2DComp->SetRelativeLocation(FVector(-50.f, 0.f, 0.f));
	SceneCapture2DComp->FOVAngle = 60.f;
	
	PointLightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	PointLightComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
	PointLightComp->Intensity = 1.f;
	PointLightComp->AttenuationRadius = 500.f;
	PointLightComp->SetRelativeLocation(FVector(0.f, -60.f, 110.f));

	Scene3DComp = CreateDefaultSubobject<USceneComponent>(TEXT("MeshRoot"));
	Scene3DComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("3DMesh"));
	Mesh->AttachToComponent(Scene3DComp, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	CurrentExtendedMesh = FVector(18.f);
	DesiredExtend = FVector(18.f, 18.f, 18.f);
	OnStartRotateLocation = FVector2D(0.f, 0.f);
	bRotationEnabled = false;
}

void ARecording3DPreviewActor::SetNewMeshPreview(class USkeletalMesh* NewMesh, FVector DesireExtent)
{
	if (UKismetSystemLibrary::IsValid(NewMesh))
	{
		Mesh->SetSkeletalMesh(NewMesh, true);
		Mesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f), false, nullptr, ETeleportType::None);
		Scene3DComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
		DesiredExtend = DesireExtent;
		UE_LOG(LogTemp, Log, TEXT("Called set mesh"));

		// Calls a custom event on the BP named UpdateScale
		// Not Done on c++ due to a glitch on the GetWorldScale functionality on c++, it's working properly on BP tough
		FOutputDeviceNull ar;
		this->CallFunctionByNameWithArguments(TEXT("UpdateScale"), ar, NULL, true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Mesh is null"));
	}
}

void ARecording3DPreviewActor::OnClickedStart(FVector2D MouseLocation)
{
	OnStartRotateLocation = MouseLocation;
	bRotationEnabled = true;
}

void ARecording3DPreviewActor::OnClickedEnd()
{
	bRotationEnabled = false;
}

void ARecording3DPreviewActor::OnMouseMove(FVector2D MouseLocation)
{
	if (bRotationEnabled)
	{
		FVector2D Rotation = MouseLocation - OnStartRotateLocation;
		float x, y;
		UKismetMathLibrary::BreakVector2D(Rotation, x, y);
		Scene3DComp->AddWorldRotation(UKismetMathLibrary::MakeRotator(0.f, y, x * -1));
		OnStartRotateLocation = MouseLocation;
	}
}
