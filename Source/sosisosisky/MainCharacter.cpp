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

	Resources.Add(EResourceType::Bone, 0);
	Resources.Add(EResourceType::Soul, 0);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	BoneProjectileComponent = CreateDefaultSubobject<UBoneProjectileComponent>(TEXT("BoneProjectileComponent"));

	Ability1 = EAbilities::BoneProjectile;
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

	PlayerInputComponent->BindAction("Interact", IE_Pressed, InteractionComponent, &UInteractionComponent::Interact);

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
}

void AMainCharacter::StopJumping()
{
	if (Controller != NULL) ACharacter::StopJumping();
}


// Абилки

void AMainCharacter::Ability1Pressed()
{
	switch (Ability1)
	{
	case EAbilities::None: return;
		break;
	case EAbilities::BoneProjectile: BoneProjectileComponent->ChargingBoneProjectilePressed();
		break;
	default:
		break;
	}
}

void AMainCharacter::Ability1Released()
{
	switch (Ability1)
	{
	case EAbilities::None: return;
		break;
	case EAbilities::BoneProjectile: BoneProjectileComponent->ChargingBoneProjectileReleased();
		break;
	default:
		break;
	}
}

void AMainCharacter::Ability2Pressed()
{
	switch (Ability2)
	{
	case EAbilities::None: return;
		break;
	case EAbilities::BoneProjectile: BoneProjectileComponent->ChargingBoneProjectilePressed();
		break;
	default:
		break;
	}
}

void AMainCharacter::Ability2Released()
{
	switch (Ability2)
	{
	case EAbilities::None: return;
		break;
	case EAbilities::BoneProjectile: BoneProjectileComponent->ChargingBoneProjectileReleased();
		break;
	default:
		break;
	}
}

void AMainCharacter::Ability3Pressed()
{
	switch (Ability3)
	{
	case EAbilities::None: return;
		break;
	case EAbilities::BoneProjectile: BoneProjectileComponent->ChargingBoneProjectilePressed();
		break;
	default:
		break;
	}
}

void AMainCharacter::Ability3Released()
{
	switch (Ability3)
	{
	case EAbilities::None: return;
		break;
	case EAbilities::BoneProjectile: BoneProjectileComponent->ChargingBoneProjectileReleased();
		break;
	default:
		break;
	}
}



// Атаки

void AMainCharacter::Attack1Pressed()
{
	switch (Attack1)
	{
	case EAttacks::None: return;
		break;
	case EAttacks::Suck: return;
		break;
	default:
		break;
	}
}

void AMainCharacter::Attack1Released()
{
	switch (Attack1)
	{
	case EAttacks::None: return;
		break;
	case EAttacks::Suck: return;
		break;
	default:
		break;
	}
}

void AMainCharacter::Attack2Pressed() 
{
	switch (Attack2)
	{
	case EAttacks::None: return;
		break;
	case EAttacks::Suck: return;
		break;
	default:
		break;
	}
}

void AMainCharacter::Attack2Released() 
{
	switch (Attack2)
	{
	case EAttacks::None: return;
		break;
	case EAttacks::Suck: return;
		break;
	default:
		break;
	}
}
