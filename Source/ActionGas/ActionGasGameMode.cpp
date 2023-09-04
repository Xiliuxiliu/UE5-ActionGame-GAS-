// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionGasGameMode.h"
#include "ActionGasCharacter.h"
#include "UObject/ConstructorHelpers.h"

AActionGasGameMode::AActionGasGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
