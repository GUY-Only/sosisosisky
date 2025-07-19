// We Sir

#pragma once

#include "CoreMinimal.h"
#include "BaseMainCharacterComponent.h"
#include "BaseAbilityComponent.generated.h"

/**
 * 
 */
UCLASS()
class SOSISOSISKY_API UBaseAbilityComponent : public UBaseMainCharacterComponent
{
	GENERATED_BODY()
	

public: 
	UBaseAbilityComponent();

	virtual void PressAbility();

	virtual void ReleaseAbility();
};
