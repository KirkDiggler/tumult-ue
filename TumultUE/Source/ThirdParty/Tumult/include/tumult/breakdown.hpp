#ifndef TUMULT_BREAKDOWN_HPP_
#define TUMULT_BREAKDOWN_HPP_

#include <string>

namespace tumult {

// One step of a folded chain, mirrored (cheap struct copy) from
// rpg::core::Chain<DamageEvent>::Result::breakdown for host consumption.
// Mirrored — not wrapped — so the host renderer never takes a chain header.
struct BreakdownStep {
  std::string id;      // modifier id ("vulnerable-goblin"); "" on base/final
  std::string stage;   // "base" | "effects" | "final"
  std::string source;  // effect source ("vulnerable"); "" if uninitialized
  int before = 0;
  int after = 0;
};

// The simplification contract. Reads `source` from the receipt — never a
// host-side hardcoded string. Format is the unit a UE HUD and a terminal
// renderer both consume without re-deriving.
//
// Shape:
//   base/final with no modifier: "<stage>: <before> -> <after>"
//   effects with no source:      "<stage> / <id>: <before> -> <after>"
//   effects with source:        "<stage> / <id> (<source>): <before> -> <after>"
inline std::string formatStep(const BreakdownStep& s) {
  if (s.id.empty()) {
    return s.stage + ": " + std::to_string(s.before) + " -> " + std::to_string(s.after);
  }
  if (s.source.empty()) {
    return s.stage + " / " + s.id + ": " + std::to_string(s.before) + " -> " +
           std::to_string(s.after);
  }
  return s.stage + " / " + s.id + " (" + s.source + "): " + std::to_string(s.before) + " -> " +
         std::to_string(s.after);
}

}  // namespace tumult

#endif  // TUMULT_BREAKDOWN_HPP_
