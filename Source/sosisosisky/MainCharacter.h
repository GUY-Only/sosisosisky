// We Sir

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BoneProjectile.h"
#include "InteractionInterface.h"
#include "Blueprint/UserWidget.h"
#include "MainCharacter.generated.h"

class UInteractionComponent;
class UBoneProjectileComponent;

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
	Suck
};


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

	void Ability2Pressed();
	void Ability2Released();

	void Ability3Pressed();
	void Ability3Released();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Character | Abilities")
	EAbilities Ability1 = EAbilities::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Character | Abilities")
	EAbilities Ability2 = EAbilities::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Character | Abilities")
	EAbilities Ability3 = EAbilities::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoneProjectileComponent* BoneProjectileComponent;
	



	// Взаимодействие

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UInteractionComponent* InteractionComponent;



	// Ресурсы

	UFUNCTION(BlueprintCallable, Category = "My Character | Resources")
	void AddResources(EResourceType ResourceType, int32 amount);
	UFUNCTION(BlueprintPure, Category = "My Character | Resources")
	bool RemoveResources(EResourceType ResourceType, int32 amount);
	UFUNCTION(BlueprintPure, Category = "My Character | Resources")
	int32 GetResourceCount(EResourceType ResourceType) const;




	// HUD

	TSubclassOf<UUserWidget> PlayerHUDClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUserWidget* PlayerHUDWidget;

	void UpdateHUD();



	// Атаки

	void Attack1Pressed();
	void Attack1Released();

	void Attack2Pressed();
	void Attack2Released();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Character | Abilities")
	EAttacks Attack1 = EAttacks::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Character | Abilities")
	EAttacks Attack2 = EAttacks::None;
	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Character | Resources")
	TMap<EResourceType, int32> Resources;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
