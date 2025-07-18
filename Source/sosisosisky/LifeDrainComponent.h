// We Sir

#pragma once

#include "CoreMinimal.h"
#include "BaseMainCharacterComponent.h"
#include "LifeDrainComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOSISOSISKY_API ULifeDrainComponent : public UBaseMainCharacterComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULifeDrainComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
