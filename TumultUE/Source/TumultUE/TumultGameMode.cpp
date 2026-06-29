// Copyright Epic Games, Inc. All Rights Reserved.

#include "TumultGameMode.h"

#include "Engine/Engine.h"

void ATumultGameMode::BeginPlay()
{
	Super::BeginPlay();

	const tumult::Character Hero{.id = "hero", .name = "Hero", .curHp = 30, .maxHp = 30, .block = 0};
	const tumult::Character Goblin{.id = "goblin", .name = "Goblin", .curHp = 20, .maxHp = 20, .block = 0};
	Encounter_.setup(Hero, Goblin);

	const tumult::StrikeResult Result = Encounter_.strike("hero", "goblin", 5);
	const FString Message = FString::Printf(
		TEXT("Tumult strike: %d dmg, goblin HP %d"), Result.finalDamage, Result.hpAfter);

	UE_LOG(LogTemp, Display, TEXT("%s"), *Message);
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Message);
	}
}
