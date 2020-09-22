// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "MyActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRADUATION_PROJECT_API UMyActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UMyActorComponent();

	/*
	*@return 0.0f if world == nullptr
	*/
	const float GetDeltaTime() const;
	virtual void PostAllComponentsReferencesInitialized() {/*Implement in children*/};

protected:

private:	
		
};
