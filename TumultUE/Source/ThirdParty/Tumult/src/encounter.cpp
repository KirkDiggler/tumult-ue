#include "tumult/encounter.hpp"

#include <algorithm>
#include <cassert>
#include <utility>

#include "rpg/core/chain.hpp"
#include "rpg/core/topic.hpp"
#include "tumult/breakdown.hpp"

namespace tumult {

Encounter::Encounter() = default;

Encounter::~Encounter() { shutdown(); }

void Encounter::setup(const Character& hero, const Character& enemy) {
  shutdown();
  bus_ = std::make_unique<rpg::core::Bus>();
  hero_ = hero;
  enemy_ = enemy;
  hasHero_ = true;
  hasEnemy_ = true;
  ready_ = false;
  subscribeRequestTopics();
  ready_ = true;
}

void Encounter::shutdown() {
  if (bus_) {
    // The Bus's destructor sweeps its subscriber tables — including the
    // tracked subscriptions held by any still-active effects — without
    // touching the Effect objects themselves. We intentionally do NOT call
    // effect->remove() here: the caller owns the effects and they may
    // already be destroyed (stack-allocated effects in tests destruct
    // before the Encounter, so a remove() would dereference freed memory).
    // We just drop our pointer vector so we don't carry stale references.
    unsubscribeRequestTopics();
    bus_.reset();
  }
  activeEffects_.clear();
  hasHero_ = false;
  hasEnemy_ = false;
  ready_ = false;
}

const Character* Encounter::findCharacter(const std::string& id) const {
  if (hasHero_ && hero_.id == id) {
    return &hero_;
  }
  if (hasEnemy_ && enemy_.id == id) {
    return &enemy_;
  }
  return nullptr;
}

namespace {

// Curate a Character into its host-facing view: drop the internal-only block,
// materialize `alive` from the same predicate the Character already exposes.
CombatantView viewOf(const Character& character) {
  return CombatantView{.id = character.id,
                       .name = character.name,
                       .curHp = character.curHp,
                       .maxHp = character.maxHp,
                       .alive = character.isAlive()};
}

}  // namespace

std::vector<CombatantView> Encounter::combatants() const {
  std::vector<CombatantView> views;
  views.reserve(2);
  if (hasHero_) {
    views.push_back(viewOf(hero_));
  }
  if (hasEnemy_) {
    views.push_back(viewOf(enemy_));
  }
  return views;
}

rpg::core::Bus& Encounter::bus() {
  // Precondition: call only while ready (after setup, before shutdown).
  // bus_ is null before setup and after shutdown; dereferencing it is UB.
  // The architectural alternative (construct bus_ in the ctor so the
  // not-ready state is unrepresentable) conflicts with shutdown()'s
  // bus-dies-on-shutdown model — the bus destructor sweeps effect
  // subscriptions, which we can't do via effect->remove() in shutdown
  // (caller-owned effects may already be destroyed). Revisit when a
  // cross-encounter effect-subscription-tracking story exists.
  assert(bus_);
  return *bus_;
}

std::pair<rpg::core::Status, rpg::core::EffectReceipt> Encounter::applyEffect(
    rpg::core::Effect& effect) {
  if (!ready_ || !bus_) {
    return {rpg::core::Status::error("encounter not ready"),
            rpg::core::EffectReceipt{.id = effect.id(),
                                     .source = effect.source(),
                                     .subscriptions = {},
                                     .correlationId = ""}};
  }
  auto it = std::ranges::find(activeEffects_, &effect);
  if (it != activeEffects_.end()) {
    if (effect.isActive()) {
      return {rpg::core::Status::error("effect already applied: " + effect.id()),
              rpg::core::EffectReceipt{.id = effect.id(),
                                       .source = effect.source(),
                                       .subscriptions = {},
                                       .correlationId = ""}};
    }
    // Effect self-removed (e.g., Bleed at 0 stacks) but the pointer is still
    // in activeEffects_. Drop the stale entry and proceed to re-apply.
    activeEffects_.erase(it);
  }
  auto result = effect.apply({.bus = *bus_});
  if (result.first.isOk()) {
    activeEffects_.push_back(&effect);
  }
  return result;
}

std::pair<rpg::core::Status, rpg::core::EffectReceipt> Encounter::removeEffect(
    rpg::core::Effect& effect) {
  auto it = std::ranges::find(activeEffects_, &effect);
  if (it == activeEffects_.end()) {
    return {rpg::core::Status::error("effect not active: " + effect.id()),
            rpg::core::EffectReceipt{.id = effect.id(),
                                     .source = effect.source(),
                                     .subscriptions = {},
                                     .correlationId = ""}};
  }
  auto result = effect.remove();
  if (result.first.isOk()) {
    activeEffects_.erase(it);
  }
  return result;
}

void Encounter::subscribeRequestTopics() {
  // Subscribe base identity modifier for combat.damage chain (required for Task 5)
  rpg::core::ChainedTopic<DamageEvent> topic = combatDamageTopic().onChained(*bus_);
  baseDamageSubId_ = topic.subscribe(
      [](const DamageEvent& /*event*/, rpg::core::Chain<DamageEvent>& chain) -> rpg::core::Status {
        return chain.add({.stage = "base",
                          .id = "",
                          .modifier = [](DamageEvent e) -> DamageEvent { return e; },
                          .source = ""});
      });

  rpg::core::Topic<DamageEvent> rawTopic = rawDamageRequestedTopic().on(*bus_);
  rawDamageSubId_ = rawTopic.subscribe([this](const DamageEvent& req) -> rpg::core::Status {
    return dealRawDamage(req.targetId, req.baseAmount);
  });

  rpg::core::Topic<DamageEvent> blockTopic = blockRequestedTopic().on(*bus_);
  blockSubId_ = blockTopic.subscribe([this](const DamageEvent& req) -> rpg::core::Status {
    addBlock(req.targetId, req.baseAmount);
    return rpg::core::Status::ok();
  });
}

void Encounter::unsubscribeRequestTopics() {
  if (baseDamageSubId_.value != 0) {
    (void)bus_->unsubscribe(baseDamageSubId_);
    baseDamageSubId_ = rpg::core::SubscriptionId{};
  }
  if (rawDamageSubId_.value != 0) {
    (void)bus_->unsubscribe(rawDamageSubId_);
    rawDamageSubId_ = rpg::core::SubscriptionId{};
  }
  if (blockSubId_.value != 0) {
    (void)bus_->unsubscribe(blockSubId_);
    blockSubId_ = rpg::core::SubscriptionId{};
  }
}

Character* Encounter::mutCharacter(const std::string& id) {
  if (hasHero_ && hero_.id == id) {
    return &hero_;
  }
  if (hasEnemy_ && enemy_.id == id) {
    return &enemy_;
  }
  return nullptr;
}

StrikeResult Encounter::strike(const std::string& attacker, const std::string& target, int base) {
  StrikeResult result;
  if (!ready_ || !bus_) {
    return result;
  }

  Character* targetPtr = mutCharacter(target);
  if (targetPtr == nullptr) {
    return result;
  }

  DamageEvent event{.attackerId = attacker, .targetId = target, .baseAmount = base};
  rpg::core::Chain<DamageEvent> chain(damageStages());
  rpg::core::ChainedTopic<DamageEvent> topic = combatDamageTopic().onChained(*bus_);
  rpg::core::Status status = topic.publish(event, chain);
  if (!status.isOk()) {
    return result;
  }

  auto chainResult = chain.execute(event);
  result.finalDamage = chainResult.value.baseAmount;

  for (const auto& step : chainResult.breakdown) {
    result.breakdown.push_back({
        .id = step.id,
        .stage = step.stage,
        .source = step.source,
        .before = step.before.baseAmount,
        .after = step.after.baseAmount,
    });
  }

  int blocked = std::min(targetPtr->block, result.finalDamage);
  targetPtr->block -= blocked;
  result.blocked = blocked;
  int hpDamage = result.finalDamage - blocked;
  targetPtr->curHp = std::max(0, targetPtr->curHp - hpDamage);
  result.hpAfter = targetPtr->curHp;

  // Broadcast the resolved strike AFTER state is mutated, so a subscriber that
  // reads combatants() in its handler sees post-strike HP. The synchronous
  // answer remains the returned StrikeResult; this event is the push edge for
  // observers (HUD, combat-log sink). A notification-delivery error has no
  // recovery path here and must not alter the return — intentionally ignored.
  rpg::core::Topic<StrikeResolved> resolvedTopic = strikeResolvedTopic().on(*bus_);
  (void)resolvedTopic.publish(
      StrikeResolved{.attackerId = attacker, .targetId = target, .result = result});

  return result;
}

void Encounter::addBlock(const std::string& fighter, int amount) {
  Character* who = mutCharacter(fighter);
  if (who != nullptr) {
    who->block += amount;
  }
}

rpg::core::Status Encounter::dealRawDamage(const std::string& target, int amount) {
  Character* who = mutCharacter(target);
  if (who == nullptr) {
    return rpg::core::Status::error("unknown target: " + target);
  }
  who->curHp = std::max(0, who->curHp - amount);
  return rpg::core::Status::ok();
}

rpg::core::Status Encounter::endTurn() {
  if (!ready_ || !bus_) {
    return rpg::core::Status::error("encounter not ready");
  }
  ++turnNumber_;
  rpg::core::Topic<int> topic = turnEndedTopic().on(*bus_);
  return topic.publish(turnNumber_);
}

bool Encounter::isEffectActive(const rpg::core::Effect& effect) const {
  auto it = std::ranges::find(activeEffects_, &effect);
  return it != activeEffects_.end() && effect.isActive();
}

}  // namespace tumult
