// Fill out your copyright notice in the Description page of Project Settings.


#include "MyIKSteppingComponent.h"
#include "GameFramework/Actor.h"
#include "MyAnimInstance.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MyCharacter.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "MyIKRotationController.h"

UMyIKSteppingComponent::UMyIKSteppingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMyIKSteppingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AMyCharacter* owner = Cast<AMyCharacter>(GetOuter());
	if (!ensure(owner != nullptr)) return;
	const USkeletalMeshComponent* skeletalaMeshComponent = owner->GetSkeletalMeshComponent();
	if (!ensure(skeletalaMeshComponent != nullptr)) return;
	UMyAnimInstance* animInstance = owner->GetMyAnimInstance_Ref();
	if (!ensure(animInstance != nullptr)) return;
	UMyIKRotationController* ikRotationController = owner->GetIKRotationComponent();

	for (FBoneSteppingData& boneData : BoneSteppingData)
	{
		const FName& boneName = boneData.BoneName;
		if (!ensure(skeletalaMeshComponent->DoesSocketExist(boneName))) return;

		const FVector* currentLegLocationptr = animInstance->GetEffectorLocationByBoneName(boneName);
		if (!ensureMsgf(currentLegLocationptr != nullptr, TEXT("Bone name is not valid"))) return;

		const FVector& currentWorldLegLocation = *currentLegLocationptr;
		const FVector& localHomePosition = boneData.LocalHomePosition;
		const FVector skeletalMeshCompForward = skeletalaMeshComponent->GetForwardVector();
		const FVector skeletalMeshCompRight = skeletalaMeshComponent->GetRightVector();
		const FVector skeletalMeshCompUp = skeletalaMeshComponent->GetUpVector();

		const FVector finalWorldHomePosition = skeletalaMeshComponent->GetComponentLocation() + (skeletalMeshCompRight * localHomePosition.Y)
			+ (skeletalMeshCompForward * localHomePosition.X + (skeletalMeshCompUp * localHomePosition.Z));

		FBoneSteppingData* oppositeLeg = boneData.OppositeLeg;
		bool& shouldCurrentLegMoveToHomeLocation = boneData.bShouldMoveToHomeLocation;
		const float& currentLegMaxDistanceToStartMoving = boneData.MaxDistanceToStartMoving;
		
		const float distanceBetweenHomeAndEffectorPosition = FVector::Dist2D(finalWorldHomePosition, currentWorldLegLocation);
		if (!shouldCurrentLegMoveToHomeLocation && boneData.LegType == LegThatShouldMove && 
			distanceBetweenHomeAndEffectorPosition >= currentLegMaxDistanceToStartMoving)
		{
			boneData.StartMovingLocation = currentWorldLegLocation;
			if (oppositeLeg != nullptr)
			{
				if (!oppositeLeg->bShouldMoveToHomeLocation)
				{
					shouldCurrentLegMoveToHomeLocation = true;
					int32 legthatShouldMove = static_cast<int32>(LegThatShouldMove) + 1;
					//keep switching between left and right
					LegThatShouldMove = static_cast<EMyLegType>(legthatShouldMove % static_cast<int32>(EMyLegType::MAX));
				}
			}
			else
			{
				shouldCurrentLegMoveToHomeLocation = true;
			}
		}

#if WITH_EDITOR
		DrawDebugLine(GetWorld(), currentWorldLegLocation, currentWorldLegLocation + FVector(0, 0, 1000), FColor::Red);
		DrawDebugLine(GetWorld(), finalWorldHomePosition, finalWorldHomePosition + FVector(0, 0, 1000), FColor::Blue);
#endif //WITH_EDITOR

		float& timePassedSinceLegMoved = boneData.TimePassedSinceMoved;
		if (shouldCurrentLegMoveToHomeLocation)
		{
			const FVector& legStartMovingLocation = boneData.StartMovingLocation;

			const FVector towardHomeVector = finalWorldHomePosition - legStartMovingLocation;
			FVector centerPoint = (towardHomeVector / 2.0f + legStartMovingLocation) +
				FVector::UpVector * FVector::Dist2D(finalWorldHomePosition, legStartMovingLocation);

			timePassedSinceLegMoved += DeltaTime;
			float timePercentageToStep = 0.0f;
			if (ensure(SteppingTime != 0))
			{
				timePercentageToStep = timePassedSinceLegMoved / SteppingTime;
				//Bezier curve for leg interpolation
				const FVector& startToMid = FMath::LerpStable(legStartMovingLocation, centerPoint, timePercentageToStep);
				const FVector& midToEnd = FMath::LerpStable(centerPoint, finalWorldHomePosition, timePercentageToStep);
				const FVector& finalPosition = FMath::LerpStable(startToMid, midToEnd, timePercentageToStep);
				animInstance->SetEffectorLocationByBoneName(boneName, finalPosition);


				//The right vector represents the forward vector here
				const float cosAngleBetweeMeshForwardAndMovementDirection = FVector::DotProduct(towardHomeVector.GetSafeNormal(),
					skeletalMeshCompRight);
				const float angleBetweeMeshForwardAndMovementDirection = FMath::Acos(cosAngleBetweeMeshForwardAndMovementDirection) * 180 / PI;
				const float distanceBetweenLegAndHome = (currentWorldLegLocation - finalWorldHomePosition).Size2D();
				const float percentagePassedFromMovingLocationToHome = 1 - distanceBetweenLegAndHome / towardHomeVector.Size2D();
				ikRotationController->ControlSpineRotation(angleBetweeMeshForwardAndMovementDirection < 90.0f, percentagePassedFromMovingLocationToHome, boneData);
			}

			//The leg reached home location
			if (timePercentageToStep >= 1.0f)
			{
				timePassedSinceLegMoved = 0.0f;
				shouldCurrentLegMoveToHomeLocation = false;				
				FVector oppositeLegEffectorLocation = currentWorldLegLocation;
				FVector oppositeLegLocalHomePosition = localHomePosition;

				if (oppositeLeg != nullptr)
				{
					const FVector* oppositeLegEffectorLocationPtr = animInstance->GetEffectorLocationByBoneName(oppositeLeg->BoneName);
					if (!ensureMsgf(oppositeLegEffectorLocationPtr != nullptr, TEXT("Bone name is not valid"))) return;

					oppositeLegEffectorLocation = *animInstance->GetEffectorLocationByBoneName(oppositeLeg->BoneName);
					oppositeLegLocalHomePosition = oppositeLeg->LocalHomePosition;
				}

				const FVector vectorBetweenLegs = currentWorldLegLocation - oppositeLegEffectorLocation;
				const float distanceBetween2Legs = vectorBetweenLegs.Size2D();
				const float distanceBetween2LegsSquared = (distanceBetween2Legs * distanceBetween2Legs);
				const float distanceBetweenHomes = (localHomePosition - oppositeLegLocalHomePosition).Size2D();

				const float verticalDistanceBetweenLegs = FMath::Sqrt(distanceBetween2LegsSquared - (distanceBetweenHomes * distanceBetweenHomes));
				const float horizontalDistanceBetweenLegs = FMath::Sqrt(distanceBetween2LegsSquared - (verticalDistanceBetweenLegs * verticalDistanceBetweenLegs))
					- distanceBetweenHomes;

				float& oppositeLegMaxDis = oppositeLeg->MaxDistanceToStartMoving;
				oppositeLegMaxDis = MaxSteppingDistance + FMath::Clamp(FVector2D(horizontalDistanceBetweenLegs, verticalDistanceBetweenLegs).Size()
					, 0.0f, MaxSteppingDistance);
				oppositeLegMaxDis = FMath::Clamp(oppositeLegMaxDis, MaxSteppingDistance, MaxSteppingDistance * 2.0f);
			}
		}
	}
}

const FBoneSteppingData* UMyIKSteppingComponent::GetBoneSteppingDataByName(const FName& boneName) const
{
	for (const FBoneSteppingData& boneSteppingData : BoneSteppingData)
	{
		if (boneSteppingData.BoneName == boneName)
		{
			return &boneSteppingData;
		}
	}

	return nullptr;
}

void UMyIKSteppingComponent::PostAllComponentsReferencesInitialized()
{
	Super::PostAllComponentsReferencesInitialized();

	AMyCharacter* owner = Cast<AMyCharacter>(GetOwner());
	if (!ensure(owner != nullptr)) return;
	USkeletalMeshComponent* skeletalMeshComp = owner->GetSkeletalMeshComponent();
	if (!ensure(skeletalMeshComp != nullptr)) return;

	TArray<USceneComponent*> homePositionSceneComponents;
	skeletalMeshComp->GetChildrenComponents(true, homePositionSceneComponents);

	//Set the local home position of bone stepping data and the max distance for a leg to start moving
	for (USceneComponent* sceneComponent : homePositionSceneComponents)
	{
		for (FBoneSteppingData& boneSteppingData : BoneSteppingData)
		{
			if (sceneComponent->GetFName() == boneSteppingData.BoneName)
			{
				const float scaleX = skeletalMeshComp->GetRelativeTransform().GetScale3D().X;
				const float scaleY = skeletalMeshComp->GetRelativeTransform().GetScale3D().Y;
				const float scaleZ = skeletalMeshComp->GetRelativeTransform().GetScale3D().Z;

				const FVector& componentRelativeLocation = sceneComponent->GetRelativeTransform().GetLocation();
				boneSteppingData.LocalHomePosition = componentRelativeLocation * FVector(scaleX, scaleY, scaleZ);
				boneSteppingData.MaxDistanceToStartMoving = MaxSteppingDistance;
				break;
			}
		}
	}

	UMyAnimInstance* animInstance = owner->GetMyAnimInstance_Ref();
	if (!ensure(animInstance != nullptr)) return;
	//Initialize a map of Fname and FVector on the animinstance to be used to fetch values in the animinstance blueprint
	animInstance->InitializeBoneSteppingData(BoneSteppingData);
	LegThatShouldMove = static_cast<EMyLegType>(FMath::RandHelper((int32)(EMyLegType::MAX)));
	SetOppositeLegs();
}

void UMyIKSteppingComponent::SetOppositeLegs()
{
	for (FBoneSteppingData& currentBoneSteppingData : BoneSteppingData)
	{
		for (FBoneSteppingData& boneData : BoneSteppingData)
		{
			if (currentBoneSteppingData.OppositeBoneName == boneData.BoneName)
			{
				currentBoneSteppingData.OppositeLeg = &boneData;
				continue;
			}
		}
	}
}