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

}  // namespace tumult

#endif  // TUMULT_CHARACTER_HPP_
