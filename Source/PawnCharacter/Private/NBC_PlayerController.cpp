#include "NBC_PlayerController.h"
#include "EnhancedInputSubsystems.h"

ANBC_PlayerController::ANBC_PlayerController()
    : InputMappingContext(nullptr),
      MoveAction(nullptr),
      JumpAction(nullptr),
      LookAction(nullptr),
      SprintAction(nullptr)
{
}

void ANBC_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                SubSystem->AddMappingContext(InputMappingContext, 0);
            }
        }
    }
}
