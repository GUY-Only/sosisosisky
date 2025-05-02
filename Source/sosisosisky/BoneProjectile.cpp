// We Sir


#include "BoneProjectile.h"
#include "Components/BoxComponent.h"

// Sets default values
ABoneProjectile::ABoneProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

	//SetMobility(EComponentMobility::Movable);

	TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	TriggerComponent->SetupAttachment(RootComponent);

	TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ABoneProjectile::Triggered);
}

void ABoneProjectile::Triggered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Destroy();
}

// Called when the game starts or when spawned
void ABoneProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoneProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*Vector Location = GetActorLocation();
	Location += Speed * DeltaTime * FVector(0,1,0);
	SetActorLocation(Location);*/

}

