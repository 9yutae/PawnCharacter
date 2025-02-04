#include "NBC_Pawn.h"
#include "NBC_PlayerController.h"
#include "NBC_AnimInstance.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ANBC_Pawn::ANBC_Pawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Capsule ���� �ʱ�ȭ
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootCapsule"));
	Capsule->SetCapsuleHalfHeight(92.0f);
	Capsule->SetCapsuleRadius(23.0f);

	// ��Ʈ ������Ʈ ����
	SetRootComponent(Capsule);

	// ���̷�Ż �޽� ������Ʈ
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComp->SetupAttachment(Capsule);

	// ���̷�Ż �޽� ������Ʈ �ּ� ����
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Resources/Characters/Meshes/SKM_Manny.SKM_Manny"));
	if (MeshAsset.Succeeded())
	{
		SkeletalMeshComp->SetSkeletalMesh(MeshAsset.Object);
	}

	// ���̷�Ż �޽� ������Ʈ Ʈ������ ��ȯ
	SkeletalMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	SkeletalMeshComp->SetRelativeRotation(FRotator(0.0f, -90.f, 0.0f));

	bUseControllerRotationYaw = true;

	// ������Ʈ ������ �Ҵ�
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// Root�� SpringArm ����
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f; // ���������� ����
	SpringArmComp->bUsePawnControlRotation = false;

	// ī�޶� ��ġ ����
	SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	// ��� ���� ī�޶� ��ġ ����
	SpringArmComp->SocketOffset = FVector(0.0f, 0.0f, 35.0f);

	// ������Ʈ ������ �Ҵ�
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// Camera�� SpringArm�� ����
	// �� ��° ���ڷ� �ش� ������Ʈ�� Ư�� ���� ���� ����
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // SocketName = SpringArm�� �� �κ�
	CameraComp->bUsePawnControlRotation = false; // �������ϸ� ȸ��, ī�޶�� ����

	// ��� ���� Initialize
	NormalSpeed = 600.0f;
	RotationSensitivity = 1.2f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	PreviousLocation = FVector(0.0f, 0.0f, -90.0f);
}

void ANBC_Pawn::BeginPlay()
{
	Super::BeginPlay();
	
	// ��Ʈ ������Ʈ ���� �ùķ��̼� ��Ȱ��ȭ
	UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(GetRootComponent());
	if (RootComp)
	{
		RootComp->SetSimulatePhysics(false);
	}

	// ���̷�Ż �޽� ������Ʈ ���� �ùķ��̼� ��Ȱ��ȭ
	USkeletalMeshComponent* SkeletalMesh = FindComponentByClass<USkeletalMeshComponent>();
	if (SkeletalMesh)
	{
		SkeletalMesh->SetSimulatePhysics(false);
	}

}

void ANBC_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// EnhancedInputComponent ĳ����
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// ���� ĳ���͸� �����ϴ� GetController()�� ������
		// ���� ������ PlayerController Ŭ������ ĳ����
		if (ANBC_PlayerController* PlayerController = Cast<ANBC_PlayerController>(GetController()))
		{
			// PlayerControlloer�� MoveAction ��������
			if (PlayerController->MoveAction)
			{
				// Input Action ���ε� (�̺�Ʈ�� �Լ� �����ϴ� �ٽ� �ڵ�)
				EnhancedInput->BindAction(
					PlayerController->MoveAction,	// IA ��������
					ETriggerEvent::Triggered,		// Ʈ���� �̺�Ʈ
					this,
					&ANBC_Pawn::MoveForward		// ȣ��Ǵ� �Լ�
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,	// IA ��������
					ETriggerEvent::Triggered,		// Ʈ���� �̺�Ʈ
					this,
					&ANBC_Pawn::MoveRight		// ȣ��Ǵ� �Լ�
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ANBC_Pawn::Look
				);
			}

#ifdef JUMPING
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ANBC_Pawn::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,		// Ű �Է��� ������ ��
					this,
					&ANBC_Pawn::StopJump
				);
			}
#endif

#ifdef SPRINT
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ANBC_Pawn::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ANBC_Pawn::StopSprint
				);
			}
#endif
		}
	}
}

void ANBC_Pawn::MoveForward(const FInputActionValue& value)
{
	if (!Controller) return;

	// �Է� �� ��������
	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		// ī�޶��� Forward ���� ���
		FVector ForwardVector = GetActorForwardVector();
		// ��� �̵��� ���� Z�� ����
		ForwardVector.Z = 0;
		// ���� ����ȭ
		ForwardVector.Normalize();

		// ���� ���� ���͸� ���� ��ǥ��� ��ȯ
		FVector LocalForward = GetActorTransform().InverseTransformVector(ForwardVector);

		// �̵� ���� ����
		FVector Direction = (LocalForward * MoveInput.X).GetSafeNormal();

		// X�� �̵�
		AddActorLocalOffset(Direction * NormalSpeed * GetWorld()->GetDeltaSeconds(), true);
		// AddActorWorldOffset(Direction * NormalSpeed * GetWorld()->GetDeltaSeconds(), true);
	}
}

void ANBC_Pawn::MoveRight(const FInputActionValue& value)
{
	if (!Controller) return;

	// �Է� �� ��������
	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		// ī�޶��� Right ���� ���
		FVector RightVector = GetActorRightVector();
		// ��� �̵��� ���� Z�� ����
		RightVector.Z = 0;
		// ���� ����ȭ
		RightVector.Normalize();

		// ���� ���� ���͸� ���� ��ǥ��� ��ȯ
		FVector LocalRight = GetActorTransform().InverseTransformVector(RightVector);

		// �̵� ���� ����
		FVector Direction = (LocalRight * MoveInput.Y).GetSafeNormal();

		// Y�� �̵�
		AddActorLocalOffset(Direction * NormalSpeed * GetWorld()->GetDeltaSeconds(), true);
		// AddActorWorldOffset(Direction * NormalSpeed * GetWorld()->GetDeltaSeconds(), true);
	}
}

void ANBC_Pawn::Look(const FInputActionValue& value)
{
	// ��ȿ�� �˻�
	if (!SpringArmComp && !Controller) return;

	// ���콺 �̵��� ���� X, Y ȸ�� �Է� ��
	FVector2D LookInput = value.Get<FVector2D>();

	// Yaw ȸ�� (�¿� ȸ��) - ī�޶� ���� ����
	FRotator NewControlRotation = Controller->GetControlRotation();
	NewControlRotation.Yaw += LookInput.X * RotationSensitivity;

	// ControlRotation ����
	Controller->SetControlRotation(NewControlRotation);

	// Pitch ȸ�� (���� ȸ��) - SpringArm�� ����
	FRotator SpringArmRotation = SpringArmComp->GetRelativeRotation();
	SpringArmRotation.Pitch = FMath::Clamp(SpringArmRotation.Pitch - LookInput.Y * RotationSensitivity, -85.0f, 85.0f);

	// SpringArm ȸ�� ����
	SpringArmComp->SetRelativeRotation(SpringArmRotation);
}

#ifdef JUMPING
void ANBC_Pawn::StartJump(const FInputActionValue& value)
{
	// �Է� �� ��������
	if (value.Get<bool>())
	{
		Jump();
	}
}

void ANBC_Pawn::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}
#endif

#ifdef SPRINT
void ANBC_Pawn::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ANBC_Pawn::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}
#endif

void ANBC_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMovementSpeed();
}

FVector ANBC_Pawn::GetVelocity()	
{
	// ���� ��ġ�� ���� ��ġ�� ������� �ӵ� ���
	FVector CurrentLocation = GetActorLocation();
	FVector DeltaLocation = CurrentLocation - PreviousLocation;  // ���� ��ġ ����
	PreviousLocation = CurrentLocation;  // ���� ��ġ ������Ʈ

	// �ӵ��� �����Ӵ� �̵� �Ÿ��� ���
	return DeltaLocation / GetWorld()->GetDeltaSeconds();
}

void ANBC_Pawn::UpdateMovementSpeed()
{
	// ���� �ӵ� ��������
	FVector Velocity = GetVelocity();
	float CurrentSpeed = Velocity.Size();

	// �ִϸ��̼� �ν��Ͻ� ���� ��������
	UAnimInstance* AnimInstance = SkeletalMeshComp->GetAnimInstance();

	// �ִϸ��̼� �ν��Ͻ��� �ùٸ� Ÿ������ ĳ���� (ABP_NBC�� �°�)
	if (AnimInstance)
	{
		UNBC_AnimInstance* NBC_AnimInstance = Cast<UNBC_AnimInstance>(AnimInstance);
		if (NBC_AnimInstance)
		{
			// �̵� �ӵ��� ���¸� �ִϸ��̼� �������Ʈ�� ����
			NBC_AnimInstance->Velocity = CurrentSpeed;
			NBC_AnimInstance->IsMoving = (!FMath::IsNearlyZero(CurrentSpeed)); // �̵� ���̸� true, �ƴϸ� false
		}
	}
}
