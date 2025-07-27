// We Sir


#include "SoulActor.h"
#include "SoulDamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "HealthComponent.h"
#include "MainCharacter.h"
#include "NiagaraComponent.h" 
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ASoulActor::ASoulActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	//USceneComponent* DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	//RootComponent = DummyRoot;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	RootComponent = SphereCollider;

	SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // ¬ключаем коллизию только дл€ запросов (лучей, оверлапов)
	SphereCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);


	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	
}

void ASoulActor::SetParams(int NewBounty, float NewLifeTime, float NewSoulHP, float NewSoulRadius)
{
	LifeTime = NewLifeTime;
	Bounty = NewBounty;
	HealthComponent->SetMaxHealth(NewSoulHP);
	SoulRadius = NewSoulRadius;
}

void ASoulActor::SetParams(int NewBounty, float NewLifeTime, float NewSoulHP)
{
	LifeTime = NewLifeTime;
	Bounty = NewBounty;
	HealthComponent->SetMaxHealth(NewSoulHP);
}

void ASoulActor::SetParams(int NewBounty, float NewLifeTime)
{
	LifeTime = NewLifeTime;
	Bounty = NewBounty;
}

void ASoulActor::SetParams(int NewBounty)
{
	Bounty = NewBounty;
}

float ASoulActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.DamageTypeClass && DamageEvent.DamageTypeClass->IsChildOf(USoulDamageType::StaticClass())) {

		if (HealthComponent)
		{
			HealthComponent->TakeDamage(ActualDamage, DamageCauser);
		}

		return ActualDamage;
	}
	return 0.0f;
}

bool ASoulActor::IsDead() const
{
	if (HealthComponent)
	{
		return HealthComponent->IsDead();
	}
	return true;
}

// Called when the game starts or when spawned
void ASoulActor::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnHealthEnded.AddDynamic(this, &ASoulActor::OnDeath);
	}

	if (LifeTime > .0f) 
	{
		GetWorld()->GetTimerManager().ClearTimer(DespawnTimer);
		GetWorld()->GetTimerManager().SetTimer(DespawnTimer, this, &ASoulActor::Despawn, LifeTime, false);
	}

	SphereCollider->SetSphereRadius(SoulRadius);
	
}

void ASoulActor::OnDeath(AActor* DamageCauser)
{
	if (auto* MC = Cast<AMainCharacter>(DamageCauser))
	{
		MC->AddResources(EResourceType::Soul, Bounty);
	}
	Despawn();
}

void ASoulActor::Despawn()
{
	Destroy();
}

// Called every frame
void ASoulActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

