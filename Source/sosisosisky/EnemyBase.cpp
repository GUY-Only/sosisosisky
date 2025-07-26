#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "MainCharacter.h"
#include "HealthComponent.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 120.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = 300.f;

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

    /*HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));    Код закомментирован в связи с перемещением логики здоровья в отдельный компонент.
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
    }*/
}

bool AEnemyBase::IsDead() const
{
    if (HealthComponent)
    {
        return HealthComponent->IsDead();
    }
    // Если по какой-то причине компонента нет, считаем, что враг "мертв" (безопасное поведение)
    return true;
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();
    SpawnLocation = GetActorLocation();

    if (HealthComponent)
    {
        HealthComponent->OnHealthEnded.AddDynamic(this, &AEnemyBase::OnDeath);
    }

    //CurrentHealth = MaxHealth;
    //CurrentPercent = DelayedPercent = 1.f;
    //UpdateHealthBar();
    //HealthBarWidget->SetVisibility(false);

    if(bIsRoaming)
        RoamToRandomPoint();
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    //if (bIsDead)
    //    return 0.f;

    const float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (HealthComponent)
    {
        HealthComponent->TakeDamage(DamageApplied, DamageCauser);
    }

    /*CurrentHealth -= DamageApplied;
    ShowHealthBar();

    CurrentPercent = FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.f);
    UpdateHealthBar();

    GetWorldTimerManager().ClearTimer(DelayTimerHandle);
    bIsDelaying = true;
    bIsLerping = false;

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
        GetMesh()->SetVisibility(false);
        GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AEnemyBase::Respawn, RespawnDelay, false);

        if (auto* MC = Cast<AMainCharacter>(DamageCauser))
        {
            MC->AddResources(EResourceType::Bone, BoneBounty);
            MC->AddResources(EResourceType::Soul, SoulBounty);
        }
    }*/

    return DamageApplied;
}

void AEnemyBase::OnDeath(AActor* DamageCauser)
{
    // Останавливаем всю логику движения
    bIsRotating = false;
    bIsMoving = false;
    GetWorldTimerManager().ClearTimer(RoamTimerHandle);
    if (AAIController* AICon = Cast<AAIController>(GetController()))
    {
        AICon->StopMovement();
    }

    // Скрываем и отключаем противника
    SetActorEnableCollision(false);
    GetMesh()->SetVisibility(false);
    //SetActorHiddenInGame(true);

    // Запускаем таймер респауна
    GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AEnemyBase::Respawn, RespawnDelay, false);

    // Выдаем награду
    if (auto* MC = Cast<AMainCharacter>(DamageCauser))
    {
        MC->AddResources(EResourceType::Bone, BoneBounty);
        if(!isSpawningSoul) MC->AddResources(EResourceType::Soul, SoulBounty);
    }

    if (isSpawningSoul) {
        UWorld* World = GetWorld();
        if (World)
        {
            FTransform SpawnTransform = this->GetActorTransform(); // Позиция умершего врага
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = GetInstigator();
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            // 1. Создаем актора, но откладываем его полную инициализацию
            ASoulActor* SpawnedSoul = World->SpawnActorDeferred<ASoulActor>(SoulActorClass, SpawnTransform, SpawnParams.Owner, SpawnParams.Instigator, SpawnParams.SpawnCollisionHandlingOverride);

            if (SpawnedSoul)
            {
                // 2. Настраиваем актора ДО вызова BeginPlay
                SpawnedSoul->SetParams(SoulLifeTime, SoulBounty, SoulHP, SoulSize);

                // 3. Завершаем спавн. Только сейчас будет вызван BeginPlay на SpawnedSoul
                UGameplayStatics::FinishSpawningActor(SpawnedSoul, SpawnTransform);
            }
        }
    }
}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (HealthComponent && HealthComponent->IsDead())
    {
        return;
    }

    /*if (bIsLerping)
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

    if (!bIsDead) {*/
        if (bIsRoaming) {
            if (bIsRotating)
            {
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
                        GetWorldTimerManager().SetTimer(RoamTimerHandle, this, &AEnemyBase::RoamToRandomPoint, RoamPauseTime, false);
                    }
                }
            }
        }
    //}
    
}

void AEnemyBase::RoamToRandomPoint()
{
    //if (bIsDead)
    //    return;

    GetWorldTimerManager().ClearTimer(RoamTimerHandle);

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys)
        return;

    FNavLocation RandomPoint;
    if (NavSys->GetRandomPointInNavigableRadius(SpawnLocation, RoamRadius, RandomPoint))
    {
        NextMoveLocation = RandomPoint.Location;

        //DesiredRotation = (NextMoveLocation - GetActorLocation()).Rotation();
        FVector Dir = NextMoveLocation - GetActorLocation();
        Dir.Z = 0;  
        if (!Dir.IsNearlyZero())
            DesiredRotation = Dir.Rotation();
        DesiredRotation.Pitch = 0;
        DesiredRotation.Roll = 0;

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
    //bIsDead = false;
    /*CurrentHealth = MaxHealth;
    CurrentPercent = DelayedPercent = 1.f;
    SetActorLocation(SpawnLocation);
    SetActorHiddenInGame(false);
    GetMesh()->SetVisibility(true);
    SetActorEnableCollision(true);
    RoamToRandomPoint();*/

    // Восстанавливаем состояние противника
    SetActorLocation(SpawnLocation);
    SetActorHiddenInGame(false);
    GetMesh()->SetVisibility(true);
    SetActorEnableCollision(true);

    // Сбрасываем здоровье в компоненте
    if (HealthComponent)
    {
        HealthComponent->ResetHealth();
    }

    // Возобновляем патрулирование
    if (bIsRoaming)
    {
        RoamToRandomPoint();
    }
}

/*void AEnemyBase::UpdateHealthBar()
{
    if (UUserWidget* Widget = HealthBarWidget->GetUserWidgetObject())
    {
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
        
        GetWorldTimerManager().ClearTimer(HealthBarTimerHandle);
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
}*/