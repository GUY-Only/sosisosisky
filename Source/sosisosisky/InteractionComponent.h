// We Sir

#pragma once

#include "CoreMinimal.h"
#include "BaseMainCharacterComponent.h"
#include "InteractableActor.h"
#include "Camera/CameraComponent.h"
#include "InteractionComponent.generated.h"

class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOSISOSISKY_API UInteractionComponent : public UBaseMainCharacterComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

	void TraceForInteractables();
	void Interact();
	void ShowInteractionUI(const FString& InteractionText);
	void HideInteractionUI();
	AActor* CurrentInteractable;
	AActor* PreviousInteractable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Settings Interaction")
	float RayDistance = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Settings Interaction")
	float RayRadius = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Settings Interaction")
	TArray<TSubclassOf<AActor>> IgnoreActorClasses;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private: 

		
};
