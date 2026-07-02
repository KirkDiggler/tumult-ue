// Copyright Epic Games, Inc. All Rights Reserved.

#include "TumultGameMode.h"

#include "Engine/Engine.h"

#include <string>

namespace
{
FString ToFString(const std::string& Value)
{
	return FString(UTF8_TO_TCHAR(Value.c_str()));
}
}  // namespace

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
		const FString Name = ToFString(Combatant.name);
		Message += FString::Printf(
			TEXT(" | %s HP %d/%d %s"),
			*Name,
			Combatant.curHp,
			Combatant.maxHp,
			Combatant.alive ? TEXT("alive") : TEXT("down"));
	}

	tumult::StrikeCard StrikeOne(Encounter_, "strike-1", "strike");
	tumult::StrikeCard StrikeTwo(Encounter_, "strike-2", "strike");
	tumult::StrikeCard HeavyStrike(Encounter_, "heavy-strike-1", "strike");

	const auto AppendCard = [&Message](
		int32 Slot, const tumult::StrikeCard& Card, const tumult::StrikeInput& Input)
	{
		Message += FString::Printf(
			TEXT(" | [%d] %s (%s): %d dmg -> %s"),
			Slot,
			*ToFString(Card.id()),
			*ToFString(Card.type()),
			Input.base,
			*ToFString(Input.targetId));
	};

	AppendCard(1, StrikeOne, tumult::StrikeInput{.targetId = "goblin", .base = 5});
	AppendCard(2, StrikeTwo, tumult::StrikeInput{.targetId = "goblin", .base = 5});
	AppendCard(3, HeavyStrike, tumult::StrikeInput{.targetId = "goblin", .base = 8});

	UE_LOG(LogTemp, Display, TEXT("%s"), *Message);
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Message);
	}
}
