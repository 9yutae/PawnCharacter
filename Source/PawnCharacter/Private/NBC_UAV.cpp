#include "NBC_UAV.h"
#include "NBC_UAVController.h"
#include "EnhancedInputComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ANBC_UAV::ANBC_UAV()
{
	PrimaryActorTick.bCanEverTick = true;

	// Box 변수 초기화
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("RootCapsule"));
	Box->SetBoxExtent(FVector(100.f, 100.f, 25.f));

	// 충돌 관련 설정
	Box->SetNotifyRigidBodyCollision(true); // 충돌 이벤트 활성화
	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Box->SetCollisionObjectType(ECC_PhysicsBody);
	Box->SetCollisionResponseToAllChannels(ECR_Block);

	// 루트 컴포넌트 설정
	SetRootComponent(Box);

	// 충돌 이벤트 바인딩
	Box->OnComponentHit.AddDynamic(this, &ANBC_UAV::OnUAVHit);

	// 스태틱 메시 컴포넌트
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(Box);

	// 스태틱 메시 컴포넌트 애셋 설정
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Fab/White_Drone/white_drone.white_drone"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshAsset.Object);
	}

	// 머리티얼 설정
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("/Game/Fab/White_Drone/Scene_-_Root.Scene_-_Root"));
	if (MaterialAsset.Succeeded())
	{
		StaticMeshComp->SetMaterial(0, MaterialAsset.Object);
	}

	// 컴포넌트 포인터 할당
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// Root에 SpringArm 부착
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 350.0f; // 스프링암의 길이
	SpringArmComp->bUsePawnControlRotation = false;

	// 카메라 위치 조정
	SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	SpringArmComp->SocketOffset = FVector(0.0f, 0.0f, 50.0f);

	// 컨트롤러의 Yaw, Pitch, Roll 값을 Pawn에 반영
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	// 컴포넌트 포인터 할당
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetWorldRotation(FRotator(0.f, -10.f, 0.f));
	// Camera는 SpringArm에 부착
	// 두 번째 인자로 해당 컴포넌트의 특정 소켓 지정 가능
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // SocketName = SpringArm의 끝 부분
	CameraComp->bUsePawnControlRotation = false; // 스프링암만 회전, 카메라는 고정

	// 초기 값 설정
	InitialPosition = FVector(0.f, 0.f, 32.f);
	InitialRotation = FRotator::ZeroRotator;
	CurrentVelocity = FVector::ZeroVector;
	MaxSpeed = 1000.f;
	RotationSensitivity = 1.2f;
	bResetTiltRequested = false;
	bIsRightClicking = false;
	bShouldInterpBack = false;
	TerminalSpeed = sqrt(AirResistance * Gravity * GetActorScale().Z);
}

void ANBC_UAV::BeginPlay()
{
	Super::BeginPlay();

	InitialCameraRotation = SpringArmComp->GetRelativeRotation();
}

// 충돌 처리
void ANBC_UAV::OnUAVHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 충돌 시 정지 (비활성화)
	SetActorTickEnabled(false);
	Box->SetPhysicsLinearVelocity(FVector::ZeroVector); // 속도를 0으로 설정
	Box->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector); // 각속도를 0으로 설정

	// 바닥과 접촉 여부 확인
	bool bIsGrounded = false;
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 10.0f);

	FHitResult GroundHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(GroundHit, Start, End, ECC_Visibility, Params))
	{
		bIsGrounded = true;
	}

	// 바닥이 아니라면 초기 위치로 리셋
	if (!bIsGrounded)
	{
		// SetActorLocation(InitialPosition);
		SetActorRotation(InitialRotation);
		SetActorTickEnabled(true); // 다시 활성화
	}
}

void ANBC_UAV::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 이동 중이 아닐 때는 드론의 Roll, Yaw 0으로 되돌리기
	if (CurrentVelocity.IsNearlyZero())
	{
		FRotator NewRotation = StaticMeshComp->GetRelativeRotation();

		// Roll과 Pitch를 0으로 점진적으로 보간
		NewRotation.Yaw = FMath::FInterpTo(NewRotation.Yaw, 0.0f, DeltaTime, 1.5f);
		NewRotation.Pitch = FMath::FInterpTo(NewRotation.Pitch, 0.0f, DeltaTime, 1.5f);
		NewRotation.Roll = FMath::FInterpTo(NewRotation.Roll, 0.0f, DeltaTime, 1.5f);		

		StaticMeshComp->SetRelativeRotation(NewRotation);
	}

	// Tilt 원 위치 (Q, E Completed)
	if (bResetTiltRequested)
	{
		FRotator NewControlRotation = Controller->GetControlRotation();
		NewControlRotation.Roll = FMath::FInterpTo(NewControlRotation.Roll, 0.f, GetWorld()->GetDeltaSeconds(), 3.0f);

		if (FMath::IsNearlyZero(NewControlRotation.Roll))
		{
			NewControlRotation.Roll = 0.f;
			bResetTiltRequested = false;
		}

		// ControlRotation 적용
		Controller->SetControlRotation(NewControlRotation);
	}

	// 마우스 우클릭 해제 시 카메라 원위치
	if (bShouldInterpBack)
	{
		FRotator CurrentRotation = SpringArmComp->GetRelativeRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, InitialCameraRotation, DeltaTime, 5.0f);
		SpringArmComp->SetRelativeRotation(NewRotation);

		// 목표 회전에 도달하면 보간 중지
		if (NewRotation.Equals(InitialCameraRotation, 0.1f))
		{
			bShouldInterpBack = false;
		}
	}
}

void ANBC_UAV::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// EnhancedInputComponent 캐스팅
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 현재 캐릭터를 조작하는 GetController()를 가져와
		// 직접 구현한 PlayerController 클래스로 캐스팅
		if (ANBC_UAVController* PlayerController = Cast<ANBC_UAVController>(GetController()))
		{
			// PlayerControlloer의 MoveAction 가져오기
			if (PlayerController->MoveAction)
			{
				// Input Action 바인딩 (이벤트와 함수 연결하는 핵심 코드)
				EnhancedInput->BindAction(
					PlayerController->MoveAction,	// IA 가져오기
					ETriggerEvent::Triggered,		// 트리거 이벤트
					this,
					&ANBC_UAV::MoveForward		// 호출되는 함수
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,	// IA 가져오기
					ETriggerEvent::Triggered,		// 트리거 이벤트
					this,
					&ANBC_UAV::MoveRight		// 호출되는 함수
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,	// IA 가져오기
					ETriggerEvent::Completed,		// 트리거 이벤트
					this,
					&ANBC_UAV::StopMove		// 호출되는 함수
				);
			}
			
			if (PlayerController->UpDownAction)
			{
				EnhancedInput->BindAction(
					PlayerController->UpDownAction,	// IA 가져오기
					ETriggerEvent::Triggered,		// 트리거 이벤트
					this,
					&ANBC_UAV::MoveUp		// 호출되는 함수
				);

				EnhancedInput->BindAction(
					PlayerController->UpDownAction,	// IA 가져오기
					ETriggerEvent::Completed,		// 트리거 이벤트
					this,
					&ANBC_UAV::StopMove		// 호출되는 함수
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

				EnhancedInput->BindAction(
					PlayerController->TiltAction,
					ETriggerEvent::Completed,
					this,
					&ANBC_UAV::ResetTilt
				);
			}

			if (PlayerController->RightClickAction)
			{
				EnhancedInput->BindAction(
					PlayerController->RightClickAction,
					ETriggerEvent::Triggered,
					this,
					&ANBC_UAV::OnRightClickStart
				);

				EnhancedInput->BindAction(
					PlayerController->RightClickAction,
					ETriggerEvent::Completed,
					this,
					&ANBC_UAV::OnRightClickStop
				);
			}
		}
	}
}

void ANBC_UAV::MoveForward(const FInputActionValue& Value)
{
	if (!Controller) return;

	// 입력 값 가져오기
	const FVector2D MoveInput = Value.Get<FVector2D>();

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
		CurrentVelocity = Direction * MaxSpeed;
		AddActorLocalOffset(CurrentVelocity * GetWorld()->GetDeltaSeconds(), true);

		// 전진/후진 시 Pitch 회전 적용
		float TargetPitch = MoveInput.X * -15.0f;
		FRotator NewRotation = StaticMeshComp->GetRelativeRotation();
		NewRotation.Pitch = FMath::FInterpTo(NewRotation.Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 5.0f);
		StaticMeshComp->SetRelativeRotation(NewRotation);
	}
}

void ANBC_UAV::MoveRight(const FInputActionValue& Value)
{
	if (!Controller) return;

	// 입력 값 가져오기
	const FVector2D MoveInput = Value.Get<FVector2D>();

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
		CurrentVelocity = Direction * MaxSpeed;
		AddActorLocalOffset(CurrentVelocity * GetWorld()->GetDeltaSeconds(), true);

		// 좌우 이동 시 Roll 회전 적용
		float TargetRoll = MoveInput.Y * 15.0f;
		FRotator NewRotation = StaticMeshComp->GetRelativeRotation();
		NewRotation.Roll = FMath::FInterpTo(NewRotation.Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 5.0f);
		StaticMeshComp->SetRelativeRotation(NewRotation);
	}
}

void ANBC_UAV::MoveUp(const FInputActionValue& Value)
{
	// 입력 값 가져오기
	const float MoveInput = Value.Get<float>();

	if (!FMath::IsNearlyZero(MoveInput))
	{
		// 카메라의 Up 벡터 사용
		FVector NormalVector = GetActorUpVector();

		// 벡터 정규화
		NormalVector.Normalize();

		// 월드 기준 벡터를 로컬 좌표계로 변환
		FVector LocalForward = GetActorTransform().InverseTransformVector(NormalVector);

		// 이동 방향 설정
		FVector Direction = (LocalForward * MoveInput).GetSafeNormal();

		// Z축 이동
		CurrentVelocity = Direction * MaxSpeed;
		AddActorLocalOffset(CurrentVelocity * GetWorld()->GetDeltaSeconds(), true);
	}
}

void ANBC_UAV::StopMove(const FInputActionValue& Value)
{
	CurrentVelocity = FVector::ZeroVector;
}

void ANBC_UAV::Turn(const FInputActionValue& Value)
{
	// 유효성 검사
	if (!SpringArmComp && !Controller) return;

	// 마우스 이동에 의한 X, Y 회전 입력 값
	FVector2D LookInput = Value.Get<FVector2D>();

	// 스프링암 회전 값 가져오기
	FRotator SpringArmRotation = SpringArmComp->GetRelativeRotation();

	// Yaw 회전 (좌우 회전)
	if (!bIsRightClicking)
	{
		// 우클릭 중이 아닐 때는 컨트롤러 자체 회전
		FRotator NewControlRotation = Controller->GetControlRotation();
		NewControlRotation.Yaw += LookInput.X * RotationSensitivity;
	}
	else
	{
		// 우클릭 중일 때 스프링암 회전
		SpringArmRotation.Yaw += LookInput.X * RotationSensitivity;
	}

	// Pitch 회전 (상하 회전) - SpringArm에 적용
	SpringArmRotation.Pitch = FMath::Clamp(SpringArmRotation.Pitch - LookInput.Y * RotationSensitivity, -85.0f, 85.0f);

	// SpringArm 회전 적용
	SpringArmComp->SetRelativeRotation(SpringArmRotation);
}

void ANBC_UAV::Tilt(const FInputActionValue& Value)
{
	// 유효성 검사
	if (!SpringArmComp && !Controller) return;

	// 입력 값 가져오기
	const float LookInput = Value.Get<float>();

	// Roll 회전 (Tilt 회전) - 카메라 방향 기준
	FRotator NewControlRotation = Controller->GetControlRotation();
	NewControlRotation.Roll = FMath::Clamp(NewControlRotation.Roll + LookInput * RotationSensitivity, -30.f, 30.f);

	// ControlRotation 적용
	Controller->SetControlRotation(NewControlRotation);
}

void ANBC_UAV::ResetTilt(const FInputActionValue& Value)
{
	bResetTiltRequested = true;
}

void ANBC_UAV::OnRightClickStart(const FInputActionValue& Value)
{
	if (Value.Get<float>())
	{
		bIsRightClicking = true;
		bShouldInterpBack = false;
	}
}

void ANBC_UAV::OnRightClickStop(const FInputActionValue& Value)
{
	if (!Value.Get<float>())
	{
		bIsRightClicking = false;
		bShouldInterpBack = true;
	}
}
