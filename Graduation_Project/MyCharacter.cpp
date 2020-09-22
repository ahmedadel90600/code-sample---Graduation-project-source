// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "MyIKSteppingComponent.h"
#include "MyAnimInstance.h"
#include "MyIKRotationController.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

UMyAnimInstance* AMyCharacter::GetMyAnimInstance_Ref()
{
	if (!ensure(SkeletalMeshComponent != nullptr)) return nullptr;

	return Cast<UMyAnimInstance>(SkeletalMeshComponent->GetAnimInstance());
}

void AMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitializeComponentsReferences();
}

void AMyCharacter::InitializeComponentsReferences()
{
	const TSet<UActorComponent*>& components = GetComponents();

	//Initialize component references
	for (UActorComponent* component : components)
	{
		if (UMyIKSteppingComponent* ikSteppingComponent = Cast<UMyIKSteppingComponent>(component))
		{
			IKSteppingComponent = ikSteppingComponent;
		}
		else if (USkeletalMeshComponent* skeletamMeshComponent = Cast<USkeletalMeshComponent>(component))
		{
			SkeletalMeshComponent = skeletamMeshComponent;
		}
		else if (UMyIKRotationController* ikRotationComponent = Cast<UMyIKRotationController>(component))
		{
			IKRotationComponent = ikRotationComponent;
		}
	}

	for (UActorComponent* component : components)
	{
		if (UMyActorComponent* myActorcomponent = Cast<UMyActorComponent>(component))
		{
			myActorcomponent->PostAllComponentsReferencesInitialized();
		}
	}
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

