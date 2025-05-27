// We Sir

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UCLASS()
class SOSISOSISKY_API AEnemyBase : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyBase();

protected:
    virtual void BeginPlay() override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Respawn")
    float RespawnDelay = 15.f;

    FVector SpawnLocation;

    bool bIsDead;

    FTimerHandle RespawnTimerHandle;

    void RoamToRandomPoint();

    void Respawn();

    UPROPERTY(EditAnywhere, Category = "Enemy|Movement")
    float AcceptanceRadius = 50.f;

    UPROPERTY(EditAnywhere, Category = "Enemy|Movement")
    float RoamRadius = 1000.f;

    UPROPERTY(EditAnywhere, Category = "Enemy|Movement")
    float RoamPauseTime = 2.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Movement")
    float DefaultSpeed;
};
