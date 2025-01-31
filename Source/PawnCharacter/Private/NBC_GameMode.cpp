#include "NBC_GameMode.h"
#include "PawnCharacter/NBC_Pawn.h"
#include "NBC_PlayerController.h"

ANBC_GameMode::ANBC_GameMode()
{
	// DefaultPawnClass ����
	DefaultPawnClass = ANBC_Pawn::StaticClass();
	
	// Default Player Controller ����
	PlayerControllerClass = ANBC_PlayerController::StaticClass();

}
