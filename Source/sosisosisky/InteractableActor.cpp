// We Sir


#include "InteractableActor.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableActor::OnInteract_Implementation(AActor* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("Interacted with: %s"), *GetName());
}

FString AInteractableActor::GetInteractionText_Implementation() const
{
	return InteractionText;
}

