#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NBC_AnimInstance.generated.h"

UCLASS()
class PAWNCHARACTER_API UNBC_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
    // 애니메이션 파라미터들
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector Velocity;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float GroundSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool IsMoving;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool IsFalling;

};
