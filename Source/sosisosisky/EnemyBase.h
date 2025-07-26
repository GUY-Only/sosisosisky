#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "SoulActor.h"
#include "EnemyBase.generated.h"

class UHealthComponent;

UCLASS()
class SOSISOSISKY_API AEnemyBase : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyBase();

    //bool bIsDead = false;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EnemyBase | Logic")
    bool IsDead() const;

protected:
    virtual void BeginPlay() override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;
    virtual void Tick(float DeltaTime) override;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyBase | Stats")
    //float MaxHealth = 100.f;

    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyBase | Stats")
    //float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyBase | Respawn")
    float RespawnDelay = 15.f;
    FTimerHandle RespawnTimerHandle;

    UPROPERTY(EditAnywhere, Category = "EnemyBase | Movement")
    bool bIsRoaming = false;

    UPROPERTY(EditAnywhere, Category = "EnemyBase | Movement")
    float AcceptanceRadius = 50.f;

    UPROPERTY(EditAnywhere, Category = "EnemyBase | Movement")
    float RoamRadius = 1000.f;

    UPROPERTY(EditAnywhere, Category = "EnemyBase | Movement")
    float RoamPauseTime = 2.f;

    //float CurrentPercent;         
    //float DelayedPercent;

    UPROPERTY(EditAnywhere, Category = "EnemyBase | Bounty")
    int BoneBounty = 20;

    UPROPERTY(EditAnywhere, Category = "EnemyBase | Bounty")
    int SoulBounty = 1;

    UPROPERTY(EditAnywhere, Category = "EnemyBase | Bounty")
    float SoulLifeTime = 5.0f;

    UPROPERTY(EditAnywhere, Category = "EnemyBase | Bounty")
    float SoulHP = 100.0f;

    UPROPERTY(EditAnywhere, Category = "EnemyBase | Bounty")
    float SoulSize = 50.0f;

    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyBase | UI")
    //UWidgetComponent* HealthBarWidget;
    
    
    bool bIsRotating = false;
    bool bIsMoving = false;
    //bool bIsDelaying = false;
    //bool bIsLerping = false;

    FVector SpawnLocation;
    FVector NextMoveLocation;
    FRotator DesiredRotation;
    FTimerHandle RoamTimerHandle;
    //FTimerHandle DelayTimerHandle;

    void RoamToRandomPoint();    
    void StartMove();            
    void Respawn();
    //void UpdateHealthBar();
    //void StartDelayedLerp();

    UFUNCTION()
    void OnDeath(AActor* DamageCauser);

    UPROPERTY(EditAnywhere, Category = "EnemyBase | Bounty")
    bool isSpawningSoul = true;

    UPROPERTY(EditAnywhere, Category = "EnemyBase | Bounty")
    TSubclassOf<class ASoulActor> SoulActorClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyBase | Components")
    UHealthComponent* HealthComponent;

private:
    //FTimerHandle HealthBarTimerHandle;
    //void HideHealthBar();
    //void ShowHealthBar();
};