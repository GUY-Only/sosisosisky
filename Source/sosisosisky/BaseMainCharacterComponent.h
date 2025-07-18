#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseMainCharacterComponent.generated.h"

class AMainCharacter;

UCLASS( Abstract )
class SOSISOSISKY_API UBaseMainCharacterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBaseMainCharacterComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	AMainCharacter* OwnerCharacter;
};
