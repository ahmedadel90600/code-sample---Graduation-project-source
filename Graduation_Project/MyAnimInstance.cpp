// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "GameFramework/Actor.h"
#include "MyIKSteppingComponent.h"
#include "MyCharacter.h"

void UMyAnimInstance::SetEffectorLocationByBoneName(const FName& boneName, const FVector& newLocation)
{
	if (!ensure(BoneSteppingData.Find(boneName) != nullptr)) return;

	BoneSteppingData[boneName] = newLocation;
}

void UMyAnimInstance::InitializeBoneSteppingData(const TArray<FBoneSteppingData>& boneSteppingData)
{
	for (const FBoneSteppingData& boneData : boneSteppingData)
	{
		if (ensure(boneData.BoneName != "None"))
		{
			FVector& createdValue = BoneSteppingData.Add(boneData.BoneName);
			if (USkeletalMeshComponent* skeletalMesh = GetSkelMeshComponent())
			{
				createdValue = boneData.LocalHomePosition + skeletalMesh->GetComponentLocation();
			}
		}
	}
}

void UMyAnimInstance::InitializeBonesRotations(const TArray<FMyBoneRotationData>& boneRotationData)
{
	for (const FMyBoneRotationData& currentBoneRotationData : boneRotationData)
	{
		for (const auto& currentControlledBone : currentBoneRotationData.ControlledBoneNames)
		{
			const FName& currentControlledBoneName = currentControlledBone.Key;
			if (currentControlledBoneName != "None" && !BonesAngles.Find(currentControlledBoneName))
			{
				float& currentAngle = BonesAngles.Add(currentControlledBoneName);
			}
		}
	}
}

const FVector* UMyAnimInstance::GetEffectorLocationByBoneName(const FName& boneName) const
{
	if (!ensure(BoneSteppingData.Find(boneName) != nullptr)) return nullptr;

	return &BoneSteppingData[boneName];
}

void UMyAnimInstance::LerpBoneAngleByBoneName(const FName& boneName, const float& newAngle, const float& alphaDistance)
{	
	if (!ensure(BonesAngles.Find(boneName) != nullptr)) return;

	BonesAngles[boneName] = FMath::Lerp(-newAngle, newAngle, alphaDistance);
}
