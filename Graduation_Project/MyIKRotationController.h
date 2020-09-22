// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyActorComponent.h"
#include "Enums/MyLegType.h"
#include "MyBoneRotationData.h"

#include "MyIKRotationController.generated.h"

struct FBoneSteppingData;

 /*
 -This class controls the rotation of the spine based on some controller bones. For example,
 the right arm will control the upper spine when it moves forward by rotating the upper spine counter clockwise and the opposite
 when moving backwards.

 - Setting the BP on this class is as follows:
 1- The array TheBoneRotationData indicates the controller bones and what bones they should control.
 2- On the anim instace, the BonesAngles map will be filled with a name (key) representing the controlled bone name
 like lower/upper spine and a value that this bone should rotate

 The call stack for this class to handle rotation is:
 1- AMyCharacter::PostAllComponentsInitialized() -> UMyActorComponent::PostAllComponentsInitialized()
 2- UMyAnimInstance::InitializeBonesRotations which will initialize a TMap<Fname, float> correspondign to the bone
 that should rotate and the rotation angle value
 3- UMyIKSteppingComponent::TickComponent -> UMyIKRotationController::ControlSpineRotation, if a leg is off the ground,
 it will rotate the bone that should rotate with it, UMyAnimInstance::LerpBoneAngleByBoneName
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GRADUATION_PROJECT_API UMyIKRotationController : public UMyActorComponent
{
	GENERATED_BODY()
	
public:

	/*
	*@param isMovingForward: if it is false it will mean moving backwards (used as a sign)
	*		to indicate the sign of the rotation.
	*@param alphaDistance: the percentage between the vertical distance between the effector's location 
	*		and the home location
	*/
	void ControlSpineRotation(const bool isMovingForward, const float& alphaDistance, const FBoneSteppingData&);
	void PostAllComponentsReferencesInitialized() override;

protected:

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FVector DelayedForwardVector;

	//Delayed vector's interpolation speed
	UPROPERTY(EditDefaultsOnly)
	float DelayedForwardInterpSpeed = 0.0f;

	//Array of bone rotation data like the bone's name and how much it should rotate
	UPROPERTY(EditDefaultsOnly)
	TArray<FMyBoneRotationData> TheBoneRotationData;
};
