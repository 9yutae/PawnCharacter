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

	// 루트 컴포넌트 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "NBC_Pawn|Components")
	UCapsuleComponent* Capsule;

	// 스켈레탈 메시 컴포넌트 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NBC_Pawn|Components")
	USkeletalMeshComponent* SkeletalMeshComp;

	// 스프링 암 컴포넌트 포인터
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	// 카메라 컴포넌트 포인터
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
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "NBC_Pawn|Movements")
	float JumpImpulse;

protected:
	virtual void BeginPlay() override;

	// 입력 바인딩을 처리할 함수 (IA 함수 연결)
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// IA 함수를 처리할 함수 원형
	// Enhanced Input에서 액션 값은 FInputActionValue로 전달된다.
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
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);
	UFUNCTION()
	void OnRightClickStart(const FInputActionValue& Value);
	UFUNCTION()
	void OnRightClickStop(const FInputActionValue& Value);

public:	
	virtual void Tick(float DeltaTime) override;
	FVector GetVelocity();
	void UpdateMovementSpeed();
	bool IsGrounded();

private:
	FVector PreviousLocation;
	FRotator InitialCameraRotation;
	bool bIsRightClicking;
	bool bShouldInterpBack;

	// 중력 관련 변수
	const float Gravity = -980.f; // 중력 가속도 (cm/s^2)
	float VerticalVelocity = 0.f;
	bool bIsInAir;
};
