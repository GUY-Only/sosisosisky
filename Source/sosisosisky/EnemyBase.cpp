#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // Movement setup
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false; // we handle rotation manually
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 120.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = 300.f;

    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    HealthBarWidget->SetupAttachment(RootComponent);
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
    HealthBarWidget->SetDrawSize(FVector2D(150, 20));
    HealthBarWidget->SetRelativeLocation(FVector(0, 0, 120));

    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(
        TEXT("/Game/UI/WBP_HealthBarRed")
    );
    if (WidgetBPClass.Class)
    {
        HealthBarWidget->SetWidgetClass(WidgetBPClass.Class);
    }
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();
    SpawnLocation = GetActorLocation();
    CurrentHealth = MaxHealth;
    CurrentPercent = DelayedPercent = 1.f;
    UpdateHealthBar();
    HealthBarWidget->SetVisibility(false);

    if(bIsRoaming)
        RoamToRandomPoint();
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead)
        return 0.f;

    const float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth -= DamageApplied;
    ShowHealthBar();

    CurrentPercent = FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.f);
    UpdateHealthBar();

    GetWorldTimerManager().ClearTimer(DelayTimerHandle);
    bIsDelaying = true;
    bIsLerping = false;

    // запускаем задержку 1 секунда
    GetWorldTimerManager().SetTimer(DelayTimerHandle, this,
        &AEnemyBase::StartDelayedLerp, 1.f, false);

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
        //SetActorHiddenInGame(true);
        GetMesh()->SetVisibility(false);
        GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AEnemyBase::Respawn, RespawnDelay, false);
    }

    return DamageApplied;
}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsLerping)
    {
        if (bIsDead) {
            DelayedPercent = FMath::FInterpConstantTo(
                DelayedPercent, CurrentPercent, DeltaTime, 1.f);
        }
        else {
            DelayedPercent = FMath::FInterpConstantTo(
                DelayedPercent, CurrentPercent, DeltaTime, 0.5f);
        }

        UpdateHealthBar();  

        if (FMath::IsNearlyEqual(DelayedPercent, CurrentPercent, 0.001f))
        {
            bIsLerping = false;

            if (bIsDead && FMath::IsNearlyZero(CurrentPercent))
            {
                HideHealthBar();
                SetActorHiddenInGame(true);
            }
        }
    }

    if (!bIsDead) {
        if (bIsRoaming) {
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
    CurrentPercent = DelayedPercent = 1.f;
    SetActorLocation(SpawnLocation);
    SetActorHiddenInGame(false);
    GetMesh()->SetVisibility(true);
    SetActorEnableCollision(true);
    RoamToRandomPoint();
}

void AEnemyBase::UpdateHealthBar()
{
    if (UUserWidget* Widget = HealthBarWidget->GetUserWidgetObject())
    {
        // Предполагаем, что в виджете есть ProgressBar с именем "HealthBar"
        if (UProgressBar* Bar = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("HealthBar"))))
        {
            Bar->SetPercent(CurrentHealth / MaxHealth);
        }
        if (UProgressBar* DelayBar =
            Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("DelayedBar"))))
        {
            DelayBar->SetPercent(DelayedPercent);
        }
    }
}

void AEnemyBase::ShowHealthBar()
{
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(true);
        // Сбрасываем предыдущий таймер
        GetWorldTimerManager().ClearTimer(HealthBarTimerHandle);
        // Ставим таймер на скрытие через 10 секунд
        GetWorldTimerManager().SetTimer(
            HealthBarTimerHandle,
            this,
            &AEnemyBase::HideHealthBar,
            10.f,
            false
        );
    }
}

void AEnemyBase::HideHealthBar()
{
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(false);
    }
}

void AEnemyBase::StartDelayedLerp()
{
    bIsDelaying = false;
    bIsLerping = true;
}