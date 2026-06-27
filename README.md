# Tumult UE

Tumult UE is the Unreal host for the Tumult game rules.

The intended dependency direction is:

```text
rpgkit -> tumult -> tumult-ue
```

`rpgkit` is the reusable rules toolkit. `tumult` is the game rules/domain library. `tumult-ue` is the Unreal project that makes Tumult playable and visible in Unreal.

## Local Layout

This repository is expected to live beside the core repositories on the Windows filesystem:

```text
C:\Users\kirk\personal\
  rpgkit\
  tumult\
  tumult-ue\
```

From WSL, that is:

```text
/mnt/c/Users/kirk/personal/
  rpgkit/
  tumult/
  tumult-ue/
```

## Unreal Project Creation

Create the Unreal project from Unreal Editor or the Epic launcher into this directory.

Recommended first project shape:

```text
Template: Blank
Project type: C++
Target: Desktop
Starter Content: off unless needed
Project name: TumultUE
Location: C:\Users\kirk\personal\tumult-ue
```

After creation, the repo should contain `TumultUE.uproject`, `Source/`, `Config/`, and any initial Unreal content.

If Unreal creates a project subdirectory, the expected shape is:

```text
tumult-ue/
  TumultUE/
    TumultUE.uproject
    Config/
    Content/
    Source/
```

## First Goal

Open `TumultUE` in Unreal, press Play, and show that Unreal can call Tumult rules code.

The first visible milestone should be intentionally small:

```text
BeginPlay -> initialize tiny Tumult encounter -> print state to UE log/on-screen debug
```

No editor tooling, card authoring, or full HUD is required for the first milestone.

## Development Flow

Changes land on `main` through pull requests with merge commits. Feature branches are deleted after merge.

See `CONTRIBUTING.md` for the project engineering principle.
