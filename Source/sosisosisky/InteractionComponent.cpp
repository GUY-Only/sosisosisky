// We Sir


#include "InteractionComponent.h"
#include "InteractableActor.h"
#include "MainCharacter.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CurrentInteractable = nullptr;
	PreviousInteractable = nullptr;

	// ...
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	OwnerCharacter = Cast<AMainCharacter>(GetOwner());

	if (OwnerCharacter)
	{
		OwnerCamera = OwnerCharacter->MainCamera;
	}
	
}


// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerCamera)
	{
		TraceForInteractables();
	}
}

void UInteractionComponent::TraceForInteractables()
{

	if (!OwnerCamera) return;

	FVector Start = OwnerCamera->GetComponentLocation();
	FVector End = Start + (OwnerCamera->GetForwardVector() * RayDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	//DrawDebugSphere(GetWorld(), End, RayRadius, 12, FColor::Green, true, 0.1f);	Дебаг сфера на конце луча с его радиусом.

	TArray<FHitResult> Hits;
	// bool bHit = GetWorld()->LineTraceMultiByChannel(Hits, Start, End, ECC_Visibility, Params);  тонкий луч
	bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(RayRadius),
		Params
	);

	AActor* ValidInteractable = nullptr;

	if (bHit)
	{
		for (const FHitResult& Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;


			// Пропускаем акторы-исключения
			bool bIsIgnored = false;
			for (TSubclassOf<AActor> IgnoreClass : IgnoreActorClasses)
			{
				if (HitActor->IsA(IgnoreClass))
				{
					bIsIgnored = true;
					break;
				}
			}
			if (bIsIgnored)
			{
				continue;
			}


			// Проверяем, реализует ли интерфейс взаимодействия
			if (HitActor->Implements<UInteractionInterface>())
			{
				ValidInteractable = HitActor;
				break;  // Первый подходящий объект
			}
		}
	}

	if (ValidInteractable != CurrentInteractable)
	{
		CurrentInteractable = ValidInteractable;

		if (CurrentInteractable)
		{
			FString InteractionText = IInteractionInterface::Execute_GetInteractionText(CurrentInteractable);
			ShowInteractionUI(InteractionText);
		}
		else
		{
			HideInteractionUI();
		}
	}
}

// Вызов взаимодействия
void UInteractionComponent::Interact()
{
	if (CurrentInteractable && CurrentInteractable->Implements<UInteractionInterface>())
	{
		IInteractionInterface::Execute_OnInteract(CurrentInteractable, GetOwner());
	}
}

void UInteractionComponent::ShowInteractionUI(const FString& InteractionText)
{
	UE_LOG(LogTemp, Log, TEXT("Show UI: %s"), *InteractionText);
	PreviousInteractable = CurrentInteractable;
	if (auto* IA = Cast<AInteractableActor>(CurrentInteractable))
	{
		IA->ShowUI();
	}
}

void UInteractionComponent::HideInteractionUI()
{
	UE_LOG(LogTemp, Log, TEXT("Hide UI"));
	if (auto* IA = Cast<AInteractableActor>(PreviousInteractable))
	{
		IA->HideUI();
	}

}
