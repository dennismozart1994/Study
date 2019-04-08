// Fill out your copyright notice in the Description page of Project Settings.

#include "Item3DPreview.h"
#include "Recording3DPreviewActor.h"

FVector2D UItem3DPreview::GetMousePosition()
{
	FVector2D MousePosition;
	GetOwningPlayer()->GetMousePosition(MousePosition.X, MousePosition.Y);
	return MousePosition;
}

void UItem3DPreview::SpawnPreview()
{
	FActorSpawnParameters SpawnParams;
	ActorToSpawn = GetWorld()->SpawnActor<ARecording3DPreviewActor>(RecordingRef, FVector(0.f, 0.f, 5000000.0f), FRotator(0.f), SpawnParams);
	if (ActorToSpawn && ItemDetails.Mesh)
	{
		UE_LOG(LogTemp, Log, TEXT("Spawned the Mesh %s with the size %s"), *ItemDetails.Mesh->GetName(), *ItemDetails.DesiredThumbSize.ToString());
		ActorToSpawn->SetNewMeshPreview(ItemDetails.Mesh, ItemDetails.DesiredThumbSize);
		UE_LOG(LogTemp, Log, TEXT("Spawned preview with success"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Preview has not been spawned"));
	}
}

void UItem3DPreview::Close3D()
{
	if (ActorToSpawn)
	{
		ActorToSpawn->Destroy();
	}
}

void UItem3DPreview::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (ActorToSpawn)
	{
		ActorToSpawn->OnClickedEnd();
	}
}

FReply UItem3DPreview::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	if (ActorToSpawn)
	{
		ActorToSpawn->OnMouseMove(GetMousePosition());
	}
	return FReply::Unhandled();
}

FReply UItem3DPreview::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
	if (ActorToSpawn)
	{
		ActorToSpawn->OnClickedStart(GetMousePosition());
	}
	return FReply::Handled();
}

FReply UItem3DPreview::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	if (ActorToSpawn)
	{
		ActorToSpawn->OnClickedEnd();
	}
	return FReply::Unhandled();
}
