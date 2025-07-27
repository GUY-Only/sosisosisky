#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "HealthComponent.generated.h"

// Декларируем делегат (событие), который будет вызываться, когда здоровье закончится.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthEnded, AActor*, DamageCauser);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SOSISOSISKY_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

    // Делегат, который можно будет использовать в Blueprint'ах и C++ для реакции на смерть.
    UPROPERTY(BlueprintAssignable, Category = "HealthComponent | Events")
    FOnHealthEnded OnHealthEnded;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent | UI")
    FVector HealthBarOffset = FVector(0, 0, 120);

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Максимальное здоровье. Можно будет настраивать для каждого актора.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent | Stats")
    float MaxHealth = 100.f;

    // Текущее здоровье. Доступно только для чтения извне.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HealthComponent | Stats")
    float CurrentHealth;

    // Компонент виджета для полоски здоровья.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HealthComponent | UI")
    UWidgetComponent* HealthBarWidget;

    // Путь к блюпринту виджета полоски здоровья.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent | UI")
    TSubclassOf<class UUserWidget> HealthBarWidgetClass;

private:
    float CurrentPercent; // Текущий процент здоровья для мгновенной полоски
    float DelayedPercent; // Процент для "отстающей" полоски

    bool bIsLerping = false; // Флаг, идет ли анимация "отстающей" полоски
    bool bIsDead = false;

    FTimerHandle HealthBarTimerHandle; // Таймер для скрытия полоски здоровья
    FTimerHandle DelayTimerHandle;     // Таймер для задержки перед анимацией "отстающей" полоски

    void UpdateHealthBar();
    void ShowHealthBar();
    void HideHealthBar();
    void StartDelayedLerp();
    void HandleOwnerDeath(AActor* DamageCauser);

public:
    // Основная функция для получения урона.
    UFUNCTION(BlueprintCallable, Category = "HealthComponent")
    void TakeDamage(float DamageAmount, AActor* DamageCauser);

    // Функция для восстановления здоровья.
    UFUNCTION(BlueprintCallable, Category = "HealthComponent")
    void RestoreHealth(float Amount);

    // Функция для сброса состояния (например, при респауне).
    UFUNCTION(BlueprintCallable, Category = "HealthComponent")
    void ResetHealth();

    // Возвращает, мертв ли владелец компонента.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponent")
    bool IsDead() const { return bIsDead; }

    // Функция для первичной установки максимального здоровья. 
    UFUNCTION(BlueprintCallable, Category = "HealthComponent")
    void SetMaxHealth(float NewHealth);

    // Возращает максимальное здоровье. 
    UFUNCTION(BlueprintCallable, Category = "HealthComponent")
    float GetMaxHealth() const { return MaxHealth; }

    // Возращает текущее здоровье. 
    UFUNCTION(BlueprintCallable, Category = "HealthComponent")
    float GetHealth() const { return CurrentHealth; }
};