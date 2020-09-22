#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"

#include "MyBoneRotationData.generated.h"

USTRUCT()
struct FMyBoneRotationData
{
	GENERATED_BODY()

	//The bone to control its rotation
	UPROPERTY(EditDefaultsOnly)
	FName ControllerBoneName = "None";

	//The bones that will be influenced by the controller bone and their rotations
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, float> ControlledBoneNames;
};