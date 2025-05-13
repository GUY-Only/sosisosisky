// We Sir


#include "BoneProjectile.h"
#include "Kismet/GameplayStatics.h"            // ApplyPointDamage
#include "GameFramework/Controller.h"          // AController
#include "Engine/World.h"                      // SetLifeSpan, GetWorld

// Sets default values
ABoneProjectile::ABoneProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
    RootComponent = MeshComp;
    MeshComp->SetCollisionProfileName("BlockAllDynamic");
    MeshComp->OnComponentHit.AddDynamic(this, &ABoneProjectile::OnHit);
    MeshComp->SetWorldScale3D(FVector(Scale));

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjMove");
    ProjectileMovement->InitialSpeed = Speed;
    ProjectileMovement->MaxSpeed = Speed;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = Gravity;

}

void ABoneProjectile::InitCharge(int32 InStage, const FVector& Direction)
{
    Stage = InStage;

    // Меш по стадии
    switch (Stage)
    {
    case 1:
        MeshComp->SetStaticMesh(MeshStage1);
        ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
        break;
    case 2:
        MeshComp->SetStaticMesh(MeshStage2);
        ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
        break;
    case 3:
        MeshComp->SetStaticMesh(MeshStage3);
        ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
        break;
    }
}

void ABoneProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (Other && Other != GetInstigator())
    {
        
        float Damage = 0.f;
        switch (Stage)
        {
        case 1: Damage = DamageStage1; break;
        case 2: Damage = DamageStage2; break;
        case 3: Damage = DamageStage3; break;
        }

        UGameplayStatics::ApplyPointDamage(Other, Damage, GetVelocity().GetSafeNormal(),
            Hit, GetInstigatorController(), this, nullptr);
    }

    // Уничтожаемся при любом столкновении
    Destroy();
}

// Called when the game starts or when spawned
void ABoneProjectile::BeginPlay()
{
	Super::BeginPlay();
    SetLifeSpan(5.f);  // Автоудаление через 5 сек
}

// Called every frame
void ABoneProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

