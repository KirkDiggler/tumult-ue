# Tumult UE Agent Notes

Kirk is not here for maximal velocity or ambitious scope. He is here to build understanding, reduce GUI friction, and evolve durable foundations for his game ideas.

Act accordingly:

- Keep goals small enough to finish and understand.
- Prefer day-sized or weekend-sized stepping stones over broad roadmaps.
- Do not inflate scope to make the work sound impressive.
- Build composable, extensible components rather than one-off demos.
- Make tradeoffs explicit when a shortcut would weaken the foundation.
- Treat Unreal and other GUI tools as places where the agent can reduce friction for Kirk.
- Move deliberately: explain the next useful step, then do it.

## Project Direction

Tumult UE is the Unreal host for Tumult. Keep the dependency direction clear:

```text
rpgkit -> tumult -> tumult-ue
```

Unreal hosts, displays, and authors content at the boundary. Tumult owns game rules. RPGKit remains engine-agnostic.

## Working Style

- Keep goals small and concrete.
- Prefer day/weekend stepping stones over ambitious milestone lists.
- Build durable seams before adding gameplay breadth.
- Do not treat a working Unreal demo as success if it creates a brittle foundation.
- Use feature branches and PRs for changes after the initial scaffold.
- Start sessions from project board 16, then pick the narrowest `Today` or `Weekend` issue that moves the foundation forward.
- Use `docs/development-loop.md` for the Windows/WSL split before touching Unreal-generated files.

## PR Review Acknowledgement

When Copilot or another reviewer comments on a PR, acknowledge the review deliberately.

- Reply to relevant review threads when addressing specific comments.
- Add a top-level PR comment summarizing the feedback, the agent's view, and what was changed or intentionally left unchanged.
- State the reasoning clearly so future us can understand the decision without guessing.
- Do not silently apply review feedback; make the tradeoff or agreement explicit.
