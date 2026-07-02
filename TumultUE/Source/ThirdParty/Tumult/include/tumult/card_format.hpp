#ifndef TUMULT_CARD_FORMAT_HPP_
#define TUMULT_CARD_FORMAT_HPP_

#include <string>

#include "tumult/encounter.hpp"

namespace tumult {

// The card-altitude simplification contract — a sibling to formatStep, one
// altitude up. Renders a one-line header that names the card FROM ITS RECEIPT
// (receipt.type / receipt.id), never a host-side hardcoded label (the formatStep
// discipline). The per-modifier lines stay formatStep over
// result.strike.breakdown; this renders ONLY the card header so the two
// formatters compose without overlap.
//
// Shape:
//   "card <type> (<id>): <finalDamage> dmg[, <blocked> blocked], hp <hpAfter>"
inline std::string formatCardPlay(const CardResult& result) {
  std::string header = "card " + result.receipt.type + " (" + result.receipt.id +
                       "): " + std::to_string(result.strike.finalDamage) + " dmg";
  if (result.strike.blocked > 0) {
    header += ", " + std::to_string(result.strike.blocked) + " blocked";
  }
  header += ", hp " + std::to_string(result.strike.hpAfter);
  return header;
}

}  // namespace tumult

#endif  // TUMULT_CARD_FORMAT_HPP_
