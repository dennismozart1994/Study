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
	RecordingRef = GetWorld()->SpawnActor<ARecording3DPreviewActor>(ARecording3DPreviewActor::StaticClass(), FVector(0.f, 0.f, 5000000.0f), FRotator(0.f), SpawnParams);
	if (RecordingRef)
	{
		RecordingRef->SetNewMeshPreview(ItemDetails.Mesh, ItemDetails.DesiredThumbSize);
		UE_LOG(LogTemp, Log, TEXT("Spawned preview with success"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Preview has~not been spawned"));
	}
}

void UItem3DPreview::Close3D()
{
	if (RecordingRef)
	{
		RecordingRef->Destroy();
	}
}

void UItem3DPreview::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (RecordingRef)
	{
		RecordingRef->OnClickedEnd();
	}
}

FReply UItem3DPreview::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	if (RecordingRef)
	{
		RecordingRef->OnMouseMove(GetMousePosition());
	}
	return FReply::Unhandled();
}

FReply UItem3DPreview::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
	if (RecordingRef)
	{
		RecordingRef->OnClickedStart(GetMousePosition());
	}
	return FReply::Handled();
}

FReply UItem3DPreview::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	if (RecordingRef)
	{
		RecordingRef->OnClickedEnd();
	}
	return FReply::Unhandled();
}
