#include "NBC_Pawn.h"
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

	// 컴포넌트 포인터 할당
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// Root에 SpringArm 부착
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f; // 스프링암의 길이
	SpringArmComp->bUsePawnControlRotation = true; // 컨트롤러를 회전할 때 스프링암도 같이 이동하도록 설정

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


void ANBC_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANBC_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

