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
	void Look(const FInputActionValue& value);

#ifdef JUMPING
	void StartJump(const FInputActionValue& value);
	void StopJump(const FInputActionValue& value);
#endif

#ifdef SPRINT
	void StartSprint(const FInputActionValue& value);
	void StopSprint(const FInputActionValue& value);
#endif

public:	
	virtual void Tick(float DeltaTime) override;
	FVector GetVelocity();
	void UpdateMovementSpeed();

private:
	float NormalSpeed;
	float RotationSensitivity;
	float SprintSpeedMultiplier;
	float SprintSpeed;
	FVector PreviousLocation;
};
