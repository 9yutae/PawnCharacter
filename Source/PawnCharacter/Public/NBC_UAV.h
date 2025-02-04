#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NBC_UAV.generated.h"

class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class PAWNCHARACTER_API ANBC_UAV : public APawn
{
	GENERATED_BODY()

public:
	ANBC_UAV();

	// 루트 컴포넌트 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NBC_UAV|Components")
	UBoxComponent* Box;

	// 스태틱 메시 컴포넌트 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NBC_UAV|Components")
	UStaticMeshComponent* StaticMeshComp;

	// 스프링 암 컴포넌트 포인터
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	// 카메라 컴포넌트 포인터
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "NBC_UAV|Movement")
	FVector CurrentVelocity;
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "NBC_UAV|Movement")
	FRotator CurrentRotationSpeed;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "NBC_UAV|Movement")
	float MaxSpeed;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "NBC_UAV|Movement")
	float RotationSensitivity;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:	
	void MoveForward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void MoveUp(const FInputActionValue& Value);
	void Turn(const FInputActionValue& Value);
	void Tilt(const FInputActionValue& Value);
};
