// Copyright Epic Games, Inc. All Rights Reserved.

#include "TumultGameMode.h"

#include "Engine/Engine.h"
#include "tumult/character.hpp"

void ATumultGameMode::BeginPlay()
{
	Super::BeginPlay();

	const tumult::Character Hero{.id = "hero", .name = "Hero", .curHp = 20, .maxHp = 20};
	const FString HeroName(UTF8_TO_TCHAR(Hero.name.c_str()));
	const FString Message = FString::Printf(
		TEXT("Tumult ready: %s HP %d/%d"), *HeroName, Hero.curHp, Hero.maxHp);

	UE_LOG(LogTemp, Display, TEXT("%s"), *Message);
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Message);
	}
}
