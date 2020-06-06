// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill3DPreview.h"
#include "Components/SceneComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ASkill3DPreview::ASkill3DPreview()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create default subobjects
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MeshPlaceHolder"));
	SceneRoot->SetupAttachment(RootComponent);
	
	MainBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MainBody"));
	MainBody->SetupAttachment(SceneRoot);
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(MainBody);
	
	SpringArm->SetRelativeRotation(FRotator(-40.f, 90.f, 0.f));
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 90.f));
	SpringArm->TargetArmLength = 800.f;

	RenderTarget = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("RenderTarget"));
	RenderTarget->SetupAttachment(SpringArm);
	RenderTarget->FOVAngle = 45.f;

}

// Called when the game starts or when spawned
void ASkill3DPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkill3DPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

