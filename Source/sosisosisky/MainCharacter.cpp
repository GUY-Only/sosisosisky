// We Sir


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"


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

	PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &AMainCharacter::Ability1);
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

void AMainCharacter::Ability1()
{
	ACharacter::Jump();
}
