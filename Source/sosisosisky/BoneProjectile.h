// We Sir

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BoneProjectile.generated.h"

UCLASS()
class SOSISOSISKY_API ABoneProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoneProjectile();

	void InitCharge(int32 InStage, const FVector& Direction);

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComp;

    /** ��������� �������� */
    UPROPERTY(VisibleAnywhere)
    UProjectileMovementComponent* ProjectileMovement;

    /** ������ ������ (1,2 ��� 3) */
    int32 Stage;

    /** ���� ��� ������ ������ */
    UPROPERTY(EditDefaultsOnly, Category = "My Settings Damage")
    float DamageStage1 = 10.f;
    UPROPERTY(EditDefaultsOnly, Category = "My Settings Damage")
    float DamageStage2 = 25.f;
    UPROPERTY(EditDefaultsOnly, Category = "My Settings Damage")
    float DamageStage3 = 50.f;

    // �������� ����� ������� � UU � ������� (1 UU = 1 ��)
    UPROPERTY(EditDefaultsOnly, Category = "My Settings Physics")
    float Speed = 2000.f; 

    UPROPERTY(EditDefaultsOnly, Category = "My Settings Physics")
    float Scale = 0.1f;

    UPROPERTY(EditDefaultsOnly, Category = "My Settings Physics")
    float Gravity = 0.1f;

    /** ���� ��� ������ ������ */
    UPROPERTY(EditDefaultsOnly, Category = "My Settings Visual")
    UStaticMesh* MeshStage1;
    UPROPERTY(EditDefaultsOnly, Category = "My Settings Visual")
    UStaticMesh* MeshStage2;
    UPROPERTY(EditDefaultsOnly, Category = "My Settings Visual")
    UStaticMesh* MeshStage3;

    /** ���������� ������������ */
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* Other,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
