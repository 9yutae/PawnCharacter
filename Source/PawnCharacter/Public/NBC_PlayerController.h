#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NBC_PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class PAWNCHARACTER_API ANBC_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANBC_PlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SprintAction;

protected:
	void BeginPlay();
	
};
