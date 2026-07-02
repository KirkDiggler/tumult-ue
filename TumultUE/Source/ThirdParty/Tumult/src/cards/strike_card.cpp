#include "tumult/cards/strike_card.hpp"

#include <string>
#include <utility>

#include "rpg/core/action.hpp"
#include "rpg/core/entity.hpp"
#include "rpg/core/status.hpp"
#include "tumult/character.hpp"
#include "tumult/encounter.hpp"

namespace tumult {

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
StrikeCard::StrikeCard(Encounter& encounter, std::string id, std::string type)
    : rpg::core::Action<StrikeInput>(std::move(id), std::move(type)), encounter_(&encounter) {}

rpg::core::Status StrikeCard::canActivate(const rpg::core::EntityRef& /*owner*/,
                                          const StrikeInput& input) {
  const Character* target = encounter_->findCharacter(input.targetId);
  if (target == nullptr) {
    return rpg::core::Status::error("strike target not found: " + input.targetId);
  }
  if (!target->isAlive()) {
    return rpg::core::Status::error("strike target not alive: " + input.targetId);
  }
  return rpg::core::Status::ok();
}

std::pair<rpg::core::Status, rpg::core::ActionReceipt> StrikeCard::activate(ActivateParams params) {
  rpg::core::ActionReceipt receipt{
      .id = id(), .type = type(), .correlationId = params.correlationId};

  // Re-check the gate: the rpgkit Action contract lets callers skip straight to
  // activate(), so the gate must hold here too. On rejection, return the
  // identity-bearing receipt and DO NOT mutate — no strike is driven.
  const rpg::core::Status gate = canActivate(params.owner, params.input);
  if (!gate.isOk()) {
    return {gate, receipt};
  }

  // Cost-spend is a no-op placeholder until B3 energy.
  // Card-runs: drive the one damage mechanism, threading the correlationId so
  // Encounter::playCard can recover this play's rich StrikeResult from the
  // StrikeResolved this strike publishes (DR-011's correlation crux). The
  // StrikeResult itself rides the bus, not the thin receipt.
  (void)encounter_->strike(params.owner.id, params.input.targetId, params.input.base,
                           params.correlationId);
  return {rpg::core::Status::ok(), receipt};
}

}  // namespace tumult
