#include "tumult/effects/vulnerable.hpp"

#include <cmath>
#include <utility>

#include "rpg/core/bus.hpp"
#include "rpg/core/chain.hpp"
#include "rpg/core/status.hpp"
#include "rpg/core/topic.hpp"
#include "tumult/encounter.hpp"

namespace tumult {

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
VulnerableEffect::VulnerableEffect(std::string targetId, int percentBonus, int remainingTurns)
    : rpg::core::Effect("vulnerable-" + targetId, "vulnerable"),
      targetId_(std::move(targetId)),
      percentBonus_(percentBonus),
      remainingTurns_(remainingTurns) {}

rpg::core::Status VulnerableEffect::onApply(rpg::core::Bus& bus) {
  rpg::core::ChainedTopic<DamageEvent> damageTopic = combatDamageTopic().onChained(bus);
  auto id = damageTopic.subscribe(
      [this](const DamageEvent& event, rpg::core::Chain<DamageEvent>& chain) -> rpg::core::Status {
        if (remainingTurns_ <= 0 || event.targetId != targetId_) {
          return rpg::core::Status::ok();
        }
        const int bonus = percentBonus_;
        return chain.add({.stage = "effects",
                          .id = "vulnerable-" + targetId_,
                          .modifier = [bonus](DamageEvent e) -> DamageEvent {
                            e.baseAmount = static_cast<int>(
                                std::ceil(static_cast<float>(e.baseAmount) *
                                          (1.0F + (static_cast<float>(bonus) / 100.0F))));
                            return e;
                          },
                          .source = "vulnerable"});
      });
  track(id);

  rpg::core::Topic<int> turnTopic = turnEndedTopic().on(bus);
  auto turnId = turnTopic.subscribe([this](const int& /*turnNumber*/) -> rpg::core::Status {
    if (remainingTurns_ > 0) {
      --remainingTurns_;
    }
    return rpg::core::Status::ok();
  });
  track(turnId);
  return rpg::core::Status::ok();
}

}  // namespace tumult
