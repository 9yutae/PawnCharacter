#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NBC_AnimInstance.generated.h"

UCLASS()
class PAWNCHARACTER_API UNBC_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
    // �ִϸ��̼� �Ķ���͵�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float Velocity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool IsMoving;

};
