// We Sir

#pragma once

#include "CoreMinimal.h"
#include "BaseAbilityComponent.h"
#include "Camera/CameraComponent.h"
#include "BoneProjectileComponent.generated.h"

class USceneComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOSISOSISKY_API UBoneProjectileComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBoneProjectileComponent();

	// Заряжаемый костянной снаряд

	// Меш для снаряда 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bone Projectile | Visual")
	class UStaticMeshComponent* ChargingMesh;

	// Точка спавна снаряда
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bone Projectile | Position")
	class USceneComponent* ChargingOrigin;

	// Максимальное время на каждую стадию зарядки 
	static constexpr float StageTime = 1.f;

	float CurrentChargeTime = 0.f;

	int32 CurrentVisualStage = 1;

	UPROPERTY(EditAnywhere, Category = "Bone Projectile | Visual")
	float Scale = 1.f;

	bool bIsBoneProjectileCharging = false;
	bool bIsButtonReleasedEarly = false;

	UPROPERTY(EditAnywhere, Category = "Bone Projectile | Cost")
	int32 Stage1Cost = 0;
	UPROPERTY(EditAnywhere, Category = "Bone Projectile | Cost")
	int32 Stage2Cost = 10;
	UPROPERTY(EditAnywhere, Category = "Bone Projectile | Cost")
	int32 Stage3Cost = 25;

	// Класс снаряда 
	UPROPERTY(EditAnywhere, Category = "Bone Projectile | Visual")
	TSubclassOf<class ABoneProjectile> BoneProjectileClass;

	// Методы нажатия и отпускания кнопки

	virtual void PressAbility() override;
	virtual void ReleaseAbility() override;

	void ChargingBoneProjectilePressed();
	void ChargingBoneProjectileReleased();

	void SpawnChargedBoneProjectile();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

};
