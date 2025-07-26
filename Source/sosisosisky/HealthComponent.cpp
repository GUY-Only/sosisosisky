#include "HealthComponent.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // Находим класс виджета по умолчанию. Вы можете изменить путь на свой.
    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/UI/WBP_HealthBarRed"));
    if (WidgetBPClass.Class)
    {
        HealthBarWidgetClass = WidgetBPClass.Class;
    }
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    CurrentPercent = DelayedPercent = 1.f;
    bIsDead = false;

    AActor* Owner = GetOwner();
    if (Owner && HealthBarWidgetClass)
    {
        // Создаем компонент виджета и прикрепляем его к владельцу.
        HealthBarWidget = NewObject<UWidgetComponent>(Owner, TEXT("HealthBar"));
        HealthBarWidget->RegisterComponent();
        HealthBarWidget->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
        HealthBarWidget->SetDrawSize(FVector2D(150, 20));
        HealthBarWidget->SetRelativeLocation(FVector(0, 0, 120)); // Вы можете настроить это смещение
        HealthBarWidget->SetWidgetClass(HealthBarWidgetClass);

        UpdateHealthBar();
        HealthBarWidget->SetVisibility(false);
    }
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Логика плавной анимации "отстающей" полоски здоровья.
    if (bIsLerping)
    {
        float InterpSpeed = bIsDead ? 1.f : 0.5f;
        DelayedPercent = FMath::FInterpConstantTo(DelayedPercent, CurrentPercent, DeltaTime, InterpSpeed);

        UpdateHealthBar();

        if (FMath::IsNearlyEqual(DelayedPercent, CurrentPercent, 0.001f))
        {
            bIsLerping = false;
            if (bIsDead && FMath::IsNearlyZero(CurrentPercent))
            {
                HideHealthBar();
            }
        }
    }
}

void UHealthComponent::TakeDamage(float DamageAmount, AActor* DamageCauser)
{
    if (bIsDead || DamageAmount <= 0)
    {
        return;
    }

    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
    ShowHealthBar();

    CurrentPercent = FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.f);
    UpdateHealthBar();

    GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
    bIsLerping = false;

    GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &UHealthComponent::StartDelayedLerp, 1.f, false);

    if (CurrentHealth <= 0.f)
    {
        bIsDead = true;
        HandleOwnerDeath(DamageCauser);
    }
}

void UHealthComponent::RestoreHealth(float Amount)
{
    if (bIsDead) return;
    CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);
    UpdateHealthBar();
}

void UHealthComponent::ResetHealth()
{
    CurrentHealth = MaxHealth;
    CurrentPercent = DelayedPercent = 1.f;
    bIsDead = false;
    UpdateHealthBar();
    HideHealthBar();
}

void UHealthComponent::SetMaxHealth(float NewHealth)
{
    MaxHealth = NewHealth;
}

void UHealthComponent::HandleOwnerDeath(AActor* DamageCauser)
{
    // Скрываем виджет через какое-то время, чтобы анимация успела проиграться
    GetWorld()->GetTimerManager().ClearTimer(HealthBarTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(HealthBarTimerHandle, this, &UHealthComponent::HideHealthBar, 3.f, false);

    // Сообщаем всем подписчикам (нашему врагу), что здоровье закончилось.
    OnHealthEnded.Broadcast(DamageCauser);
}

void UHealthComponent::UpdateHealthBar()
{
    if (UUserWidget* Widget = HealthBarWidget->GetUserWidgetObject())
    {
        if (UProgressBar* Bar = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("HealthBar"))))
        {
            Bar->SetPercent(CurrentPercent);
        }
        if (UProgressBar* DelayBar = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("DelayedBar"))))
        {
            DelayBar->SetPercent(DelayedPercent);
        }
    }
}

void UHealthComponent::ShowHealthBar()
{
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(true);
        GetWorld()->GetTimerManager().ClearTimer(HealthBarTimerHandle);
        GetWorld()->GetTimerManager().SetTimer(HealthBarTimerHandle, this, &UHealthComponent::HideHealthBar, 10.f, false);
    }
}

void UHealthComponent::HideHealthBar()
{
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(false);
    }
}

void UHealthComponent::StartDelayedLerp()
{
    bIsLerping = true;
}