#include "NBC_Pawn.h"
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

	// ������Ʈ ������ �Ҵ�
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// Root�� SpringArm ����
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f; // ���������� ����
	SpringArmComp->bUsePawnControlRotation = true; // ��Ʈ�ѷ��� ȸ���� �� �������ϵ� ���� �̵��ϵ��� ����

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


void ANBC_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANBC_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

