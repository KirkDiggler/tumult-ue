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
	FString Message = FString::Printf(
		TEXT("Tumult encounter: strike dealt %d dmg"), Result.finalDamage);
	for (const tumult::CombatantView& Combatant : Encounter_.combatants())
	{
		const FString Name(UTF8_TO_TCHAR(Combatant.name.c_str()));
		Message += FString::Printf(
			TEXT(" | %s HP %d/%d %s"),
			*Name,
			Combatant.curHp,
			Combatant.maxHp,
			Combatant.alive ? TEXT("alive") : TEXT("down"));
	}

	UE_LOG(LogTemp, Display, TEXT("%s"), *Message);
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Message);
	}
}
