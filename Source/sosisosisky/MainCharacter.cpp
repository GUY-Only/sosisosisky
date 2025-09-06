// We Sir


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InteractionInterface.h"
#include "Engine/World.h"
#include "UObject/Interface.h"
#include "Components/TextBlock.h"
#include "InteractionComponent.h"
#include "BoneProjectileComponent.h"
#include "LifeDrainComponent.h"
#include "HealthComponent.h"
#include "Components/ProgressBar.h"
#include "PlayerDeathHUDWidget.h"
#include "Kismet/GameplayStatics.h"


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

	

	// HUD

	/*static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(
		TEXT("/Game/UI/WBP_PlayerHUD")
	);
	APlayerController* PlayerController = GetController<APlayerController>();
	if (WidgetBPClass.Class && PlayerController) PlayerHUDWidget = CreateWidget<UUserWidget>(PlayerController, WidgetBPClass.Class);
	if (PlayerHUDWidget) PlayerHUDWidget->AddToViewport();
	if (PlayerHUDWidget) UE_LOG(LogTemp, Log, TEXT("SSSSSSSS"));*/

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/UI/WBP_PlayerHUD"));

	if (WidgetBPClass.Succeeded())
	{
		PlayerHUDClass = WidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UPlayerDeathHUDWidget> DeathWidgetBPClass(TEXT("/Game/UI/WBP_DeathHUD"));

	if (WidgetBPClass.Succeeded())
	{
		PlayerDeathHUDClass = DeathWidgetBPClass.Class;
	}

	Resources.Add(EResourceType::Bone, 0);
	Resources.Add(EResourceType::Soul, 0);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	BoneProjectileComponent = CreateDefaultSubobject<UBoneProjectileComponent>(TEXT("BoneProjectileComponent"));
	LifeDrainComponent = CreateDefaultSubobject<ULifeDrainComponent>(TEXT("LifeDrainComponent"));

	Ability1_ComponentPtr = nullptr;
	Ability2_ComponentPtr = nullptr;
	Ability3_ComponentPtr = nullptr;

	Attack1 = EAttacks::None;
	Attack2 = EAttacks::LifeDrain;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void AMainCharacter::UpdateHUD()
{
	if (!PlayerHUDWidget) return;

	if (UTextBlock* TB = Cast<UTextBlock>(PlayerHUDWidget->GetWidgetFromName(TEXT("Txt_BoneCounter"))))
	{
		TB->SetText(FText::AsNumber(GetResourceCount(EResourceType::Bone)));
	}
	if (UTextBlock* TB = Cast<UTextBlock>(PlayerHUDWidget->GetWidgetFromName(TEXT("Txt_SoulCounter"))))
	{
		TB->SetText(FText::AsNumber(GetResourceCount(EResourceType::Soul)));
	}
}

void AMainCharacter::Interact()
{
	if (!HealthComponent->IsDead()) {
		InteractionComponent->Interact();
	}
}

void AMainCharacter::AddResources(EResourceType ResourceType, int32 amount)
{
	int32* CurrentAmount = Resources.Find(ResourceType);
	if (CurrentAmount)
	{
		// Если ресурс уже есть в карте, просто увеличиваем его количество.
		*CurrentAmount += amount;
	}
	else
	{
		// Если ресурса еще нет, добавляем его с указанным количеством.
		// Эта ветка сработает, если вы не инициализировали все ресурсы в конструкторе.
		Resources.Add(ResourceType, amount);
	}

	UpdateHUD();
}

bool AMainCharacter::RemoveResources(EResourceType ResourceType, int32 amount)
{
	if (GetResourceCount(ResourceType) >= amount) 
	{
		int32* CurrentAmount = Resources.Find(ResourceType);
		if (CurrentAmount)
		{
			// Если ресурс уже есть в карте, просто увеличиваем его количество.
			*CurrentAmount -= amount;
		}
		UpdateHUD();
		return true;
	}
	else 
	{
		return false;
	}
}

int32 AMainCharacter::GetResourceCount(EResourceType ResourceType) const
{
	return Resources.FindRef(ResourceType);
}



// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerHUDClass)
	{
		APlayerController* PlayerController = GetController<APlayerController>();
		if (PlayerController)
		{
			PlayerHUDWidget = CreateWidget<UUserWidget>(PlayerController, PlayerHUDClass);

			if (PlayerHUDWidget)
			{
				PlayerHUDWidget->AddToViewport();
			}
		}

		
	}

	if (PlayerDeathHUDClass)
	{
		APlayerController* PlayerController = GetController<APlayerController>();
		if (PlayerController)
		{
			PlayerDeathHUDWidget = CreateWidget<UPlayerDeathHUDWidget>(PlayerController, PlayerDeathHUDClass);
		}


	}

	SetAbilityForSlot(1, EAbilities::BoneProjectile);
	SetAbilityForSlot(2, EAbilities::None);
	SetAbilityForSlot(3, EAbilities::None);

	if (HealthComponent) {
		HealthComponent->OnHealthEnded.AddDynamic(this, &AMainCharacter::OnDeath);
		HealthComponent->OnHealthChanged.AddDynamic(this, &AMainCharacter::HandleHealthChanged);
	}

	SpawnLocation = GetActorLocation();
	SpawnRotation = GetActorRotation();
	SpawnMeshRotation = GetMesh()->GetRelativeRotation();

	if(CameraBoom)
		DefaultSpringArmRelativeTransform = CameraBoom->GetRelativeTransform();

}

void AMainCharacter::OnDeath(AActor* DamageCauser)
{
	// Отключаем управление
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->SetIgnoreMoveInput(true);	//	Клавиши ввода
		//PC->SetIgnoreLookInput(true);		Камера
		PC->DisableInput(PC);
	}

	// Форсим отпустить все зажатые кнопки
	Ability1Released();
	Ability2Released();
	Ability3Released();
	Attack1Released();
	Attack2Released();

	// Сохраняем старого родителя спрингарма
	if (CameraBoom && !DefaultSpringArmParent)
	{
		DefaultSpringArmParent = CameraBoom->GetAttachParent();
	}

	// Перепривязываем спрингарм к мешу (например, к socket "pelvis")
	if (CameraBoom && GetMesh())
	{
		CameraBoom->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("pelvis"));
	}

	if (CameraBoom)
	{
		CameraBoom->bEnableCameraLag = true;
		CameraBoom->CameraLagSpeed = CamLagSpeed;
	}

	// Отключаем коллизию капсулы, чтобы не мешала регдоллу
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Включаем физику на меше (регдолл)
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetAllBodiesSimulatePhysics(true);
	MeshComp->WakeAllRigidBodies();
	MeshComp->bBlendPhysics = true;

	// Запускаем таймер респауна
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AMainCharacter::Respawn, RespawnDelay, false);

	// Виджет смерти
	if (PlayerDeathHUDWidget)
	{
		PlayerDeathHUDWidget->AddToViewport();
		PlayerDeathHUDWidget->PlayAnimation(PlayerDeathHUDWidget->FadeInAnimation);
		GetWorldTimerManager().SetTimer(RespawnHUDTimerHandle, this, &AMainCharacter::RespawnHUDAnim, RespawnDelay-1, false);
	}
}

void AMainCharacter::Respawn()
{
	// Отправляем на спавн
	SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	GetMesh()->SetRelativeRotation(SpawnMeshRotation);

	// Выключаем физику
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetAllBodiesSimulatePhysics(false);
	MeshComp->SetCollisionProfileName(TEXT("CharacterMesh"));

	// Включаем капсулу
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Сбрасываем здоровье в компоненте
	if (HealthComponent)
	{
		HealthComponent->ResetHealth();
	}

	// Возвращаем управление
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->SetIgnoreMoveInput(false);
		//PC->SetIgnoreLookInput(false);
		PC->EnableInput(PC);
	}

	// Возвращаем спрингарм на исходного родителя
	if (CameraBoom && DefaultSpringArmParent)
	{
		CameraBoom->AttachToComponent(DefaultSpringArmParent, FAttachmentTransformRules::SnapToTargetIncludingScale);
		CameraBoom->SetRelativeTransform(DefaultSpringArmRelativeTransform);
	}

	if (CameraBoom)
	{
		CameraBoom->bEnableCameraLag = false;
	}

	// Виджет смерти
	if (PlayerDeathHUDWidget)
	{
		PlayerDeathHUDWidget->RemoveFromViewport();
	}
}

void AMainCharacter::RespawnHUDAnim()
{
	if (PlayerDeathHUDWidget && PlayerDeathHUDWidget->FadeOutAnimation)
	{
		PlayerDeathHUDWidget->PlayAnimation(PlayerDeathHUDWidget->FadeOutAnimation);
	}
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (HealthComponent)
		HealthComponent->TakeDamage(DamageApplied, DamageCauser);
	return DamageApplied;

}

void AMainCharacter::HandleHealthChanged(float NewPercent, float DelayedPercent)
{
	if (!PlayerHUDWidget) return;
	
		// 1. Получаем вложенный виджет (сам HealthBarHUD)
		if (UUserWidget* HealthBarHUD = Cast<UUserWidget>(PlayerHUDWidget->GetWidgetFromName(TEXT("WBP_HealthBarHUD"))))
		{
			// 2. Уже внутри него ищем прогресс-бары
			if (UProgressBar* Bar = Cast<UProgressBar>(HealthBarHUD->GetWidgetFromName(TEXT("HealthBar"))))
			{
				Bar->SetPercent(NewPercent);
			}
			if (UProgressBar* DelayBar = Cast<UProgressBar>(HealthBarHUD->GetWidgetFromName(TEXT("DelayedBar"))))
			{
				DelayBar->SetPercent(DelayedPercent);
			}
		}
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &AMainCharacter::Ability1Pressed);
	PlayerInputComponent->BindAction("Ability1", IE_Released, this, &AMainCharacter::Ability1Released);
	PlayerInputComponent->BindAction("Ability2", IE_Pressed, this, &AMainCharacter::Ability2Pressed);
	PlayerInputComponent->BindAction("Ability2", IE_Released, this, &AMainCharacter::Ability2Released);
	PlayerInputComponent->BindAction("Ability3", IE_Pressed, this, &AMainCharacter::Ability3Pressed);
	PlayerInputComponent->BindAction("Ability3", IE_Released, this, &AMainCharacter::Ability3Released);

	//PlayerInputComponent->BindAction("Interact", IE_Pressed, InteractionComponent, &UInteractionComponent::Interact);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::Interact);

	PlayerInputComponent->BindAction("Attack1", IE_Pressed, this, &AMainCharacter::Attack1Pressed);
	PlayerInputComponent->BindAction("Attack1", IE_Released, this, &AMainCharacter::Attack1Released);
	PlayerInputComponent->BindAction("Attack2", IE_Pressed, this, &AMainCharacter::Attack2Pressed);
	PlayerInputComponent->BindAction("Attack2", IE_Released, this, &AMainCharacter::Attack2Released);
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
	HealthComponent->TakeDamage(10.0f, this);
}

void AMainCharacter::StopJumping()
{
	if (Controller != NULL) ACharacter::StopJumping();
}


// Абилки

void AMainCharacter::Ability1Pressed()
{
	if (Ability1_ComponentPtr && !HealthComponent->IsDead())
	{
		Ability1_ComponentPtr->PressAbility();
	}
}

void AMainCharacter::Ability1Released()
{
	if (Ability1_ComponentPtr)
	{
		Ability1_ComponentPtr->ReleaseAbility();
	}
}

void AMainCharacter::Ability2Pressed()
{
	if (Ability2_ComponentPtr && !HealthComponent->IsDead())
	{
		Ability2_ComponentPtr->PressAbility();
	}
}

void AMainCharacter::Ability2Released()
{
	if (Ability2_ComponentPtr)
	{
		Ability2_ComponentPtr->ReleaseAbility();
	}
}

void AMainCharacter::Ability3Pressed()
{
	if (Ability3_ComponentPtr && !HealthComponent->IsDead())
	{
		Ability3_ComponentPtr->PressAbility();
	}
}

void AMainCharacter::Ability3Released()
{
	if (Ability3_ComponentPtr)
	{
		Ability3_ComponentPtr->ReleaseAbility();
	}
}

void AMainCharacter::SetAbilityForSlot(int32 SlotIndex, EAbilities NewAbility)
{
	UBaseAbilityComponent** TargetSlot = nullptr;

	switch (SlotIndex)
	{
	case 1: TargetSlot = &Ability1_ComponentPtr; break;
	case 2: TargetSlot = &Ability2_ComponentPtr; break;
	case 3: TargetSlot = &Ability3_ComponentPtr; break;
	default: return; 
	}

	if (!TargetSlot) return;

	switch (NewAbility)
	{
	case EAbilities::None:
		*TargetSlot = nullptr;
		break;
	case EAbilities::BoneProjectile:
		*TargetSlot = BoneProjectileComponent;
		break;
	default: return; 
		break;
	}
}



// Атаки

void AMainCharacter::Attack1Pressed()
{
	if (!HealthComponent->IsDead()) {
		switch (Attack1)
		{
		case EAttacks::None: return;
			break;
		case EAttacks::LifeDrain: LifeDrainComponent->PressAttack();
			break;
		default:
			break;
		}
	}
}

void AMainCharacter::Attack1Released()
{
	
		switch (Attack1)
		{
		case EAttacks::None: return;
			break;
		case EAttacks::LifeDrain: LifeDrainComponent->ReleaseAttack();
			break;
		default:
			break;
		}
	
}

void AMainCharacter::Attack2Pressed() 
{
	if (!HealthComponent->IsDead()) {
		switch (Attack2)
		{
		case EAttacks::None: return;
			break;
		case EAttacks::LifeDrain: LifeDrainComponent->PressAttack();
			break;
		default:
			break;
		}
	}
}

void AMainCharacter::Attack2Released() 
{
	
		switch (Attack2)
		{
		case EAttacks::None: return;
			break;
		case EAttacks::LifeDrain: LifeDrainComponent->ReleaseAttack();
			break;
		default:
			break;
		}
	
}
