// We Sir


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraDistance = 400.0f;
	RotationRate = 1000.0f;
	JumpVel = 700.0f;
	MaxSpeed = 400.0f;

	bCanMove = true;


	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));	//создаётсяы объект USpringArmComponent и называется CameraBoom
	CameraBoom->SetupAttachment(RootComponent);	//через -> вызываются функции и переменные объекта под указателем. Закрепляем спрингарм на персонаже
	CameraBoom->TargetArmLength = CameraDistance;
	CameraBoom->bUsePawnControlRotation = true;

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	MainCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, RotationRate,0.0f);
	GetCharacterMovement()->JumpZVelocity = JumpVel;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
	GetCharacterMovement()->SetWalkableFloorAngle(60);

	// Точка спавна костянного снаряда
	ChargingOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("ChargingOrigin"));
	ChargingOrigin->SetupAttachment(RootComponent);
	ChargingOrigin->SetRelativeLocation(FVector(0, 0, 100));

	// Визуализация костянного снаряда при зарядке
	ChargingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChargingMesh"));
	ChargingMesh->SetupAttachment(ChargingOrigin);
	ChargingMesh->SetRelativeLocation(FVector::ZeroVector);
	ChargingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ChargingMesh->SetVisibility(false);

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsBoneProjectileCharging)
	{

		GetCharacterMovement()->MaxWalkSpeed = MaxSpeed/2;

		// Увеличиваем время зарядки, максимум до 3 секунд (3 стадии)
		CurrentChargeTime = FMath::Min(CurrentChargeTime + DeltaTime, 3.f);

		// Вычисляем стадию: каждая секунда — новая
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

				// Выбираем нужный меш по стадии
				switch (CurrentVisualStage)
				{
				case 1: NewMesh = DefaultProj->MeshStage1; break;
				case 2: NewMesh = DefaultProj->MeshStage2; break;
				case 3: NewMesh = DefaultProj->MeshStage3; break;
				}

				// Если нашли меш — меняем его
				if (NewMesh)
				{
					ChargingMesh->SetStaticMesh(NewMesh);
				}
			}
		}

		// Анимция появления при зарядке первой стадии

		if (CurrentChargeTime < StageTime)
		{
			
			/* Линейное масштабирование 
			* 
			float Scale = CurrentChargeTime / StageTime;		
			ChargingMesh->SetWorldScale3D(FVector(Scale));*/


			// Плавное масштабирование по кривой

			float Alpha = CurrentChargeTime / StageTime;
			float Eased = FMath::InterpEaseInOut(0.f, Scale, Alpha, 2.f);
			ChargingMesh->SetWorldScale3D(FVector(Eased));
		}
		else
		{
			// После первой секунды оставляем нормальный размер
			ChargingMesh->SetWorldScale3D(FVector(Scale));
		}

		// Если кнопку отпустили раньше времени, всё равно ждём 

		if (bIsButtonReleasedEarly && CurrentChargeTime >= StageTime) {

			bIsButtonReleasedEarly = false;
			bIsBoneProjectileCharging = false;

			ChargingMesh->SetVisibility(false);
			SpawnChargedBoneProjectile();
		}
	}else GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//оси и кнопки задаются в настройках проекта в движке

	PlayerInputComponent->BindAxis("MouseX", this, &APawn::AddControllerYawInput);	//берём информацию оси и передаём её в функцию
	PlayerInputComponent->BindAxis("MouseY", this, &APawn::AddControllerPitchInput);
	

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);	//проверяем нажатие кнопки и выполняем функцию
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainCharacter::StopJumping);

	PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &AMainCharacter::ChargingBoneProjectilePressed);
	PlayerInputComponent->BindAction("Ability1", IE_Released, this, &AMainCharacter::ChargingBoneProjectileReleased);
}


void AMainCharacter::MoveForward(float Axis)
{
	if ((Controller != NULL) && (bCanMove) && (Axis != 0.0f)) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRot(0, Rotation.Yaw, 0);
		const FVector Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);	//определяем направление камеры по оси Х

		AddMovementInput(Dir, Axis); //перемещаем персонажа
	}
}

void AMainCharacter::MoveRight(float Axis)
{
	if (Controller != NULL && bCanMove && Axis != 0.0f) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRot(0, Rotation.Yaw, 0);
		const FVector Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y); //определяем направление камеры по оси Y

		AddMovementInput(Dir, Axis);
	}
}

void AMainCharacter::Jump()
{
	if (Controller != NULL && bCanMove) ACharacter::Jump();
}

void AMainCharacter::StopJumping()
{
	if (Controller != NULL) ACharacter::StopJumping();
}


//Абилки

void AMainCharacter::Ability1Pressed()		//Задел на будущее на случай добавления новых абилок и их переключения
{
	ChargingBoneProjectilePressed();
}

void AMainCharacter::Ability1Released()
{
	ChargingBoneProjectileReleased();
}


//Снаряд

void AMainCharacter::ChargingBoneProjectilePressed()
{
	if (!bIsBoneProjectileCharging)
	{
		bIsBoneProjectileCharging = true;
		CurrentChargeTime = 0.f;

		CurrentVisualStage = 1; // начинаем с первой стадии

		// Получаем меш первой стадии из дефолтного объекта снаряда
		if (BoneProjectileClass)
		{
			ABoneProjectile* DefaultProj = BoneProjectileClass.GetDefaultObject();
			if (DefaultProj && DefaultProj->MeshStage1)
			{
				ChargingMesh->SetStaticMesh(DefaultProj->MeshStage1);
			}
		}

		// Показываем меш над головой
		ChargingMesh->SetVisibility(true);
	}
}

void AMainCharacter::ChargingBoneProjectileReleased()
{
	if (CurrentChargeTime < StageTime) {
		bIsButtonReleasedEarly = true;
	}
	else
	{
		bIsBoneProjectileCharging = false;
		ChargingMesh->SetVisibility(false);
		SpawnChargedBoneProjectile();
	}
}

void AMainCharacter::SpawnChargedBoneProjectile()
{
	if (!BoneProjectileClass) return;

	// Определяем стадию: 0–2 сек - Stage=1, 2–3 сек - Stage=2, 3+ сек - Stage=3	
	int32 Stage;
	if (CurrentChargeTime < StageTime) Stage = 1;
	else Stage = FMath::Clamp(int32(CurrentChargeTime / StageTime), 1, 3);

	// Позиция над головой: можно взять Socket в скелете или просто смещение
	FVector SpawnLoc = ChargingOrigin->GetComponentLocation();
	FRotator SpawnRot = GetControlRotation();

	FActorSpawnParameters Params;
	Params.Instigator = this;
	Params.Owner = this;

	ABoneProjectile* Proj = GetWorld()->SpawnActor<ABoneProjectile>(
		BoneProjectileClass, SpawnLoc, SpawnRot, Params);

	if (Proj)
	{
		Proj->InitCharge(Stage, SpawnRot.Vector());
	}
}
