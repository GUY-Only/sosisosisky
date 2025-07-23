// We Sir

#pragma once

#include "CoreMinimal.h"
#include "BaseMainCharacterComponent.h"
#include "BaseCharacterAttackComponent.generated.h"

/**
 * 
 */
UCLASS()
class SOSISOSISKY_API UBaseCharacterAttackComponent : public UBaseMainCharacterComponent
{
	GENERATED_BODY()
	
	virtual void PressAttack();

	virtual void ReleaseAttack();
};
