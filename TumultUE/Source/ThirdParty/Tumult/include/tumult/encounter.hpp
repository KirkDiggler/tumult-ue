#ifndef TUMULT_ENCOUNTER_HPP_
#define TUMULT_ENCOUNTER_HPP_

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "rpg/core/bus.hpp"
#include "rpg/core/chain.hpp"
#include "rpg/core/effect.hpp"
#include "rpg/core/topic.hpp"
#include "tumult/breakdown.hpp"
#include "tumult/character.hpp"

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

  rpg::core::Bus& bus();

  std::pair<rpg::core::Status, rpg::core::EffectReceipt> applyEffect(rpg::core::Effect& effect);
  std::pair<rpg::core::Status, rpg::core::EffectReceipt> removeEffect(rpg::core::Effect& effect);

  StrikeResult strike(const std::string& attacker, const std::string& target, int base);
  void addBlock(const std::string& fighter, int amount);

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
};

}  // namespace tumult

#endif  // TUMULT_ENCOUNTER_HPP_
