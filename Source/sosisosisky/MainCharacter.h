// We Sir

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BoneProjectile.h"
#include "InteractionInterface.h"
#include "Blueprint/UserWidget.h"
#include "BaseAbilityComponent.h"
#include "MainCharacter.generated.h"

class UInteractionComponent;
class UBoneProjectileComponent;
class ULifeDrainComponent;
class UHealthComponent;
class UPlayerDeathHUDWidget;

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	Bone,
	Soul
};

UENUM(BlueprintType)
enum class EAbilities : uint8
{
	None,
	BoneProjectile
};

UENUM(BlueprintType)
enum class EAttacks : uint8
{
	None,
	LifeDrain
};


UCLASS()
class SOSISOSISKY_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();



	// Камера

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main Character | Camera")  // используется с компонентами, указателями на объекты. В противном случае, возможны вылеты
	class USpringArmComponent* CameraBoom;	// указатель используется с компонентами и с объектами в другом классе. Не копируют объект целиком, что экономит память

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main Character | Camera")
	class UCameraComponent* MainCamera;

	float CameraDistance;
	float RotationRate;

	UPROPERTY()
	USceneComponent* DefaultSpringArmParent;
	UPROPERTY()
	FTransform DefaultSpringArmRelativeTransform;



	// Передвижение

	void MoveForward(float Axis);
	void MoveRight(float Axis);
	void Jump();
	void StopJumping();


	float JumpVel;
	float MaxSpeed;

	bool bCanMove;



	// Абилки
	
	bool CanUseAbilities = true;

	void Ability1Pressed();
	void Ability1Released();

	void Ability2Pressed();
	void Ability2Released();

	void Ability3Pressed();
	void Ability3Released();

	UFUNCTION(BlueprintCallable, Category = "Main Character | Abilities")
	void SetAbilityForSlot(int32 SlotIndex, EAbilities NewAbility);

	UPROPERTY(BlueprintReadOnly, Category = "Main Character | Abilities")
	UBaseAbilityComponent* Ability1_ComponentPtr;

	UPROPERTY(BlueprintReadOnly, Category = "Main Character | Abilities")
	UBaseAbilityComponent* Ability2_ComponentPtr;

	UPROPERTY(BlueprintReadOnly, Category = "Main Character | Abilities")
	UBaseAbilityComponent* Ability3_ComponentPtr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main Character | Components")
	class UBoneProjectileComponent* BoneProjectileComponent;
	



	// Взаимодействие

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main Character | Components")
	class UInteractionComponent* InteractionComponent;

	void Interact();



	// Ресурсы

	UFUNCTION(BlueprintCallable, Category = "Main Character | Resources")
	void AddResources(EResourceType ResourceType, int32 amount);
	UFUNCTION(BlueprintPure, Category = "Main Character | Resources")
	bool RemoveResources(EResourceType ResourceType, int32 amount);
	UFUNCTION(BlueprintPure, Category = "Main Character | Resources")
	int32 GetResourceCount(EResourceType ResourceType) const;




	// HUD

	TSubclassOf<UUserWidget> PlayerHUDClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main Character | UI")
	UUserWidget* PlayerHUDWidget;

	TSubclassOf<UPlayerDeathHUDWidget> PlayerDeathHUDClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main Character | UI")
	UPlayerDeathHUDWidget* PlayerDeathHUDWidget;

	void UpdateHUD();



	// Атаки

	void Attack1Pressed();
	void Attack1Released();

	void Attack2Pressed();
	void Attack2Released();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Character | Attacks")
	EAttacks Attack1 = EAttacks::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Character | Attacks")
	EAttacks Attack2 = EAttacks::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Main Character | Components")
	class ULifeDrainComponent* LifeDrainComponent;

	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Character | Resources")
	TMap<EResourceType, int32> Resources;

	
	// Смэрть

	FVector SpawnLocation;
	FRotator SpawnRotation;
	FRotator SpawnMeshRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Character | Respawn")
	float RespawnDelay = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Character | Respawn")
	float CamLagSpeed = 3.f;

	FTimerHandle RespawnTimerHandle;
	FTimerHandle RespawnHUDTimerHandle;

	UFUNCTION()
	void OnDeath(AActor* DamageCauser);
	void Respawn();
	void RespawnHUDAnim();

	

	

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyBase | Components")
	UHealthComponent* HealthComponent;

	UFUNCTION()
	void HandleHealthChanged(float CurrentPercent, float DelayedPercent);



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
