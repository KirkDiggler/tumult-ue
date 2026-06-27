# Windows And WSL Development Loop

Tumult UE lives on the Windows filesystem so Unreal and Visual Studio can own
the editor/build experience while WSL/OpenCode can own small source and docs
changes.

## Start Of Session

- Check project board 16 before choosing work.
- Prefer the `Today` or `Weekend` item with the narrowest done state.
- Start from clean `main`, then create an issue-named feature branch.
- Keep one PR to one board or issue-sized change.

## Windows Owns

- Opening `TumultUE/TumultUE.uproject` in Unreal Editor.
- Regenerating solution/project files when Unreal needs them.
- Building with the normal Unreal/Visual Studio toolchain.
- Running Play In Editor and checking editor-only behavior.

## WSL/OpenCode Owns

- Editing tracked C++, C#, config, docs, and git metadata.
- Inspecting diffs, status, recent commits, issues, PRs, and project board state.
- Keeping Unreal-facing code at the host boundary.
- Recording friction discovered in Windows as docs or follow-up issues.

## Do Not Commit

Generated and machine-local files should stay out of git. In normal work, avoid
staging these paths even when Unreal touches them:

- `TumultUE/Binaries/`
- `TumultUE/Build/`
- `TumultUE/DerivedDataCache/`
- `TumultUE/Intermediate/`
- `TumultUE/Saved/`
- `TumultUE/.vs/`
- generated solution files such as `*.sln` and `*.slnx`
- debug/build outputs such as `*.dll`, `*.pdb`, `*.obj`, and `*.lib`

Keep source assets, `Config/`, `Content/`, `Plugins/`, `Source/`, and
`*.uproject` tracked when they represent intentional project state.

## Handoff Pattern

When Windows verification is needed, make the WSL change easy to inspect first:

- Leave the working tree clean except for the intended files.
- Summarize exactly what needs to be opened, built, or played in Unreal.
- If Windows reveals friction, document it in the PR or open a follow-up issue
  instead of widening the current task.
