// We Sir

#include "InteractableActor.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	//InteractionWidget->SetWidgetSpace(EWidgetSpace::World);
	//InteractionWidget->SetDrawSize(FVector2D(200.f, 50.f));
	InteractionWidget->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	InteractionWidget->SetVisibility(false);
	InteractionWidget->SetDrawAtDesiredSize(true);

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(
		TEXT("/Game/Interactables/WBP_InteractionHint")
	);
	if (WidgetBPClass.Class)
	{
		InteractionWidget->SetWidgetClass(WidgetBPClass.Class);
	}

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

void AInteractableActor::ShowUI()
{
	if (!InteractionWidget) return;
	UpdateInteractionText();
	InteractionWidget->SetVisibility(true);
}

void AInteractableActor::HideUI()
{
	if (!InteractionWidget) return;
	InteractionWidget->SetVisibility(false);
}

void AInteractableActor::UpdateInteractionText()
{
	if (!InteractionWidget) return;

	if (UUserWidget* UW = InteractionWidget->GetUserWidgetObject())
	{
		if (UTextBlock* TB = Cast<UTextBlock>(UW->GetWidgetFromName(TEXT("TextBlock_Hint"))))
		{
			TB->SetText(FText::FromString(InteractionText));
		}
		if (UTextBlock* TB = Cast<UTextBlock>(UW->GetWidgetFromName(TEXT("TextBlock_InputKey"))))
		{
			TB->SetText(GetFirstKeyForAction(GetWorld()->GetFirstPlayerController(), "Interact").GetDisplayName());
		}
	}
}

FKey AInteractableActor::GetFirstKeyForAction(APlayerController* PlayerController, const FName& ActionName)
{
	if (!PlayerController) return FKey();

	const UPlayerInput* PlayerInput = PlayerController->PlayerInput;
	if (!PlayerInput) return FKey();

	const TArray<FInputActionKeyMapping>& Mappings = PlayerInput->GetKeysForAction(ActionName);

	if (Mappings.Num() > 0)
	{
		return Mappings[0].Key; // Первая клавиша
	}

	return FKey(); // Возврат "пустой" клавиши, если ничего не найдено
}

