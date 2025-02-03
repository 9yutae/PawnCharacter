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
