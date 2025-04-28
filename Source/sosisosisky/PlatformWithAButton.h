// We Sir

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "PlatformWithAButton.generated.h"

/**
 * 
 */
UCLASS()
class SOSISOSISKY_API APlatformWithAButton : public AStaticMeshActor
{
	GENERATED_BODY()

public: 
	APlatformWithAButton();

	  virtual void BeginPlay() override;
	  virtual void Tick(float DeltaTime) override;

	  UPROPERTY(EditAnywhere, BlueprintReadWrite)
	  class UBoxComponent* TriggerComponent;

	  UPROPERTY(EditAnywhere, BlueprintReadWrite)
	  class UStaticMeshComponent* ButtonMeshComponent;


	  UPROPERTY(EditAnywhere, BlueprintReadWrite)
	  float Speed = 100.0f;

	  UPROPERTY(EditAnywhere, BlueprintReadWrite)
	  float PressOffset = -2.0f;

	  UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (MakeEditWidget = true))
	  FVector TargetLocation;

	  UFUNCTION()
	  void Triggered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	  UFUNCTION()
	  void Untriggered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	  UFUNCTION()
	  void PressButton();

	  UFUNCTION()
	  void ReleaseButton();

private: 
	FVector GStartLocation;
	FVector GTargetLocation;
	FVector OriginalButtonLocation;

	bool bMoving = false;
	bool bCanPressAgain = true;
	bool bTriggered = false;

};
