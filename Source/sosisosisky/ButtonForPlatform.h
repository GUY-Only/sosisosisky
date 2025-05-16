// We Sir

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "PlatformWithAButton.h"
#include "Engine/StaticMeshActor.h"
#include "ButtonForPlatform.generated.h"

/**
 *
 */
UCLASS()
class SOSISOSISKY_API AButtonForPlatform : public AInteractableActor
{
	GENERATED_BODY()

public:
	AButtonForPlatform();

	virtual void OnInteract_Implementation(AActor* Caller) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Settings Button")
	APlatformWithAButton* TargetPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* ButtonMeshComponent;
};
