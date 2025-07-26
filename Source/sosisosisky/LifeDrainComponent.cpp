// We Sir


#include "LifeDrainComponent.h"
#include "MainCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "SoulDamageType.h"

// Sets default values for this component's properties
ULifeDrainComponent::ULifeDrainComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaxBeamDistance = 3000.0f;
	MaxTargetingAngle = 15.0f;
	DrainAmountPerSecond = 50.0f;
	BeamOriginOffset = FVector(0, 0, 80.0f);
	BeamVFX = nullptr;
	ActiveBeamComponent = nullptr;
	bIsAttacking = false;
	BeamStartParamName = FName("BeamStart");
	BeamEndParamName = FName("BeamEnd");
	LineOfSightTraceChannel = ECC_Visibility;

	TangentStrength = 1000.0f; // Хорошее значение для начала, можно будет поменять в редакторе
	BeamStartTangentParamName = FName("BeamStartTangent");
	BeamEndTangentParamName = FName("BeamEndTangent");
}

void ULifeDrainComponent::PressAttack()
{

	/*if (bIsAttacking)
	{
		return;
	}*/

	if (AActor* BestTarget = FindBestTarget())
	{
		UE_LOG(LogTemp, Warning, TEXT("LifeDrainComponent: Target: %s"), *BestTarget->GetName());
		StartDraining(BestTarget);
	}
}

void ULifeDrainComponent::ReleaseAttack()
{
	if (bIsAttacking)
	{
		StopDraining(false);
	}
}


// Called when the game starts
void ULifeDrainComponent::BeginPlay()
{
	Super::BeginPlay();

	MaxTargetingCosine = FMath::Cos(FMath::DegreesToRadians(MaxTargetingAngle));

	SetComponentTickEnabled(false);
	
}


// Called every frame
void ULifeDrainComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsAttacking && IsTargetStillValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Sucking..."));
		UpdateBeam();
		// TODO: Здесь можно добавить логику самого высасывания жизни.
		UGameplayStatics::ApplyDamage(CurrentTarget.Get(), DrainAmountPerSecond * DeltaTime, OwnerCharacter->GetController(), OwnerCharacter, USoulDamageType::StaticClass());
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Ain't sucking :c"));
		//StopDraining();		Вместо окончательного обрыва луча, смотрим, нет ли новой подходящей цели

		if (AActor* BestTarget = FindBestTarget())
		{
			UE_LOG(LogTemp, Warning, TEXT("LifeDrainComponent: Target: %s"), *BestTarget->GetName());
			StopDraining(true);
			StartDraining(BestTarget);
		}
		else 
		{
			StopDraining(false);
		}
	}
}

AActor* ULifeDrainComponent::FindBestTarget() const
{
	
	if (!OwnerCharacter) return nullptr;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || !PC->PlayerCameraManager) return nullptr;


	const APlayerCameraManager* CameraManager = PC->PlayerCameraManager;
	const FVector CameraLocation = CameraManager->GetCameraLocation();
	const FVector CameraForward = CameraManager->GetCameraRotation().Vector();

	
	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASoulActor::StaticClass(), AllEnemies); //[25]

	UE_LOG(LogTemp, Warning, TEXT("Enemies on scene: %d"), AllEnemies.Num());

	AActor* BestTarget = nullptr;
	float MaxDotProduct = MaxTargetingCosine;

	const FVector TraceStart = GetBeamOrigin();

	for (AActor* Enemy : AllEnemies)
	{
		//if (Enemy == OwnerCharacter || !Enemy->Implements<UInterface>()) continue; // Пропускаем себя и невалидных врагов

		ASoulActor* EnemyActor = Cast<ASoulActor>(Enemy);
		if (!EnemyActor || EnemyActor->IsDead())
		{
			continue;
		}

		const FVector PlayerForward = OwnerCharacter->GetActorForwardVector();
		const FVector ToEnemyFromPlayerDir = (Enemy->GetActorLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal();
		// Проверяем, что враг находится в передней полусфере персонажа (угол < 90 градусов)
		if (FVector::DotProduct(PlayerForward, ToEnemyFromPlayerDir) < 0)
		{
			continue; // Враг за спиной персонажа
		}

		// 1. Проверка дистанции до врага
		const FVector EnemyLocation = Enemy->GetActorLocation();
		const float Distance = FVector::Dist(TraceStart, EnemyLocation);
		if (Distance > MaxBeamDistance)
		{
			continue;
		}

		// 2. Проверка близости к центру экрана
		const FVector ToEnemyDir = (EnemyLocation - CameraLocation).GetSafeNormal();
		const float DotProduct = FVector::DotProduct(CameraForward, ToEnemyDir);

		if (DotProduct > MaxDotProduct)
		{
			// 3. Проверка прямой видимости (Line of Sight)
			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(OwnerCharacter);

			GetWorld()->LineTraceSingleByChannel(
				HitResult,
				TraceStart,
				EnemyLocation,
				LineOfSightTraceChannel,
				QueryParams);

			if (!HitResult.bBlockingHit || HitResult.GetActor() == Enemy)
			{
				BestTarget = Enemy;
				MaxDotProduct = DotProduct; // Обновляем лучший результат
			}
		}
	}

	return BestTarget;
}

void ULifeDrainComponent::UpdateBeam()
{
	
	if (ActiveBeamComponent && CurrentTarget.IsValid())
	{
		const FVector StartPoint = GetBeamOrigin();
		const FVector EndPoint = CurrentTarget->GetActorLocation();

		ActiveBeamComponent->SetVectorParameter(BeamStartParamName, StartPoint);
		ActiveBeamComponent->SetVectorParameter(BeamEndParamName, EndPoint);


		const FVector StartTangent = OwnerCharacter->GetActorForwardVector() * TangentStrength;
		const FVector EndTangent = (StartPoint - EndPoint).GetSafeNormal() * TangentStrength;

		ActiveBeamComponent->SetVectorParameter(BeamStartTangentParamName, StartTangent);
		ActiveBeamComponent->SetVectorParameter(BeamEndTangentParamName, EndTangent);
	}
}

bool ULifeDrainComponent::IsTargetStillValid() const
{
	UE_LOG(LogTemp, Warning, TEXT("LifeDrainComponent: IsTargetStillValid()"));
	if (!CurrentTarget.IsValid()) return false;

	const FVector StartLocation = GetBeamOrigin();
	const FVector EndLocation = CurrentTarget->GetActorLocation();

	// Проверка дистанции
	const float Distance = FVector::Dist(StartLocation, EndLocation);
	if (Distance > MaxBeamDistance) return false;

	ASoulActor* TargetEnemy = Cast<ASoulActor>(CurrentTarget.Get());
	if (TargetEnemy && TargetEnemy->IsDead())
	{
		return false; // Прерываем луч, если цель мертва
	}

	/*	Обрыв луча в этом случае выглядит глупо, так как персонаж физически ещё смотрит на противника. У нас тут не киберспирт, в конце концов
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && PC->PlayerCameraManager)
	{
		const FVector CameraForward = PC->PlayerCameraManager->GetCameraRotation().Vector();
		const FVector ToTargetDir = (EndLocation - PC->PlayerCameraManager->GetCameraLocation()).GetSafeNormal();
		if (FVector::DotProduct(CameraForward, ToTargetDir) < MaxTargetingCosine)
		{
			return false; // Цель вышла за пределы конуса прицеливания
		}
	}*/

	const FVector PlayerForward = OwnerCharacter->GetActorForwardVector();
	const FVector ToTargetFromPlayerDir = (EndLocation - OwnerCharacter->GetActorLocation()).GetSafeNormal();
	if (FVector::DotProduct(PlayerForward, ToTargetFromPlayerDir) < 0)
	{
		return false; // Игрок отвернулся от цели спиной
	}

	// Проверка прямой видимости
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		GetBeamOrigin(),
		CurrentTarget->GetActorLocation(),
		LineOfSightTraceChannel,
		QueryParams);

	return !HitResult.bBlockingHit || HitResult.GetActor() == CurrentTarget.Get();
}

void ULifeDrainComponent::StartDraining(AActor* Target)
{
	
	if (!OwnerCharacter || !Target || !BeamVFX) return;

	bIsAttacking = true;
	CurrentTarget = Target;
	SetComponentTickEnabled(true);

	const FVector StartLocation = GetBeamOrigin();

	// Создаем и прикрепляем систему частиц Niagara к нашему персонажу.
	ActiveBeamComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		BeamVFX,
		OwnerCharacter->GetRootComponent(),
		NAME_None,
		BeamOriginOffset,
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true); //[9, 12]

		if (ActiveBeamComponent)
		{
			UpdateBeam();
		}
}

void ULifeDrainComponent::StopDraining(bool isTargetAutoSwaped)
{
	UE_LOG(LogTemp, Warning, TEXT("StopDraining()"));
	if (!bIsAttacking) return;
	if (!isTargetAutoSwaped)
	{
		bIsAttacking = false;
	}
	CurrentTarget = nullptr;
	SetComponentTickEnabled(false);

	if (ActiveBeamComponent)
	{
		ActiveBeamComponent->Deactivate();
		ActiveBeamComponent = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Deactivate Beam"));
	}
}

FVector ULifeDrainComponent::GetBeamOrigin() const
{
	if (!OwnerCharacter)
	{
		return GetOwner()->GetActorLocation();
	}
	
	return OwnerCharacter->GetActorLocation() + BeamOriginOffset;
}



