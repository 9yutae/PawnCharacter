#include "NBC_UAV.h"
#include "NBC_UAVController.h"
#include "EnhancedInputComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ANBC_UAV::ANBC_UAV()
{
	PrimaryActorTick.bCanEverTick = true;

	// Box ���� �ʱ�ȭ
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("RootCapsule"));
	Box->SetBoxExtent(FVector(100.f, 100.f, 25.f));

	// ��Ʈ ������Ʈ ����
	SetRootComponent(Box);

	// ����ƽ �޽� ������Ʈ
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(Box);

	// ����ƽ �޽� ������Ʈ �ּ� ����
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Fab/White_Drone/white_drone.white_drone"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshAsset.Object);
	}

	// �Ӹ�Ƽ�� ����
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("/Game/Fab/White_Drone/Scene_-_Root.Scene_-_Root"));
	if (MaterialAsset.Succeeded())
	{
		StaticMeshComp->SetMaterial(0, MaterialAsset.Object);
	}

	// ������Ʈ ������ �Ҵ�
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// Root�� SpringArm ����
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 350.0f; // ���������� ����
	SpringArmComp->bUsePawnControlRotation = false;

	// ī�޶� ��ġ ����
	SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	SpringArmComp->SocketOffset = FVector(0.0f, 0.0f, 50.0f);

	// ��Ʈ�ѷ��� Yaw ���� Pawn�� �ݿ�
	bUseControllerRotationYaw = true;

	// ������Ʈ ������ �Ҵ�
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetWorldRotation(FRotator(0.f, -10.f, 0.f));
	// Camera�� SpringArm�� ����
	// �� ��° ���ڷ� �ش� ������Ʈ�� Ư�� ���� ���� ����
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // SocketName = SpringArm�� �� �κ�
	CameraComp->bUsePawnControlRotation = false; // �������ϸ� ȸ��, ī�޶�� ����

	// �⺻ �ӵ� �ʱ�ȭ
	CurrentVelocity = FVector::ZeroVector;
	CurrentRotationSpeed = FRotator::ZeroRotator;
	MaxSpeed = 1000.f;
	RotationSensitivity = 1.2f;

}

void ANBC_UAV::BeginPlay()
{
	Super::BeginPlay();

}

void ANBC_UAV::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��ġ �̵� ����
	if (!CurrentVelocity.IsZero())
	{
		FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
		SetActorLocation(NewLocation);
	}

	// ȸ�� ����
	if (!CurrentRotationSpeed.IsZero())
	{
		FRotator NewRotation = GetActorRotation() + (CurrentRotationSpeed * DeltaTime);
		SetActorRotation(NewRotation);
	}
}

void ANBC_UAV::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// EnhancedInputComponent ĳ����
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// ���� ĳ���͸� �����ϴ� GetController()�� ������
		// ���� ������ PlayerController Ŭ������ ĳ����
		if (ANBC_UAVController* PlayerController = Cast<ANBC_UAVController>(GetController()))
		{
			// PlayerControlloer�� MoveAction ��������
			if (PlayerController->MoveAction)
			{
				// Input Action ���ε� (�̺�Ʈ�� �Լ� �����ϴ� �ٽ� �ڵ�)
				EnhancedInput->BindAction(
					PlayerController->MoveAction,	// IA ��������
					ETriggerEvent::Triggered,		// Ʈ���� �̺�Ʈ
					this,
					&ANBC_UAV::MoveForward		// ȣ��Ǵ� �Լ�
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,	// IA ��������
					ETriggerEvent::Triggered,		// Ʈ���� �̺�Ʈ
					this,
					&ANBC_UAV::MoveRight		// ȣ��Ǵ� �Լ�
				);
			}
			
			if (PlayerController->UpDownAction)
			{
				EnhancedInput->BindAction(
					PlayerController->UpDownAction,	// IA ��������
					ETriggerEvent::Triggered,		// Ʈ���� �̺�Ʈ
					this,
					&ANBC_UAV::MoveUp		// ȣ��Ǵ� �Լ�
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ANBC_UAV::Turn
				);
			}
			
			if (PlayerController->TiltAction)
			{
				EnhancedInput->BindAction(
					PlayerController->TiltAction,
					ETriggerEvent::Triggered,
					this,
					&ANBC_UAV::Tilt
				);
			}
		}
	}
}

void ANBC_UAV::MoveForward(const FInputActionValue& Value)
{
	if (!Controller) return;

	// �Է� �� ��������
	const FVector2D MoveInput = Value.Get<FVector2D>();

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
		AddActorLocalOffset(Direction * MaxSpeed * GetWorld()->GetDeltaSeconds(), true);
	}
}

void ANBC_UAV::MoveRight(const FInputActionValue& Value)
{
	if (!Controller) return;

	// �Է� �� ��������
	const FVector2D MoveInput = Value.Get<FVector2D>();

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
		AddActorLocalOffset(Direction * MaxSpeed * GetWorld()->GetDeltaSeconds(), true);
	}
}

void ANBC_UAV::MoveUp(const FInputActionValue& Value)
{
	// �Է� �� ��������
	const float MoveInput = Value.Get<float>();

	if (!FMath::IsNearlyZero(MoveInput))
	{
		// ī�޶��� Up ���� ���
		FVector NormalVector = GetActorUpVector();

		// ���� ����ȭ
		NormalVector.Normalize();

		// ���� ���� ���͸� ���� ��ǥ��� ��ȯ
		FVector LocalForward = GetActorTransform().InverseTransformVector(NormalVector);

		// �̵� ���� ����
		FVector Direction = (LocalForward * MoveInput).GetSafeNormal();

		// Z�� �̵�
		AddActorLocalOffset(Direction * MaxSpeed * GetWorld()->GetDeltaSeconds(), true);
	}
}

void ANBC_UAV::Turn(const FInputActionValue& Value)
{
	// ��ȿ�� �˻�
	if (!SpringArmComp && !Controller) return;

	// ���콺 �̵��� ���� X, Y ȸ�� �Է� ��
	FVector2D LookInput = Value.Get<FVector2D>();

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

void ANBC_UAV::Tilt(const FInputActionValue& Value)
{
	// ��ȿ�� �˻�
	if (!SpringArmComp && !Controller) return;

	// �Է� �� ��������
	const float LookInput = Value.Get<float>();

	// Roll ȸ�� (Tilt ȸ��) - SpringArm�� ����
	FRotator SpringArmRotation = SpringArmComp->GetRelativeRotation();
	SpringArmRotation.Roll = FMath::Clamp(SpringArmRotation.Roll + LookInput * RotationSensitivity, -45.0f, 45.0f);

	// SpringArm ȸ�� ����
	SpringArmComp->SetRelativeRotation(SpringArmRotation);

	// �ε巯�� ƿƮ ȿ�� ���� (����)
	SpringArmRotation.Roll = FMath::FInterpTo(SpringArmComp->GetRelativeRotation().Roll, SpringArmRotation.Roll, GetWorld()->GetDeltaSeconds(), 5.0f);
}
