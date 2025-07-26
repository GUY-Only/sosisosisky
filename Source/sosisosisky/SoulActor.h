// We Sir

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoulActor.generated.h"

class UHealthComponent;
class UNiagaraComponent;
class USphereComponent;

UCLASS()
class SOSISOSISKY_API ASoulActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoulActor();

	UPROPERTY(EditAnywhere, Category = "Soul Actor | Params")
	float LifeTime = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Soul Actor | Params")
	int Bounty = 1;

	UPROPERTY(EditAnywhere, Category = "Soul Actor | Params")
	float SoulHP = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Soul Actor | Params")
	float SoulRadius = 50.0f;

	// Устанавливает параметры души
	void SetParams(float NewLifeTime, int NewBounty, float NewSoulHP, float NewSoulRadius);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	// Провряет, не мертва ли цель
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Soul Actor | Logic")
	bool IsDead() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Вызывается при смерти цели
	UFUNCTION()
	void OnDeath(AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Soul Actor | Components")
	UHealthComponent* HealthComponent;

	FTimerHandle DespawnTimer;

	// Удаляет душу из мира
	void Despawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals")
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	USphereComponent* SphereCollider;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
