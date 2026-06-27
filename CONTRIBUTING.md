# Contributing

## Main Branch Policy

All changes that land on `main` must come through pull requests.

Use feature branches for work, merge PRs into `main` with merge commits, and delete feature branches after merge.

## Review Workflow

Copilot review comments are part of the deliberate decision trail.

When addressing review feedback:

- Reply to specific threads when a comment is addressed directly.
- Leave a top-level PR comment that summarizes the feedback, the project view, and the action taken.
- Be explicit when accepting, rejecting, or deferring a suggestion.
- Prefer visible reasoning over silent changes.

Before merging a PR:

- Confirm every Copilot or reviewer comment has been acknowledged.
- Do not merge with unresolved silent feedback; either address it, defer it with a follow-up, or explain why no change is being made.

## Engineering Principle

This project is not just trying to make something work once. It is building composable, extensible components that give Tumult room to evolve.

Prefer small, durable seams over large one-off integrations. Keep runtime rules independent from Unreal. Let Unreal host, display, and author content at the boundary.

When a shortcut would make the first demo easier but weaken the foundation, pause and make the tradeoff explicit before taking it.
