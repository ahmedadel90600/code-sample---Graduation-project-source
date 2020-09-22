// Fill out your copyright notice in the Description page of Project Settings.


#include "MyIKRotationController.h"
#include "MyCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "MyAnimInstance.h"
#include "BoneSteppingData.h"

void UMyIKRotationController::ControlSpineRotation(const bool isMovingForward, const float& alphaDistance, const FBoneSteppingData& boneSteppingData)
{
	//TODO: Do the spine rotation handling if the character turning while stationary

	//If the leg is moving forward and the rotation is positive,
	//it will be reverted if the character is moving backwards
	const int32 movingForwardSign = isMovingForward ? 1 : -1;

	AMyCharacter* owner = Cast<AMyCharacter>(GetOuter());
	if (!ensure(owner != nullptr)) return;
	USkeletalMeshComponent* skeletalMeshComponent = owner->GetSkeletalMeshComponent();
	if (!ensure(skeletalMeshComponent != nullptr)) return;
	UMyAnimInstance* animaInstance = owner->GetMyAnimInstance_Ref();
	if (!ensure(animaInstance != nullptr)) return;
	//Use this formula to apply rotation (controlledBone.Value * cos(angle between forward and delayed) + controlledBone.Value * sin (angle between forward and delayed))
	for (FMyBoneRotationData& boneRotationData : TheBoneRotationData)
	{ 
		if (boneRotationData.ControllerBoneName == boneSteppingData.BoneName)
		{
			for (const auto& controlledBone : boneRotationData.ControlledBoneNames)
			{
				animaInstance->LerpBoneAngleByBoneName(controlledBone.Key, controlledBone.Value * movingForwardSign, alphaDistance);
			}
		}
	}
}

void UMyIKRotationController::PostAllComponentsReferencesInitialized()
{
	Super::PostAllComponentsReferencesInitialized();

	AMyCharacter* owner = Cast<AMyCharacter>(GetOuter());
	if (!ensure(owner != nullptr)) return;
	USkeletalMeshComponent* skeletalMeshComponent = owner->GetSkeletalMeshComponent();
	if (!ensure(skeletalMeshComponent != nullptr)) return;
	UMyAnimInstance* animaInstance = owner->GetMyAnimInstance_Ref();
	if (!ensure(animaInstance != nullptr)) return;

	animaInstance->InitializeBonesRotations(TheBoneRotationData);
}

void UMyIKRotationController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AMyCharacter* owner = Cast<AMyCharacter>(GetOuter());
	if (!ensure(owner != nullptr)) return;
	USkeletalMeshComponent* skeletalMeshComponent = owner->GetSkeletalMeshComponent();
	if (!ensure(skeletalMeshComponent != nullptr)) return;
	DelayedForwardVector = FMath::VInterpTo(DelayedForwardVector, skeletalMeshComponent->GetForwardVector(), DeltaTime, DelayedForwardInterpSpeed);
}
