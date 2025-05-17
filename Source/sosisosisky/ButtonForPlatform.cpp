// We Sir


#include "ButtonForPlatform.h"
#include "Engine/Engine.h"

AButtonForPlatform::AButtonForPlatform()
{
    InteractionText = TEXT("Press F to interact");

    ButtonMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    ButtonMeshComponent->SetupAttachment(RootComponent);

}

void AButtonForPlatform::OnInteract_Implementation(AActor* Caller)
{
    TargetPlatform->Ineraction();
}