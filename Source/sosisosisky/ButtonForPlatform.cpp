// We Sir


#include "ButtonForPlatform.h"
#include "Engine/Engine.h"

AButtonForPlatform::AButtonForPlatform()
{
    //InteractionText = TEXT("Послать нахуй платформу");

    ButtonMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    //ButtonMeshComponent->SetupAttachment(RootComponent);
    RootComponent = ButtonMeshComponent;

    InteractionWidget->SetupAttachment(ButtonMeshComponent);
    InteractionWidget->SetRelativeLocation(FVector(0, 0, 100));
    

}

void AButtonForPlatform::OnInteract_Implementation(AActor* Caller)
{
    TargetPlatform->Ineraction();
}