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

    // Movement setup
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false; // we handle rotation manually
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 120.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = 300.f;
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
        bIsRotating = false;
        bIsMoving = false;
        GetWorldTimerManager().ClearTimer(RoamTimerHandle);
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
    if (bIsDead)
        return;

    if (bIsRotating)
    {
        // Interpolate rotation toward desired
        FRotator Current = GetActorRotation();
        FRotator NewRot = FMath::RInterpConstantTo(Current, DesiredRotation, DeltaTime, GetCharacterMovement()->RotationRate.Yaw);
        SetActorRotation(NewRot);
        if (NewRot.Equals(DesiredRotation, 1.f))
        {
            bIsRotating = false;
            StartMove();
        }
    }
    else if (bIsMoving)
    {
        if (AAIController* AICon = Cast<AAIController>(GetController()))
        {
            if (AICon->GetMoveStatus() != EPathFollowingStatus::Moving)
            {
                bIsMoving = false;
                // Pause before next roam
                GetWorldTimerManager().SetTimer(RoamTimerHandle, this, &AEnemyBase::RoamToRandomPoint, RoamPauseTime, false);
            }
        }
    }
}

void AEnemyBase::RoamToRandomPoint()
{
    if (bIsDead)
        return;

    GetWorldTimerManager().ClearTimer(RoamTimerHandle);

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys)
        return;

    FNavLocation RandomPoint;
    if (NavSys->GetRandomPointInNavigableRadius(SpawnLocation, RoamRadius, RandomPoint))
    {
        NextMoveLocation = RandomPoint.Location;
        DesiredRotation = (NextMoveLocation - GetActorLocation()).Rotation();
        bIsRotating = true;
        bIsMoving = false;
    }
}

void AEnemyBase::StartMove()
{
    if (AAIController* AICon = Cast<AAIController>(GetController()))
    {
        bIsMoving = true;
        AICon->MoveToLocation(NextMoveLocation, AcceptanceRadius, true, true, true, false, 0, true);
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