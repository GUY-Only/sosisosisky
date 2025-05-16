// We Sir


#include "ButtonForPlatform.h"
#include "Engine/Engine.h"

AButtonForPlatform::AButtonForPlatform()
{
    // «начение по умолчанию дл€ подсказки
    InteractionText = TEXT("Press F to interact");

    ButtonMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    ButtonMeshComponent->SetupAttachment(RootComponent);

}

void AButtonForPlatform::OnInteract_Implementation(AActor* Caller)
{
    // ѕример: лог в Output Log
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1, 2.f, FColor::Green,
            FString::Printf(TEXT("Interacted: %s"), *GetName())
        );
    }

    // “ут можно добавить любую логику: открыть дверь, запустить анимацию и т.д.

    TargetPlatform->Ineraction();
}