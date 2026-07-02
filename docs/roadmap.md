# Tumult UE Roadmap

## Overall Goal

Make Tumult playable and inspectable in Unreal while keeping the game rules outside Unreal.

The architecture should keep dependencies flowing in one direction:

```text
rpgkit -> tumult -> tumult-ue
```

Unreal should host, display, and author content. Tumult should own game rules. RPGKit should stay reusable and engine-agnostic.

## Stepping Stones

### 1. Empty Unreal Project Opens

Create a clean Unreal 5.8 C++ project in this repository.

Done means:

- `TumultUE.uproject` opens from Windows.
- The default C++ module builds.
- The project can enter Play In Editor.

### 2. Unreal Can Call Tumult Code

Wire the Unreal module to compile against the local `tumult` repository.

Done means:

- Unreal C++ includes a Tumult header.
- `BeginPlay` calls a tiny Tumult function or constructs a minimal rules object.
- The result appears in the UE log or on-screen debug text.

### 3. Display A Minimal Encounter

Create one hardcoded encounter through Tumult and display its state.

Done means:

- Player and enemy state are visible.
- At least HP and one actionable value are shown.
- The state comes from Tumult, not duplicated Unreal-only data.

### 4. Execute One Action

Trigger one player action from Unreal and route it through Tumult.

The first card interaction surface is debug keyboard input plus on-screen debug
text. See `docs/card-interaction-surface.md`.

Done means:

- A button, key press, or debug command executes an action.
- Encounter state changes.
- The change is visible in Unreal.

### 5. Show Receipts And Observations

Surface the rules explanation from Tumult/RPGKit in Unreal.

Done means:

- Damage/effect/action output includes structured receipt text.
- The player can see why a value changed.
- The display uses core observation data instead of hardcoded strings where practical.

### 6. Replace Hardcoded Data With Unreal Content

Move selected Tumult content into Unreal-authored assets only after the runtime loop is proven.

Done means:

- A designer-facing asset can define at least one card/action/effect.
- The asset is converted into Tumult runtime data at the boundary.
- Tumult remains independent from Unreal types.

## MCP Role

Unreal MCP should be treated as an editor automation bridge.

It can help inspect editor state, run commands, trigger Play In Editor, examine logs, and eventually create or inspect assets. It should not become part of the runtime architecture.
