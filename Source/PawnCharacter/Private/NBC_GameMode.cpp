#include "NBC_GameMode.h"
#include "PawnCharacter/NBC_Pawn.h"
#include "NBC_PlayerController.h"

ANBC_GameMode::ANBC_GameMode()
{
	// DefaultPawnClass 설정
	DefaultPawnClass = ANBC_Pawn::StaticClass();
	
	// Default Player Controller 설정
	PlayerControllerClass = ANBC_PlayerController::StaticClass();

}
