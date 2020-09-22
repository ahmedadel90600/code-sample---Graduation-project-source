// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyActorComponent.h"
#include "BoneSteppingData.h"
#include "Enums/MyLegType.h"

#include "MyIKSteppingComponent.generated.h"

/*
* call stack:
* UMyIKSteppingComponent::PostAllComponentsReferencesInitialized -> UMyAnimInstance::InitializeBoneSteppingData
* UMyIKSteppingComponent::TickComponent -> UMyAnimInstance::SetEffectorLocationByBoneName
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRADUATION_PROJECT_API UMyIKSteppingComponent : public UMyActorComponent
{
	GENERATED_BODY()

public:	

	UMyIKSteppingComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	const TArray<FBoneSteppingData>& GetBoneSteppingData() const { return BoneSteppingData; }
	/*
	*@return the bone stepping data by name if it exists and null if it doesnt
	*/
	const FBoneSteppingData* GetBoneSteppingDataByName(const FName& boneName) const;

	const float GetMaxSteppingDistance() const { return MaxSteppingDistance; }
	const float GetSteppingSpeed() const { return SteppingTime; }

protected:

	void PostAllComponentsReferencesInitialized() override;

	//Array contains data like bone's name and home position of the bone
	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite)
	TArray<FBoneSteppingData> BoneSteppingData;

	//The max distance after which the leg should take a step
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxSteppingDistance = 0.0f;

	//Time the leg takes to step
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SteppingTime = 0.0f;

private:

	void SetOppositeLegs();
	EMyLegType LegThatShouldMove;
};