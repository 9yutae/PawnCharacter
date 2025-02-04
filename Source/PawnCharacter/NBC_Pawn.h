#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NBC_Pawn.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class PAWNCHARACTER_API ANBC_Pawn : public APawn
{
	GENERATED_BODY()

public:
	ANBC_Pawn();

	// ��Ʈ ������Ʈ ������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "NBC_Pawn|Components")
	UCapsuleComponent* Capsule;

	// ���̷�Ż �޽� ������Ʈ ������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NBC_Pawn|Components")
	USkeletalMeshComponent* SkeletalMeshComp;

	// ������ �� ������Ʈ ������
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	// ī�޶� ������Ʈ ������
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "NBC_Pawn|Movements")
	float Acceleration;
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "NBC_Pawn|Movements")
	float NormalSpeed;
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "NBC_Pawn|Movements")
	float MaxWalkSpeed;
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "NBC_Pawn|Movements")
	float CurrentPlaneSpeed;
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "NBC_Pawn|Movements")
	float RotationSensitivity;
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "NBC_Pawn|Movements")
	float SprintSpeedMultiplier;
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "NBC_Pawn|Movements")
	float SprintSpeed;

protected:
	virtual void BeginPlay() override;

	// �Է� ���ε��� ó���� �Լ� (IA �Լ� ����)
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// IA �Լ��� ó���� �Լ� ����
	// Enhanced Input���� �׼� ���� FInputActionValue�� ���޵ȴ�.
	UFUNCTION()
	void MoveForward(const FInputActionValue& value);
	UFUNCTION()
	void MoveRight(const FInputActionValue& value);
	UFUNCTION()
	void ApplyCharacterRotation(float TargetYaw);
	UFUNCTION()
	void StopMove(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);

#ifdef JUMPING
	void StartJump(const FInputActionValue& value);
	void StopJump(const FInputActionValue& value);
#endif
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

public:	
	virtual void Tick(float DeltaTime) override;
	FVector GetVelocity();
	void UpdateMovementSpeed();
	bool IsGrounded();

private:
	FVector PreviousLocation;

	// �߷� ���� ����
	const float Gravity = -980.f; // �߷� ���ӵ� (cm/s^2)
	float VerticalVelocity = 0.f;
	bool bIsInAir;
};
