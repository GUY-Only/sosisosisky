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



	// ������

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)  // ������������ � ������������, ����������� �� �������. � ��������� ������, �������� ������
	class USpringArmComponent* CameraBoom;	// ��������� ������������ � ������������ � � ��������� � ������ ������. �� �������� ������ �������, ��� �������� ������

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* MainCamera;

	float CameraDistance;
	float RotationRate;



	// ������������

	void MoveForward(float Axis);
	void MoveRight(float Axis);
	void Jump();
	void StopJumping();


	float JumpVel;
	float MaxSpeed;

	bool bCanMove;



	// ������

	void Ability1Pressed();
	void Ability1Released();


	// ���������� ��������� ������

	// ��� ��� ������� 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Settings BoneProjectile")
	class UStaticMeshComponent* ChargingMesh;

	// ����� ������ �������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Settings BoneProjectile")
	class USceneComponent* ChargingOrigin;

	// ������������ ����� �� ������ ������ ������� 
	static constexpr float StageTime = 1.f;

	float CurrentChargeTime = 0.f;

	int32 CurrentVisualStage = 1;

	UPROPERTY(EditAnywhere, Category = "My Settings BoneProjectile")
	float Scale = 0.1f;

	bool bIsBoneProjectileCharging = false;
	bool bIsButtonReleasedEarly = false;

	// ����� ������� 
	UPROPERTY(EditAnywhere, Category = "My Settings BoneProjectile")
	TSubclassOf<class ABoneProjectile> BoneProjectileClass;

	// ������ ������� � ���������� ������
	void ChargingBoneProjectilePressed();
	void ChargingBoneProjectileReleased();

	void SpawnChargedBoneProjectile();



	//��������������

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
