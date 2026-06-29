#ifndef TUMULT_EFFECTS_BLEED_HPP_
#define TUMULT_EFFECTS_BLEED_HPP_

#include <string>

#include "rpg/core/effect.hpp"
#include "tumult/export.hpp"

namespace tumult {

class TUMULT_API BleedEffect : public rpg::core::Effect {
 public:
  BleedEffect(std::string targetId, int damagePerStack, int stacks);
  rpg::core::Status onApply(rpg::core::Bus& bus) override;

  [[nodiscard]] const std::string& targetId() const { return targetId_; }
  [[nodiscard]] int damagePerStack() const { return damagePerStack_; }
  [[nodiscard]] int stacks() const { return stacks_; }

 private:
  std::string targetId_;
  int damagePerStack_;
  int stacks_;
};

}  // namespace tumult

#endif  // TUMULT_EFFECTS_BLEED_HPP_
