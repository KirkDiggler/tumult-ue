#ifndef TUMULT_CHARACTER_HPP_
#define TUMULT_CHARACTER_HPP_

#include <string>

namespace tumult {

// A combat participant. Value type — copy freely; identified by `id`.
// The host owns the field-of-view/order; tumult owns the HP/block numbers.
struct Character {
  std::string id;
  std::string name;
  int curHp = 0;
  int maxHp = 0;
  int block = 0;

  [[nodiscard]] bool isAlive() const { return curHp > 0; }
};

// A host-facing snapshot of one combatant — the value the read-model
// (Encounter::combatants()) hands a host each loop iteration. Mirrors
// Character cheaply (the mirror-not-wrap pattern, decision T3:
// CombatantView:Character :: BreakdownStep:Chain::Step), but curates the
// display surface: it drops the internal-only `block` and materializes
// `alive` (the same predicate as Character::isAlive(), curHp > 0). It is a
// value copy, so the host never holds a live pointer into encounter state.
struct CombatantView {
  std::string id;
  std::string name;
  int curHp = 0;
  int maxHp = 0;
  bool alive = false;
};

}  // namespace tumult

#endif  // TUMULT_CHARACTER_HPP_
