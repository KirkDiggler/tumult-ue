#include "tumult/effects/tough_skin.hpp"

#include <algorithm>
#include <utility>

#include "rpg/core/bus.hpp"
#include "rpg/core/chain.hpp"
#include "rpg/core/status.hpp"
#include "rpg/core/topic.hpp"
#include "tumult/encounter.hpp"

namespace tumult {

ToughSkinEffect::ToughSkinEffect(std::string protectedId, int reduction)
    : rpg::core::Effect("tough-skin-" + protectedId, "tough-skin"),
      protectedId_(std::move(protectedId)),
      reduction_(reduction) {}

rpg::core::Status ToughSkinEffect::onApply(rpg::core::Bus& bus) {
  rpg::core::ChainedTopic<DamageEvent> topic = combatDamageTopic().onChained(bus);
  auto id = topic.subscribe(
      [this](const DamageEvent& event, rpg::core::Chain<DamageEvent>& chain) -> rpg::core::Status {
        if (event.targetId != protectedId_) {
          return rpg::core::Status::ok();
        }
        const int reduction = reduction_;
        return chain.add({.stage = "effects",
                          .id = "tough-skin-" + protectedId_,
                          .modifier = [reduction](DamageEvent e) -> DamageEvent {
                            e.baseAmount = std::max(0, e.baseAmount - reduction);
                            return e;
                          },
                          .source = "tough-skin"});
      });
  track(id);
  return rpg::core::Status::ok();
}

}  // namespace tumult
