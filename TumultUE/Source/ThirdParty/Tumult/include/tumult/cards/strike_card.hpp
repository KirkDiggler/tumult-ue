#ifndef TUMULT_CARDS_STRIKE_CARD_HPP_
#define TUMULT_CARDS_STRIKE_CARD_HPP_

#include <string>
#include <utility>

#include "rpg/core/action.hpp"
#include "rpg/core/entity.hpp"
#include "rpg/core/status.hpp"
#include "tumult/export.hpp"

namespace tumult {

class Encounter;  // forward-declared; strike_card.cpp drives strike() through it

// The typed input a StrikeCard needs: which target to hit and the base damage.
// (Energy/cost is B3 — not modeled here.) rpgkit's Action never looks inside
// TInput; only the card and Encounter::playCard's caller do.
struct StrikeInput {
  std::string targetId;
  int base = 0;
};

// The first card. A card is an Action (rpgkit decision 10: the transient verb —
// gate, spend, publish, done; an Effect persists, an Action fires once).
// StrikeCard OWNS its behavior: activate() drives the Encounter's strike() under
// the hood (card-runs, DR-011). Encounter::playCard stays generic over the
// input, so a new card is a new Action subclass, never a new playCard branch.
//
// Identity (id/type) is supplied at construction and echoed onto the receipt;
// the renderer reads it back (formatCardPlay) — never a hardcoded label (the
// formatStep discipline, one altitude up).
class TUMULT_API StrikeCard : public rpg::core::Action<StrikeInput> {
 public:
  // The card holds a reference to the owning Encounter so activate() can reach
  // strike(); the targeting/base data rides in StrikeInput, not the constructor.
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  StrikeCard(Encounter& encounter, std::string id, std::string type);

  // Gate: the target exists and is alive. Never mutates (the gate is a yes/no).
  [[nodiscard]] rpg::core::Status canActivate(const rpg::core::EntityRef& owner,
                                              const StrikeInput& input) override;

  // Fire: re-check the gate, then drive strike() under the hood, threading the
  // correlationId so playCard can recover the rich StrikeResult from the
  // StrikeResolved strike() publishes. The receipt names the card (id/type),
  // populated even on gate-rejection.
  [[nodiscard]] std::pair<rpg::core::Status, rpg::core::ActionReceipt> activate(
      ActivateParams params) override;

 private:
  Encounter* encounter_;  // non-owning; the encounter outlives the transient card
};

}  // namespace tumult

#endif  // TUMULT_CARDS_STRIKE_CARD_HPP_
