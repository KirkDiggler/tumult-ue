# First Card Interaction Surface

Issue: `tumult-ue#12`

## Decision

Use **debug keyboard input with on-screen debug text** as the first Unreal card
interaction surface.

The first playable card slice should let a player choose from a visible numbered
list and press a key to select or play one card. Presentation can remain plain:
`GEngine->AddOnScreenDebugMessage`, UE log output, and a small C++ input handler
are enough.

Do not build a UMG card widget, 3D card actor, editor authoring asset, or final
HUD in this slice.

## Why This Fits Now

- Tumult owns the rules contract, and `tumult#9` is the next card/play contract
  step. TumultUE should not invent card data or play semantics ahead of that.
- Debug input keeps the first interaction slice focused on the host boundary:
  can Unreal pass a selected card identity toward Tumult and show the resulting
  state?
- On-screen debug text matches the existing encounter-state proof and requires
  no Blueprint, widget hierarchy, asset authoring, layout, or input-mode work.
- The interaction can be replaced later without changing Tumult's game rules or
  the vendored module boundary.

## Options Compared

| Option | Strength | Cost / Risk | Decision |
|---|---|---|---|
| Debug keyboard input + on-screen text | Fastest way to prove selection and card-play routing | Not a real UI; temporary presentation | Choose first |
| Simple UMG widget | Closer to player-facing cards; useful soon | Adds widget lifecycle, input focus, layout, and binding choices before the card contract lands | Defer |
| 3D card actor | Good for future tactile/board-like presentation | Adds actor spawning, hit testing, camera/pawn assumptions, materials, and more Unreal-specific structure | Defer |

## First Slice Shape

The first implementation after this decision should be small:

1. Display a small numbered list of Tumult-backed card summaries once Tumult
   exposes the card contract.
2. Let the player press a key such as `1`, `2`, or `3` to select one entry.
3. Show the selected card identity clearly in on-screen debug text or the UE log.
4. Do not resolve gameplay in the selection-only slice.

The later play slice can use the same temporary surface to send the selected card
identity through Tumult and refresh the encounter read-model.

## Revisit Trigger

Move from debug input to UMG when at least one of these is true:

- Tumult's card contract is stable enough that card names, costs, and effect
  summaries are known.
- The next slice needs mouse/touch selection instead of numeric keys.
- Debug text starts hiding useful rule feedback or slowing verification.

Move to 3D card actors only after the rules loop is stable and the project needs
spatial card presentation.
