#ifndef TUMULT_EFFECTS_VULNERABLE_HPP_
#define TUMULT_EFFECTS_VULNERABLE_HPP_

#include <string>

#include "rpg/core/effect.hpp"

namespace tumult {

class TUMULT_API VulnerableEffect : public rpg::core::Effect {
 public:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  VulnerableEffect(std::string targetId, int percentBonus, int remainingTurns);
  rpg::core::Status onApply(rpg::core::Bus& bus) override;

  // Host-facing getters (Blueprint/terminal reflection in Slice 2/7).
  [[nodiscard]] const std::string& targetId() const { return targetId_; }
  [[nodiscard]] int percentBonus() const { return percentBonus_; }
  [[nodiscard]] int remainingTurns() const { return remainingTurns_; }

 private:
  std::string targetId_;
  int percentBonus_;
  int remainingTurns_;
};

}  // namespace tumult

#endif  // TUMULT_EFFECTS_VULNERABLE_HPP_
