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

	// Capsule 변수 초기화
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootCapsule"));
	Capsule->SetCapsuleHalfHeight(92.0f);
	Capsule->SetCapsuleRadius(23.0f);

	// 루트 컴포넌트 설정
	SetRootComponent(Capsule);

	// 스켈레탈 메시 컴포넌트
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComp->SetupAttachment(Capsule);

	// 스켈레탈 메시 컴포넌트 애셋 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Resources/Characters/Meshes/SKM_Manny.SKM_Manny"));
	if (MeshAsset.Succeeded())
	{
		SkeletalMeshComp->SetSkeletalMesh(MeshAsset.Object);
	}

	// 스켈레탈 메시 컴포넌트 트랜스폼 변환
	SkeletalMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	SkeletalMeshComp->SetRelativeRotation(FRotator(0.0f, -90.f, 0.0f));

	bUseControllerRotationYaw = true;

	// 컴포넌트 포인터 할당
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// Root에 SpringArm 부착
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f; // 스프링암의 길이
	SpringArmComp->bUsePawnControlRotation = false;

	// 카메라 위치 조정
	SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	// 어깨 위로 카메라 위치 조정
	SpringArmComp->SocketOffset = FVector(0.0f, 0.0f, 35.0f);

	// 컴포넌트 포인터 할당
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// Camera는 SpringArm에 부착
	// 두 번째 인자로 해당 컴포넌트의 특정 소켓 지정 가능
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // SocketName = SpringArm의 끝 부분
	CameraComp->bUsePawnControlRotation = false; // 스프링암만 회전, 카메라는 고정

	// 멤버 변수 Initialize
	NormalSpeed = 600.0f;
	RotationSensitivity = 1.2f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	PreviousLocation = FVector(0.0f, 0.0f, -90.0f);
}

void ANBC_Pawn::BeginPlay()
{
	Super::BeginPlay();
	
	// 루트 컴포넌트 물리 시뮬레이션 비활성화
	UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(GetRootComponent());
	if (RootComp)
	{
		RootComp->SetSimulatePhysics(false);
	}

	// 스켈레탈 메쉬 컴포넌트 물리 시뮬레이션 비활성화
	USkeletalMeshComponent* SkeletalMesh = FindComponentByClass<USkeletalMeshComponent>();
	if (SkeletalMesh)
	{
		SkeletalMesh->SetSimulatePhysics(false);
	}

}

void ANBC_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// EnhancedInputComponent 캐스팅
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 현재 캐릭터를 조작하는 GetController()를 가져와
		// 직접 구현한 PlayerController 클래스로 캐스팅
		if (ANBC_PlayerController* PlayerController = Cast<ANBC_PlayerController>(GetController()))
		{
			// PlayerControlloer의 MoveAction 가져오기
			if (PlayerController->MoveAction)
			{
				// Input Action 바인딩 (이벤트와 함수 연결하는 핵심 코드)
				EnhancedInput->BindAction(
					PlayerController->MoveAction,	// IA 가져오기
					ETriggerEvent::Triggered,		// 트리거 이벤트
					this,
					&ANBC_Pawn::MoveForward		// 호출되는 함수
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,	// IA 가져오기
					ETriggerEvent::Triggered,		// 트리거 이벤트
					this,
					&ANBC_Pawn::MoveRight		// 호출되는 함수
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
					ETriggerEvent::Completed,		// 키 입력이 끝났을 때
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

	// 입력 값 가져오기
	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		// 카메라의 Forward 벡터 사용
		FVector ForwardVector = GetActorForwardVector();
		// 평면 이동을 위한 Z축 제거
		ForwardVector.Z = 0;
		// 벡터 정규화
		ForwardVector.Normalize();

		// 월드 기준 벡터를 로컬 좌표계로 변환
		FVector LocalForward = GetActorTransform().InverseTransformVector(ForwardVector);

		// 이동 방향 설정
		FVector Direction = (LocalForward * MoveInput.X).GetSafeNormal();

		// X축 이동
		AddActorLocalOffset(Direction * NormalSpeed * GetWorld()->GetDeltaSeconds(), true);
		// AddActorWorldOffset(Direction * NormalSpeed * GetWorld()->GetDeltaSeconds(), true);
	}
}

void ANBC_Pawn::MoveRight(const FInputActionValue& value)
{
	if (!Controller) return;

	// 입력 값 가져오기
	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		// 카메라의 Right 벡터 사용
		FVector RightVector = GetActorRightVector();
		// 평면 이동을 위한 Z축 제거
		RightVector.Z = 0;
		// 벡터 정규화
		RightVector.Normalize();

		// 월드 기준 벡터를 로컬 좌표계로 변환
		FVector LocalRight = GetActorTransform().InverseTransformVector(RightVector);

		// 이동 방향 설정
		FVector Direction = (LocalRight * MoveInput.Y).GetSafeNormal();

		// Y축 이동
		AddActorLocalOffset(Direction * NormalSpeed * GetWorld()->GetDeltaSeconds(), true);
		// AddActorWorldOffset(Direction * NormalSpeed * GetWorld()->GetDeltaSeconds(), true);
	}
}

void ANBC_Pawn::Look(const FInputActionValue& value)
{
	// 유효성 검사
	if (!SpringArmComp && !Controller) return;

	// 마우스 이동에 의한 X, Y 회전 입력 값
	FVector2D LookInput = value.Get<FVector2D>();

	// Yaw 회전 (좌우 회전) - 카메라 방향 기준
	FRotator NewControlRotation = Controller->GetControlRotation();
	NewControlRotation.Yaw += LookInput.X * RotationSensitivity;

	// ControlRotation 적용
	Controller->SetControlRotation(NewControlRotation);

	// Pitch 회전 (상하 회전) - SpringArm에 적용
	FRotator SpringArmRotation = SpringArmComp->GetRelativeRotation();
	SpringArmRotation.Pitch = FMath::Clamp(SpringArmRotation.Pitch - LookInput.Y * RotationSensitivity, -85.0f, 85.0f);

	// SpringArm 회전 적용
	SpringArmComp->SetRelativeRotation(SpringArmRotation);
}

#ifdef JUMPING
void ANBC_Pawn::StartJump(const FInputActionValue& value)
{
	// 입력 값 가져오기
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
	// 현재 위치와 이전 위치를 기반으로 속도 계산
	FVector CurrentLocation = GetActorLocation();
	FVector DeltaLocation = CurrentLocation - PreviousLocation;  // 이전 위치 저장
	PreviousLocation = CurrentLocation;  // 이전 위치 업데이트

	// 속도를 프레임당 이동 거리로 계산
	return DeltaLocation / GetWorld()->GetDeltaSeconds();
}

void ANBC_Pawn::UpdateMovementSpeed()
{
	// 현재 속도 가져오기
	FVector Velocity = GetVelocity();
	float CurrentSpeed = Velocity.Size();

	// 애니메이션 인스턴스 참조 가져오기
	UAnimInstance* AnimInstance = SkeletalMeshComp->GetAnimInstance();

	// 애니메이션 인스턴스를 올바른 타입으로 캐스팅 (ABP_NBC에 맞게)
	if (AnimInstance)
	{
		UNBC_AnimInstance* NBC_AnimInstance = Cast<UNBC_AnimInstance>(AnimInstance);
		if (NBC_AnimInstance)
		{
			// 이동 속도와 상태를 애니메이션 블루프린트로 전달
			NBC_AnimInstance->Velocity = CurrentSpeed;
			NBC_AnimInstance->IsMoving = (!FMath::IsNearlyZero(CurrentSpeed)); // 이동 중이면 true, 아니면 false
		}
	}
}
