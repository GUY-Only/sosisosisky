// We Sir


#include "BeamTest.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
ABeamTest::ABeamTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BeamVFX = nullptr;
	ActiveBeamComponent = nullptr;

	BeamStartParamName = FName("BeamStart");
	BeamEndParamName = FName("BeamEnd");

	BeamStartTangentParamName = FName("BeamStartTangent");
	BeamEndTangentParamName = FName("BeamEndTangent");

	USceneComponent* DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = DummyRoot;

}

// Called when the game starts or when spawned
void ABeamTest::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void ABeamTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ActiveBeamComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		BeamVFX,
		GetRootComponent(),
		NAME_None,
		BeamStart,
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true);

	if (ActiveBeamComponent)
	{

		ActiveBeamComponent->SetVectorParameter(BeamStartParamName, GetActorLocation() + BeamStart);
		ActiveBeamComponent->SetVectorParameter(BeamEndParamName, GetActorLocation() + BeamEnd);


		const FVector StartTangent = BeamStartRotation.RotateVector(GetActorForwardVector()) * TangentStrength;
		const FVector EndTangent = ((GetActorLocation() + BeamStart) - (GetActorLocation() + BeamEnd)).GetSafeNormal() * TangentStrength;

		ActiveBeamComponent->SetVectorParameter(BeamStartTangentParamName, StartTangent);
		ActiveBeamComponent->SetVectorParameter(BeamEndTangentParamName, EndTangent);
	}
}

