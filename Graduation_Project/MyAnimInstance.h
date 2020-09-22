// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BoneSteppingData.h"
#include "MyBoneRotationData.h"

#include "MyAnimInstance.generated.h"

UCLASS()
class GRADUATION_PROJECT_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	void SetEffectorLocationByBoneName(const FName& boneName, const FVector& newLocation);
	void InitializeBoneSteppingData(const TArray<FBoneSteppingData>& boneSteppingData);
	void InitializeBonesRotations(const TArray<FMyBoneRotationData>& boneRotationData);

	const FVector* GetEffectorLocationByBoneName(const FName& boneName) const;

	//Returns true when the angle is close enough to the destination angle or false if boneName is not in BonesAngles map
	void LerpBoneAngleByBoneName(const FName& boneName, const float& newAngle, const float& alphaDistance);

protected:

	//Contains information that will move the effectors
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly)
	TMap<FName, FVector> BoneSteppingData;

	//Contains angles of bones that will be rotated
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly)
	TMap<FName, float> BonesAngles;

private:

};
