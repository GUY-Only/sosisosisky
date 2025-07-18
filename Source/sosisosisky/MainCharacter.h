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

	EAbilities Ability1 = EAbilities::None;

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



	// ПКМ

	void Attack2Pressed();
	void Attack2Released();


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
