// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActorComponent.h"
#include "Engine/World.h"

UMyActorComponent::UMyActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

const float UMyActorComponent::GetDeltaTime() const
{
	const UWorld* world = GetWorld();
	return world ? world->DeltaTimeSeconds : 0.0f;
}