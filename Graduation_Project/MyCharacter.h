// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class UMyIKSteppingComponent;
class USkeletalMeshComponent;
class UMyIKRotationController;
class UMyAnimInstance;

UCLASS()
class GRADUATION_PROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AMyCharacter();
	UMyIKSteppingComponent* GetIKSteppingComponent() { return IKSteppingComponent; }
	USkeletalMeshComponent* GetSkeletalMeshComponent() { return SkeletalMeshComponent; }
	UMyIKRotationController* GetIKRotationComponent() { return IKRotationComponent; }

	UMyAnimInstance* GetMyAnimInstance_Ref();

protected:

	virtual void PostInitializeComponents() override;

	UMyIKSteppingComponent* IKSteppingComponent = nullptr;
	USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
	UMyIKRotationController* IKRotationComponent = nullptr;

public:	

	void InitializeComponentsReferences();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
