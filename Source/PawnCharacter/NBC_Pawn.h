#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NBC_Pawn.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;

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

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
