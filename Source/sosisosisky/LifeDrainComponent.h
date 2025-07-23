// We Sir

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterAttackComponent.h"
#include "LifeDrainComponent.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOSISOSISKY_API ULifeDrainComponent : public UBaseCharacterAttackComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULifeDrainComponent();

	virtual void PressAttack() override;
	virtual void ReleaseAttack() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private: 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Life Drain | VFX", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* BeamVFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Life Drain | Targeting", meta = (AllowPrivateAccess = "true"))
	float MaxTargetingAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Life Drain | Targeting", meta = (AllowPrivateAccess = "true"))
	float MaxBeamDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Life Drain | Targeting", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector BeamOriginOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Life Drain | VFX", meta = (AllowPrivateAccess = "true"))
	FName BeamStartParamName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Life Drain | VFX", meta = (AllowPrivateAccess = "true"))
	FName BeamEndParamName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Life Drain | Targeting", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> LineOfSightTraceChannel;


	float MaxTargetingCosine;

	TWeakObjectPtr<AActor> CurrentTarget;

	UPROPERTY()
	UNiagaraComponent* ActiveBeamComponent;

	bool bIsAttacking;

	AActor* FindBestTarget() const;

	void UpdateBeam();

	bool IsTargetStillValid() const;

	void StartDraining(AActor* Target);

	void StopDraining(bool isTargetAutoSwaped);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Life Drain | Damage", meta = (AllowPrivateAccess = "true"))
	float DrainAmountPerSecond;

	FVector GetBeamOrigin() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Life Drain | Curvature", meta = (AllowPrivateAccess = "true"))
	float TangentStrength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Life Drain | Curvature", meta = (AllowPrivateAccess = "true"))
	FName BeamStartTangentParamName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Life Drain | Curvature", meta = (AllowPrivateAccess = "true"))
	FName BeamEndTangentParamName;
};
