#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsDead = false;

    // Плавный разворот в направлении движения
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0, 180.f, 0); //180 градусов в секунду - скорость разворота

    DefaultSpeed = 300.f;
    GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed;
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();
    SpawnLocation = GetActorLocation();
    CurrentHealth = MaxHealth;
    RoamToRandomPoint();
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead)
        return 0.f;

    const float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth -= DamageApplied;

    if (CurrentHealth <= 0.f)
    {
        bIsDead = true;
        if (AAIController* AICon = Cast<AAIController>(GetController()))
        {
            AICon->StopMovement();
        }
        SetActorEnableCollision(false);
        SetActorHiddenInGame(true);
        GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AEnemyBase::Respawn, RespawnDelay, false);
    }

    return DamageApplied;
}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsDead) return;

    if (AAIController* AICon = Cast<AAIController>(GetController()))
    {
        // Если мы уже не в пути — ставим таймер на новую рум-точку
        if (AICon->GetMoveStatus() != EPathFollowingStatus::Moving
            && !GetWorldTimerManager().IsTimerActive(RespawnTimerHandle))
        {
            // используем отдельный таймер для роуминга
            GetWorldTimerManager().SetTimer(
                RespawnTimerHandle,              // можно создать новый FTimerHandle, 
                // но здесь переиспользуем один для простоты
                this,
                &AEnemyBase::RoamToRandomPoint,
                RoamPauseTime,
                false
            );
        }
    }
}

void AEnemyBase::RoamToRandomPoint()
{
    GetWorldTimerManager().ClearTimer(RespawnTimerHandle);
    if (bIsDead)
        return;

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys)
        return;

    FNavLocation RandomPoint;
    if (NavSys->GetRandomPointInNavigableRadius(SpawnLocation, RoamRadius, RandomPoint))
    {
        if (AAIController* AICon = Cast<AAIController>(GetController()))
        {
            AICon->MoveToLocation(RandomPoint.Location, AcceptanceRadius, true, true, true, false, 0, true);
        }
    }
}

void AEnemyBase::Respawn()
{
    bIsDead = false;
    CurrentHealth = MaxHealth;
    SetActorLocation(SpawnLocation);
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    RoamToRandomPoint();
}