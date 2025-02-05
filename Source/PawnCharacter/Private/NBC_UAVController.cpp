#include "NBC_UAVController.h"
#include "EnhancedInputSubsystems.h"

ANBC_UAVController::ANBC_UAVController()
    : InputMappingContext(nullptr),
    MoveAction(nullptr),
    UpDownAction(nullptr),
    LookAction(nullptr),
    TiltAction(nullptr),
    RightClickAction(nullptr)
{
}

void ANBC_UAVController::BeginPlay()
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
