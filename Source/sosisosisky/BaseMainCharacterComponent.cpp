#include "BaseMainCharacterComponent.h"
#include "MainCharacter.h"

UBaseMainCharacterComponent::UBaseMainCharacterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	OwnerCharacter = nullptr;
}

void UBaseMainCharacterComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<AMainCharacter>(GetOwner());

	if (!OwnerCharacter)
	{
		//UE_LOG(LogTemp, Error, TEXT("Компонент %s привязан к актору, который не является AMainCharacter!"), *GetName());
	}
}

