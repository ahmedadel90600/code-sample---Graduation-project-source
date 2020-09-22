// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Class.h"
#include "Enums/MyLegType.h"

#include "BoneSteppingData.generated.h"

USTRUCT(BluePrintType)
struct FBoneSteppingData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	FName BoneName = "";
	UPROPERTY(EditDefaultsOnly)
	FName OppositeBoneName = "";

	UPROPERTY(EditDefaultsOnly)
	EMyLegType LegType;

	float TimePassedSinceMoved = 0.0f;

	FVector LocalHomePosition;

	FBoneSteppingData* OppositeLeg = nullptr;

	bool bShouldMoveToHomeLocation = false;
	float MaxDistanceToStartMoving = 0.0f;

	//The location where the foot lifts off the ground to lerp from
	FVector StartMovingLocation;
};