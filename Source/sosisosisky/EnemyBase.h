#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "EnemyBase.generated.h"

UCLASS()
class SOSISOSISKY_API AEnemyBase : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyBase();

    bool bIsDead = false;

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
    FTimerHandle RespawnTimerHandle;

    UPROPERTY(EditAnywhere, Category = "Enemy|Movement")
    bool bIsRoaming = false;

    UPROPERTY(EditAnywhere, Category = "Enemy|Movement")
    float AcceptanceRadius = 50.f;

    UPROPERTY(EditAnywhere, Category = "Enemy|Movement")
    float RoamRadius = 1000.f;

    UPROPERTY(EditAnywhere, Category = "Enemy|Movement")
    float RoamPauseTime = 2.f;

    float CurrentPercent;         
    float DelayedPercent;

    UPROPERTY(EditAnywhere, Category = "Enemy|Bounty")
    int BoneBounty = 20;

    UPROPERTY(EditAnywhere, Category = "Enemy|Bounty")
    int SoulBounty = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|UI")
    UWidgetComponent* HealthBarWidget;
    
    
    bool bIsRotating = false;
    bool bIsMoving = false;
    bool bIsDelaying = false;
    bool bIsLerping = false;

    FVector SpawnLocation;
    FVector NextMoveLocation;
    FRotator DesiredRotation;
    FTimerHandle RoamTimerHandle;
    FTimerHandle DelayTimerHandle;

    void RoamToRandomPoint();    
    void StartMove();            
    void Respawn();
    void UpdateHealthBar();
    void StartDelayedLerp();

private:
    FTimerHandle HealthBarTimerHandle;
    void HideHealthBar();
    void ShowHealthBar();
};