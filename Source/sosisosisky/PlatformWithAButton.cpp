// We Sir


#include "PlatformWithAButton.h"
#include "Components/BoxComponent.h"


APlatformWithAButton::APlatformWithAButton()
{
	PrimaryActorTick.bCanEverTick = true;

	SetMobility(EComponentMobility::Movable);

	TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	TriggerComponent->SetupAttachment(RootComponent);

	TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &APlatformWithAButton::Triggered);
	TriggerComponent->OnComponentEndOverlap.AddDynamic(this, &APlatformWithAButton::Untriggered);

	ButtonMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMeshComponent->SetupAttachment(RootComponent);

}

void APlatformWithAButton::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) 
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	GStartLocation = GetActorLocation();
	GTargetLocation = GetTransform().TransformPosition(TargetLocation);

	OriginalButtonLocation = ButtonMeshComponent->GetRelativeLocation();
}

void APlatformWithAButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bMoving) 
	{
		FVector Location = GetActorLocation();
		float Length = (GTargetLocation - GStartLocation).Size();
		float Travalled = (Location - GStartLocation).Size();

		FVector Dir = (GTargetLocation - GStartLocation).GetSafeNormal();
		Location += Speed * DeltaTime * Dir;
		SetActorLocation(Location);

		if (Travalled >= Length) 
		{
			bMoving = false;

			SetActorLocation(GTargetLocation);

			FVector Swap = GStartLocation;
			GStartLocation = GTargetLocation;
			GTargetLocation = Swap;

			if (!bTriggered) ReleaseButton();
			
		}
	}
}

void APlatformWithAButton::Triggered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr) {
		bTriggered = true;
		if (bCanPressAgain) {
			bMoving = true;
			PressButton();
		}
	}
}

void APlatformWithAButton::Untriggered(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr) {
		bTriggered = false;
		if (!bMoving) ReleaseButton();
		
	}
}

void APlatformWithAButton::PressButton()
{
	if (ButtonMeshComponent)
	{
		ButtonMeshComponent->SetRelativeLocation(OriginalButtonLocation + FVector(0.0f, 0.0f, PressOffset));
	}
	bCanPressAgain = false;
}

void APlatformWithAButton::ReleaseButton()
{
	if (ButtonMeshComponent)
	{
		ButtonMeshComponent->SetRelativeLocation(OriginalButtonLocation);
	}
	bCanPressAgain = true;
}

void APlatformWithAButton::Ineraction()
{
	if (bCanPressAgain) {
		bMoving = true;
		PressButton();
	}
}
