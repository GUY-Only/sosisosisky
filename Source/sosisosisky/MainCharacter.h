// We Sir

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BoneProjectile.h"
#include "InteractionInterface.h"
#include "MainCharacter.generated.h"

UCLASS()
class SOSISOSISKY_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();



	// Камера

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)  // используется с компонентами, указателями на объекты. В противном случае, возможны вылеты
	class USpringArmComponent* CameraBoom;	// указатель используется с компонентами и с объектами в другом классе. Не копируют объект целиком, что экономит память

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* MainCamera;

	float CameraDistance;
	float RotationRate;



	// Передвижение

	void MoveForward(float Axis);
	void MoveRight(float Axis);
	void Jump();
	void StopJumping();


	float JumpVel;
	float MaxSpeed;

	bool bCanMove;



	// Абилки

	void Ability1Pressed();
	void Ability1Released();


	// Заряжаемый костянной снаряд

	// Меш для снаряда 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Settings BoneProjectile")
	class UStaticMeshComponent* ChargingMesh;

	// Точка спавна снаряда
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Settings BoneProjectile")
	class USceneComponent* ChargingOrigin;

	// Максимальное время на каждую стадию зарядки 
	static constexpr float StageTime = 1.f;

	float CurrentChargeTime = 0.f;

	int32 CurrentVisualStage = 1;

	UPROPERTY(EditAnywhere, Category = "My Settings BoneProjectile")
	float Scale = 0.1f;

	bool bIsBoneProjectileCharging = false;
	bool bIsButtonReleasedEarly = false;

	// Класс снаряда 
	UPROPERTY(EditAnywhere, Category = "My Settings BoneProjectile")
	TSubclassOf<class ABoneProjectile> BoneProjectileClass;

	// Методы нажатия и отпускания кнопки
	void ChargingBoneProjectilePressed();
	void ChargingBoneProjectileReleased();

	void SpawnChargedBoneProjectile();



	//Взаимодействие

	void TraceForInteractables();
	void Interact();
	void ShowInteractionUI(const FString& InteractionText);
	void HideInteractionUI();
	AActor* CurrentInteractable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Settings Interaction")
	float RayDistance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Settings Interaction")
	TArray<TSubclassOf<AActor>> IgnoreActorClasses;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
