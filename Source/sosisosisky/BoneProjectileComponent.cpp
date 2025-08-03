// We Sir

#include "BoneProjectileComponent.h"
#include "MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"



// Sets default values for this component's properties
UBoneProjectileComponent::UBoneProjectileComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	
	// Точка спавна костянного снаряда
	ChargingOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("ChargingOrigin"));
	
	// Визуализация костянного снаряда при зарядке
	ChargingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChargingMesh"));
}


// Called when the game starts
void UBoneProjectileComponent::BeginPlay()
{
	Super::BeginPlay();


	if (OwnerCharacter) {
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
		ChargingOrigin->AttachToComponent(OwnerCharacter->GetRootComponent(), AttachmentRules);
		ChargingOrigin->SetRelativeLocation(FVector(0, 0, 100));

		ChargingMesh->AttachToComponent(ChargingOrigin, AttachmentRules);
		ChargingMesh->SetRelativeLocation(FVector::ZeroVector);
		ChargingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ChargingMesh->SetVisibility(false);
	}
	// ...
}


// Called every frame
void UBoneProjectileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	// Визуализация поввляения BoneProjectile 
	if (OwnerCharacter) {

		switch (ChargeState)
		{
		case EProjectileChargeState::Idle:
			OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = OwnerCharacter->MaxSpeed;
			break;
		case EProjectileChargeState::Charging:
		{
			OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = OwnerCharacter->MaxSpeed / 2;
			CurrentChargeTime = FMath::Min(CurrentChargeTime + DeltaTime, 3.f);

			int32 NewStage = (CurrentChargeTime < StageTime) ? 1 : FMath::Clamp(int32(CurrentChargeTime / StageTime), 1, 3);
			if (NewStage != CurrentVisualStage)
			{
				CurrentVisualStage = NewStage;
				if (BoneProjectileClass)
				{
					ABoneProjectile* DefaultProj = BoneProjectileClass.GetDefaultObject();
					UStaticMesh* NewMesh = nullptr;
					switch (CurrentVisualStage)
					{
					case 1: if (OwnerCharacter->GetResourceCount(EResourceType::Bone) >= Stage1Cost) NewMesh = DefaultProj->MeshStage1; break;
					case 2: if (OwnerCharacter->GetResourceCount(EResourceType::Bone) >= Stage2Cost) NewMesh = DefaultProj->MeshStage2; break;
					case 3: if (OwnerCharacter->GetResourceCount(EResourceType::Bone) >= Stage3Cost) NewMesh = DefaultProj->MeshStage3; break;
					}
					if (NewMesh)
					{
						ChargingMesh->SetStaticMesh(NewMesh);
					}
				}
			}

			

			if (ChargeState == EProjectileChargeState::PendingRelease && CurrentChargeTime >= StageTime)
			{
				ChargeState = EProjectileChargeState::Idle;
				ChargingMesh->SetVisibility(false);
				SpawnChargedBoneProjectile();
			}
			break;
		}
		case EProjectileChargeState::PendingRelease:
			CurrentChargeTime = FMath::Min(CurrentChargeTime + DeltaTime, 3.f);
			if (CurrentChargeTime >= StageTime) {
				ChargeState = EProjectileChargeState::Idle;
				ChargingMesh->SetVisibility(false);
				SpawnChargedBoneProjectile();
			}
			break;
		default:
			break;
		}

		if (ChargeState == EProjectileChargeState::Charging || ChargeState == EProjectileChargeState::PendingRelease) {
			FRotator ProjDirection = FRotationMatrix::MakeFromX(OwnerCharacter->MainCamera->GetForwardVector()).Rotator();
			ChargingMesh->SetWorldRotation(ProjDirection);

			if (CurrentChargeTime < StageTime)
			{
				float Alpha = CurrentChargeTime / StageTime;
				float Eased = FMath::InterpEaseInOut(0.f, Scale, Alpha, 2.f);
				ChargingMesh->SetWorldScale3D(FVector(Eased));
			}
			else
			{
				ChargingMesh->SetWorldScale3D(FVector(Scale));
			}
		}

		/* Старый способ до ввода enum
		if (bIsBoneProjectileCharging)
		{

			OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = OwnerCharacter->MaxSpeed / 2;
			CurrentChargeTime = FMath::Min(CurrentChargeTime + DeltaTime, 3.f);

			int32 NewStage;
			if (CurrentChargeTime < StageTime) NewStage = 1;
			else NewStage = FMath::Clamp(int32(CurrentChargeTime / StageTime), 1, 3);

			// Если стадия изменилась — обновляем визуал
			if (NewStage != CurrentVisualStage)
			{
				CurrentVisualStage = NewStage;

				if (BoneProjectileClass)
				{
					ABoneProjectile* DefaultProj = BoneProjectileClass.GetDefaultObject();
					UStaticMesh* NewMesh = nullptr;

					switch (CurrentVisualStage)
					{
					case 1: if (OwnerCharacter->GetResourceCount(EResourceType::Bone) >= Stage1Cost) NewMesh = DefaultProj->MeshStage1; break;
					case 2: if (OwnerCharacter->GetResourceCount(EResourceType::Bone) >= Stage2Cost) NewMesh = DefaultProj->MeshStage2; break;
					case 3: if (OwnerCharacter->GetResourceCount(EResourceType::Bone) >= Stage3Cost) NewMesh = DefaultProj->MeshStage3; break;
					}

					if (NewMesh)
					{
						ChargingMesh->SetStaticMesh(NewMesh);
					}
				}
			}

			FRotator ProjDirection = FRotationMatrix::MakeFromX(OwnerCharacter->MainCamera->GetForwardVector()).Rotator();
			ChargingMesh->SetWorldRotation(ProjDirection);

			// Анимция появления при зарядке первой стадии

			if (CurrentChargeTime < StageTime)
			{

				// Линейное масштабирование
				//
				//float Scale = CurrentChargeTime / StageTime;
				//ChargingMesh->SetWorldScale3D(FVector(Scale));


				// Плавное масштабирование по кривой

				float Alpha = CurrentChargeTime / StageTime;
				float Eased = FMath::InterpEaseInOut(0.f, Scale, Alpha, 2.f);
				ChargingMesh->SetWorldScale3D(FVector(Eased));
			}
			else
			{
				ChargingMesh->SetWorldScale3D(FVector(Scale));
			}


			// Если кнопку отпустили раньше времени, всё равно ждём 

			if (bIsButtonReleasedEarly && CurrentChargeTime >= StageTime) {

				bIsButtonReleasedEarly = false;
				bIsBoneProjectileCharging = false;

				ChargingMesh->SetVisibility(false);
				SpawnChargedBoneProjectile();
			}
		}
		else OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = OwnerCharacter->MaxSpeed; */
	}
}

void UBoneProjectileComponent::PressAbility()
{
	ChargingBoneProjectilePressed();
}

void UBoneProjectileComponent::ReleaseAbility()
{
	ChargingBoneProjectileReleased();
}

void UBoneProjectileComponent::ChargingBoneProjectilePressed()
{
	if (bIsAbilityLocked || !OwnerCharacter) return;

	if (OwnerCharacter->GetResourceCount(EResourceType::Bone) >= Stage1Cost) {
		//if (!bIsBoneProjectileCharging)											Старый способ до ввода enum
		//{
		//	bIsBoneProjectileCharging = true;
		//	CurrentChargeTime = 0.f;
		//
		//	CurrentVisualStage = 1; // начинаем с первой стадии
		//
			// Получаем меш первой стадии из дефолтного объекта снаряда
		//	if (BoneProjectileClass)
		//	{
		//		ABoneProjectile* DefaultProj = BoneProjectileClass.GetDefaultObject();
		//		if (DefaultProj && DefaultProj->MeshStage1)
		//		{
		//			ChargingMesh->SetStaticMesh(DefaultProj->MeshStage1);
		//		}
		//	}

			// Показываем меш над головой
		//	ChargingMesh->SetVisibility(true);
		//}

		if (ChargeState == EProjectileChargeState::Idle)
		{
			ChargeState = EProjectileChargeState::Charging;
			CurrentChargeTime = 0.f;
			CurrentVisualStage = 1;

			if (BoneProjectileClass)
			{
				ABoneProjectile* DefaultProj = BoneProjectileClass.GetDefaultObject();
				if (DefaultProj && DefaultProj->MeshStage1)
				{
					ChargingMesh->SetStaticMesh(DefaultProj->MeshStage1);
				}
			}

			ChargingMesh->SetVisibility(true);
		}
	}
}

void UBoneProjectileComponent::ChargingBoneProjectileReleased()
{
	/* Старый способ до ввода enum
	if (CurrentChargeTime < StageTime) {
		bIsButtonReleasedEarly = true;
	}
	else
	{
		bIsBoneProjectileCharging = false;
		bIsButtonReleasedEarly = false;
		ChargingMesh->SetVisibility(false);
		SpawnChargedBoneProjectile();
	}*/

	if (ChargeState == EProjectileChargeState::Charging)
	{
		if (CurrentChargeTime < StageTime)
		{
			ChargeState = EProjectileChargeState::PendingRelease;
		}
		else
		{
			ChargeState = EProjectileChargeState::Idle;
			ChargingMesh->SetVisibility(false);
			SpawnChargedBoneProjectile();
		}
	}
}

void UBoneProjectileComponent::SpawnChargedBoneProjectile()
{
	/* Старый способ до ввода enum
	if (!BoneProjectileClass) return;

	// Определяем стадию: 0–2 сек - Stage=1, 2–3 сек - Stage=2, 3+ сек - Stage=3	
	int32 Stage;
	if (CurrentChargeTime < StageTime) Stage = 1;
	else Stage = FMath::Clamp(int32(CurrentChargeTime / StageTime), 1, 3);

	bool isStageSolved = false;
	do
	{
		switch (Stage)
		{
		case 1: if (!OwnerCharacter->RemoveResources(EResourceType::Bone, Stage1Cost))
		{
			return;
		}
			  else isStageSolved = true;
			break;
		case 2: if (!OwnerCharacter->RemoveResources(EResourceType::Bone, Stage2Cost))
		{
			Stage = 1;
		}
			  else isStageSolved = true;
			break;
		case 3: if (!OwnerCharacter->RemoveResources(EResourceType::Bone, Stage3Cost))
		{
			Stage = 2;
		}
			  else isStageSolved = true;
			break;
		default:
			break;
		}
	} while (!isStageSolved);


	// Позиция над головой: можно взять Socket в скелете или просто смещение
	FVector SpawnLoc = ChargingOrigin->GetComponentLocation();
	FRotator SpawnRot = OwnerCharacter->GetControlRotation();

	FActorSpawnParameters Params;
	Params.Instigator = OwnerCharacter;
	Params.Owner = OwnerCharacter;

	ABoneProjectile* Proj = GetWorld()->SpawnActor<ABoneProjectile>(
		BoneProjectileClass, SpawnLoc, SpawnRot, Params);

	if (Proj)
	{
		Proj->InitCharge(Stage, SpawnRot.Vector(), OwnerCharacter);
	}*/

	if (!BoneProjectileClass || !OwnerCharacter) return;

	int32 Stage = (CurrentChargeTime < StageTime) ? 1 : FMath::Clamp(int32(CurrentChargeTime / StageTime), 1, 3);
	bool bStageSolved = false;
	do
	{
		switch (Stage)
		{
		case 1: if (!OwnerCharacter->RemoveResources(EResourceType::Bone, Stage1Cost)) return; else bStageSolved = true; break;
		case 2: if (!OwnerCharacter->RemoveResources(EResourceType::Bone, Stage2Cost)) Stage = 1; else bStageSolved = true; break;
		case 3: if (!OwnerCharacter->RemoveResources(EResourceType::Bone, Stage3Cost)) Stage = 2; else bStageSolved = true; break;
		default: break;
		}
	} while (!bStageSolved);

	FVector SpawnLoc = ChargingOrigin->GetComponentLocation();
	FRotator SpawnRot = OwnerCharacter->GetControlRotation();

	FActorSpawnParameters Params;
	Params.Instigator = OwnerCharacter;
	Params.Owner = OwnerCharacter;

	ABoneProjectile* Proj = GetWorld()->SpawnActor<ABoneProjectile>(BoneProjectileClass, SpawnLoc, SpawnRot, Params);

	if (Proj)
	{
		Proj->InitCharge(Stage, SpawnRot.Vector(), OwnerCharacter);
	}

	CurrentChargeTime = 0.f;
	CurrentVisualStage = 0;
	bIsAbilityLocked = true;
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { bIsAbilityLocked = false; });
}