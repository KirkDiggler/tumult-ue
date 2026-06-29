#include "tumult/effects/bleed.hpp"

#include <utility>

#include "rpg/core/bus.hpp"
#include "rpg/core/status.hpp"
#include "rpg/core/topic.hpp"
#include "tumult/encounter.hpp"

namespace tumult {

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
BleedEffect::BleedEffect(std::string targetId, int damagePerStack, int stacks)
    : rpg::core::Effect("bleed-" + targetId, "rend"),
      targetId_(std::move(targetId)),
      damagePerStack_(damagePerStack),
      stacks_(stacks) {}

rpg::core::Status BleedEffect::onApply(rpg::core::Bus& bus) {
  rpg::core::Topic<int> turnTopic = turnEndedTopic().on(bus);
  auto id = turnTopic.subscribe([this, &bus](const int& /*turn*/) -> rpg::core::Status {
    if (stacks_ <= 0) {
      return rpg::core::Status::ok();
    }

    DamageEvent req{
        .attackerId = "bleed-" + targetId_, .targetId = targetId_, .baseAmount = damagePerStack_};
    rpg::core::Topic<DamageEvent> raw = rawDamageRequestedTopic().on(bus);
    rpg::core::Status status = raw.publish(req);
    if (!status.isOk()) {
      return status;
    }

    --stacks_;
    if (stacks_ <= 0) {
      // Self-remove: Effect::remove() sweeps tracked subscriptions (including
      // this one). Propagate the status so a removal failure surfaces to the
      // turn.ended publisher instead of hiding behind a hardcoded ok().
      auto removeResult = remove();
      if (!removeResult.first.isOk()) {
        return removeResult.first;
      }
    }
    return rpg::core::Status::ok();
  });
  track(id);
  return rpg::core::Status::ok();
}

}  // namespace tumult
