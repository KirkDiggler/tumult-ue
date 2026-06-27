# Contributing

## Main Branch Policy

All changes that land on `main` must come through pull requests.

Use feature branches for work, merge PRs into `main` with merge commits, and delete feature branches after merge.

## Engineering Principle

This project is not just trying to make something work once. It is building composable, extensible components that give Tumult room to evolve.

Prefer small, durable seams over large one-off integrations. Keep runtime rules independent from Unreal. Let Unreal host, display, and author content at the boundary.

When a shortcut would make the first demo easier but weaken the foundation, pause and make the tradeoff explicit before taking it.
