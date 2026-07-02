#ifndef TUMULT_ENCOUNTER_HPP_
#define TUMULT_ENCOUNTER_HPP_

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "rpg/core/action.hpp"
#include "rpg/core/bus.hpp"
#include "rpg/core/chain.hpp"
#include "rpg/core/effect.hpp"
#include "rpg/core/entity.hpp"
#include "rpg/core/topic.hpp"
#include "tumult/breakdown.hpp"
#include "tumult/character.hpp"
#include "tumult/export.hpp"

namespace tumult {

// Authoritative damage event on the chained combat.damage topic.
struct DamageEvent {
  std::string attackerId;
  std::string targetId;
  int baseAmount = 0;
};

// Topic definitions — declared once, bound to the Encounter's Bus.
inline const rpg::core::TopicDef<DamageEvent>& combatDamageTopic() {
  static const rpg::core::TopicDef<DamageEvent> kDef{"combat.damage"};
  return kDef;
}
inline const rpg::core::TopicDef<int>& turnEndedTopic() {
  static const rpg::core::TopicDef<int> kDef{"turn.ended"};
  return kDef;
}
inline const rpg::core::TopicDef<DamageEvent>& rawDamageRequestedTopic() {
  static const rpg::core::TopicDef<DamageEvent> kDef{"combat.raw_damage.requested"};
  return kDef;
}
inline const rpg::core::TopicDef<DamageEvent>& blockRequestedTopic() {
  static const rpg::core::TopicDef<DamageEvent> kDef{"combat.block.requested"};
  return kDef;
}
inline const std::vector<std::string>& damageStages() {
  static const std::vector<std::string> kStages = {"base", "effects", "final"};
  return kStages;
}

struct StrikeResult {
  int finalDamage = 0;
  int blocked = 0;
  int hpAfter = 0;
  std::vector<BreakdownStep> breakdown;
};

// Resolved-strike notification. strike() publishes this on the Encounter Bus
// AFTER it mutates state and STILL returns StrikeResult unchanged: the return
// is the caller's synchronous answer (pull), the event is the broadcast a HUD
// or combat-log sink subscribes to once and reacts to each loop iteration
// (push). It reuses StrikeResult as the single outcome shape — it is NOT the
// request-shaped DamageEvent. Notification flavor (Topic<T>), as turn.ended.
struct StrikeResolved {
  std::string attackerId;
  std::string targetId;
  StrikeResult result;
  // Echoed from the strike()'s correlationId (B1). Lets Encounter::playCard
  // recover THIS strike's StrikeResult — which can't ride the thin, identity-
  // only ActionReceipt that activate() returns. Default "" (a direct strike
  // that doesn't correlate); A2 callers/subscribers that ignore it are
  // unaffected.
  std::string correlationId;
};

inline const rpg::core::TopicDef<StrikeResolved>& strikeResolvedTopic() {
  static const rpg::core::TopicDef<StrikeResolved> kDef{"combat.strike.resolved"};
  return kDef;
}

// --- B1: the card verb layer (parallel to strike, reuses it) ----------------

// The rich outcome of playing a card: the card's identity receipt (from the
// Action) bundled with the existing strike breakdown. ActionReceipt is
// identity-only, so the breakdown can't leave through activate()'s fixed
// return — playCard recovers it out-of-band (via StrikeResolved correlation)
// and bundles it here (DR-011).
struct CardResult {
  rpg::core::ActionReceipt receipt;
  StrikeResult strike;
};

// Card-altitude notification: published once per successful playCard, AFTER the
// mutation (mirrors StrikeResolved one level up — the return is the pull, the
// event is the push). A host that cares about cards subscribes here; the
// underlying StrikeResolved still fires at the mechanism altitude (both, by
// design — different altitudes, not double-counting). A distinct type (not a
// bare CardResult) so the two altitudes never alias on one topic id.
struct CardResolved {
  CardResult result;
};

inline const rpg::core::TopicDef<CardResolved>& cardResolvedTopic() {
  static const rpg::core::TopicDef<CardResolved> kDef{"card.resolved"};
  return kDef;
}

// Stateful runtime owner of one encounter — a bus, two characters, and
// (later) active effects + request-topic subscriptions. Mirrors rpgkit-ue's
// URPGKitEncounterRuntime: same shape, no UObject.
class TUMULT_API Encounter {
 public:
  Encounter();
  ~Encounter();

  Encounter(const Encounter&) = delete;
  Encounter& operator=(const Encounter&) = delete;
  Encounter(Encounter&&) = delete;
  Encounter& operator=(Encounter&&) = delete;

  void setup(const Character& hero, const Character& enemy);
  void shutdown();
  [[nodiscard]] bool isReady() const { return ready_; }

  [[nodiscard]] const Character* findCharacter(const std::string& id) const;

  // A value snapshot of every combatant for host display. The host enumerates
  // and renders current HP each iteration without pre-knowing ids and without
  // holding a live pointer into internal state (later mutation cannot touch a
  // returned snapshot). findCharacter(id) stays for point lookups.
  [[nodiscard]] std::vector<CombatantView> combatants() const;

  rpg::core::Bus& bus();

  std::pair<rpg::core::Status, rpg::core::EffectReceipt> applyEffect(rpg::core::Effect& effect);
  std::pair<rpg::core::Status, rpg::core::EffectReceipt> removeEffect(rpg::core::Effect& effect);

  // correlationId is additive (defaulted): when non-empty it is stamped onto the
  // StrikeResolved this strike publishes, so a caller (Encounter::playCard) can
  // match this strike's outcome. A2 callers that omit it are unaffected.
  StrikeResult strike(const std::string& attacker, const std::string& target, int base,
                      const std::string& correlationId = "");
  void addBlock(const std::string& fighter, int amount);

  // Play a card through the encounter — the generic card verb (DR-011). Generic
  // over the Action's input: it never switches on card kind, so a new card is a
  // new Action subclass, not a new branch here. It gates via the card, threads a
  // per-play correlationId so it can recover the rich StrikeResult from the
  // StrikeResolved the card's activate() publishes synchronously, bundles a
  // CardResult, and — on success — publishes CardResolved after the mutation.
  // Returns (Status, CardResult); the receipt names the card even on gate
  // rejection (never-return-(nil,nil)). Defined out-of-line below (template).
  template <typename TInput>
  std::pair<rpg::core::Status, CardResult> playCard(rpg::core::Action<TInput>& card,
                                                    const rpg::core::EntityRef& owner,
                                                    const TInput& input);

  rpg::core::Status dealRawDamage(const std::string& target, int amount);
  rpg::core::Status endTurn();
  [[nodiscard]] bool isEffectActive(const rpg::core::Effect& effect) const;

 private:
  void subscribeRequestTopics();
  void unsubscribeRequestTopics();

  Character* mutCharacter(const std::string& id);

  std::unique_ptr<rpg::core::Bus> bus_;
  Character hero_{};
  Character enemy_{};
  bool hasHero_ = false;
  bool hasEnemy_ = false;
  bool ready_ = false;

  std::vector<rpg::core::Effect*> activeEffects_;

  int turnNumber_ = 0;

  rpg::core::SubscriptionId baseDamageSubId_{};
  rpg::core::SubscriptionId rawDamageSubId_{};
  rpg::core::SubscriptionId blockSubId_{};

  // Monotonic per-encounter sequence for playCard's correlation ids — makes each
  // play's StrikeResolved unambiguously matchable, even across many plays.
  std::uint64_t cardPlaySeq_ = 0;
};

// Generic card verb (DR-011). Defined here because it is a member template; it
// uses only the public Encounter surface plus the private cardPlaySeq_.
template <typename TInput>
std::pair<rpg::core::Status, CardResult> Encounter::playCard(rpg::core::Action<TInput>& card,
                                                             const rpg::core::EntityRef& owner,
                                                             const TInput& input) {
  // A per-play correlation id so the recovery matches THIS play's strike even if
  // other strikes were to fire on the bus. Threaded activate() -> strike() ->
  // StrikeResolved (DR-011's correlation crux).
  const std::string correlationId = "card-play-" + std::to_string(++cardPlaySeq_);
  const rpg::core::ActionReceipt receipt{
      .id = card.id(), .type = card.type(), .correlationId = correlationId};

  // Not-ready guard. playCard is generic over the card, so it cannot assume the
  // card's canActivate queries the encounter — a card that gates only on its
  // input would reach bus() (which asserts) on a not-ready encounter. Return a
  // Status instead, like applyEffect()/endTurn(); the receipt still names the
  // card.
  if (!isReady()) {
    return {rpg::core::Status::error("encounter not ready"),
            CardResult{.receipt = receipt, .strike = {}}};
  }

  // Gate next — on rejection there is no mutation and no CardResolved; the
  // receipt still names the card. (activate() re-checks the gate per the Action
  // contract; the double-check is intentional and cheap.)
  const rpg::core::Status gate = card.canActivate(owner, input);
  if (!gate.isOk()) {
    return {gate, CardResult{.receipt = receipt, .strike = {}}};
  }

  // Recover the rich StrikeResult out-of-band. The Bus delivers synchronously in
  // subscription order, so subscribing here, then calling activate() (which
  // drives strike() -> publishes StrikeResolved synchronously within the call),
  // then unsubscribing, captures exactly this play's event. Match by
  // correlationId so an unrelated strike can never be mistaken for this one.
  StrikeResult recovered;
  bool recoveredHit = false;
  rpg::core::Topic<StrikeResolved> resolved = strikeResolvedTopic().on(bus());
  const rpg::core::SubscriptionId sub = resolved.subscribe(
      [&recovered, &recoveredHit, &correlationId](const StrikeResolved& ev) -> rpg::core::Status {
        if (ev.correlationId == correlationId) {
          recovered = ev.result;
          recoveredHit = true;
        }
        return rpg::core::Status::ok();
      });

  const std::pair<rpg::core::Status, rpg::core::ActionReceipt> activated =
      card.activate({.owner = owner, .input = input, .correlationId = correlationId});
  (void)bus().unsubscribe(sub);

  CardResult bundle{.receipt = activated.second, .strike = recovered};
  if (!activated.first.isOk()) {
    return {activated.first, bundle};  // strike failed post-gate: no CardResolved
  }
  // Fail loudly rather than publish an empty outcome. For B1's StrikeCard a
  // successful activate() always drives a strike we recover, so this never trips
  // here. A card that activates without a correlated strike (a non-strike card)
  // is exactly DR-011's "first non-strike card" revisit trigger — surface it as
  // an error instead of a silently-empty CardResult, do NOT publish CardResolved.
  if (!recoveredHit) {
    return {rpg::core::Status::error("playCard: no correlated strike outcome recovered"), bundle};
  }

  // Publish the card-altitude notification AFTER the mutation (mirrors strike()).
  rpg::core::Topic<CardResolved> cardTopic = cardResolvedTopic().on(bus());
  (void)cardTopic.publish(CardResolved{.result = bundle});
  return {rpg::core::Status::ok(), bundle};
}

}  // namespace tumult

#endif  // TUMULT_ENCOUNTER_HPP_
