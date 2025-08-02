// We Sir

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BeamTest.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class SOSISOSISKY_API ABeamTest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeamTest();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Beam Test | VFX", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* BeamVFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Beam Test | Targeting", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector BeamStart = FVector(0, 0, 0);;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Beam Test | Targeting", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector BeamEnd = FVector(0, 0, 80.0f);;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Beam Test | Targeting", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FRotator BeamStartRotation = FRotator(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Beam Test | VFX", meta = (AllowPrivateAccess = "true"))
	FName BeamStartParamName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Beam Test | VFX", meta = (AllowPrivateAccess = "true"))
	FName BeamEndParamName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Beam Test | Curvature", meta = (AllowPrivateAccess = "true"))
	float TangentStrength = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Beam Test | Curvature", meta = (AllowPrivateAccess = "true"))
	FName BeamStartTangentParamName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Beam Test | Curvature", meta = (AllowPrivateAccess = "true"))
	FName BeamEndTangentParamName;

	UPROPERTY()
	UNiagaraComponent* ActiveBeamComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
