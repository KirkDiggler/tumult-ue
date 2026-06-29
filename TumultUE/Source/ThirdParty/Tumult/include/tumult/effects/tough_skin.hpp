#ifndef TUMULT_EFFECTS_TOUGH_SKIN_HPP_
#define TUMULT_EFFECTS_TOUGH_SKIN_HPP_

#include <string>

#include "rpg/core/effect.hpp"
#include "tumult/export.hpp"

namespace tumult {

class TUMULT_API ToughSkinEffect : public rpg::core::Effect {
 public:
  ToughSkinEffect(std::string protectedId, int reduction);
  rpg::core::Status onApply(rpg::core::Bus& bus) override;

  [[nodiscard]] const std::string& protectedId() const { return protectedId_; }
  [[nodiscard]] int reduction() const { return reduction_; }

 private:
  std::string protectedId_;
  int reduction_;
};

}  // namespace tumult

#endif  // TUMULT_EFFECTS_TOUGH_SKIN_HPP_
