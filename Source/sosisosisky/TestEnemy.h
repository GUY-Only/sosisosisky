// We Sir

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "TestEnemy.generated.h"

UCLASS()
class SOSISOSISKY_API ATestEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestEnemy();

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
    // Переопределяем TakeDamage
    virtual float TakeDamage(
        float DamageAmount,
        const FDamageEvent& DamageEvent,
        AController* EventInstigator,
        AActor* DamageCauser
    ) override;

protected:
    // Например, хп
    UPROPERTY(EditAnywhere, Category = "Stats")
    float Health = 100.f;

};
