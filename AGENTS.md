# Repository Guidelines

## Project Structure & Module Organization

This repository contains a Quest standalone Beat Saber mod for Quest 2/3/Pro.
Quest 1 and PC/BSIPA builds are not supported by this target.

- `qpm.json` defines the QPM package, scripts, and dependency ranges.
- `mod.template.json` defines the `.qmod` metadata.
- `CMakeLists.txt` defines the native C++ shared library build.
- `include/BeatSinger/` contains public mod headers for config, lyrics parsing,
  providers, and the Unity controller custom type.
- `src/` contains the native implementation and gameplay hooks.
- Local lyrics are read from custom song folders as `lyrics.json` or `lyrics.srt`.
- There is currently no automated test project.

## Build, Test, and Development Commands

- `qpm restore` restores Quest dependencies.
- `qpm s build` builds the native mod library.
- `qpm s qmod` packages the `.qmod`.
- `qpm s copy` copies the `.qmod` to a connected headset.
- `qpm s logcat -self` tails logs for this mod.

The target Beat Saber Quest version is `1.40.8_7379` with the Scotland2
modloader. Runtime dependencies should match the Quest mod list for that game
version, especially `SongCore 1.1.26`, `BSML 0.4.55`, and `custom-types 0.18.3`.
Keep `bs-cordl` on `4008.0.0` for this game version. Newer `bs-cordl`,
`beatsaber-hook`, BSML, SongCore, or custom-types packages may exist on QPM,
but do not upgrade to a dependency set for a different Beat Saber version unless
the project target is intentionally moved.
Do not add `config-utils` unless the QPM dependency graph has a version
compatible with `custom-types 0.18.3`; config is currently persisted as local
JSON under Quest mod storage.

## Coding Style & Naming Conventions

Use the style already present in the native codebase. C++ files use 4-space
indentation, namespaces for mod code, `PascalCase` for public types and Unity
custom type methods, and `camelCase` for locals and private fields. Keep
Beat Saber hook logic isolated in `src/main.cpp`, and keep parsing/provider
logic independent of Unity APIs when practical.

Prefer structured parsing for lyrics formats. Keep local file lookups and HTTP
lookups behind `LyricsProvider` implementations.

## Testing Guidelines

There is no automated test suite yet. For mod changes, verify with:

```sh
qpm restore
qpm s build
qpm s qmod
```

Then install the generated `.qmod` through ModsBeforeFriday or QuestPatcher on
Beat Saber Quest `1.40.8_7379`.

Manual smoke checks:

- Game launches with BeatSinger enabled.
- A custom song with `lyrics.json` displays timed lyrics.
- A custom song with `lyrics.srt` displays timed lyrics.
- Missing lyrics logs a clear message and does not crash.
- Toggle/config changes persist after restart.
- HTTP provider disabled by default makes no network call.
- HTTP provider enabled fetches JSON and SRT from a test endpoint, handles
  404/timeout safely, and falls back to local/no lyrics.

## Commit & Pull Request Guidelines

Recent commits use short imperative or past-tense summaries, sometimes with
issue references, for example `Add LICENSE (fixes #18).` Keep subjects concise
and focused on one change.

Pull requests should include a brief description, manual test results, linked
issues when applicable, and screenshots or clips for visible in-game changes.
Call out Beat Saber Quest version assumptions and required configuration.

## Security & Configuration Tips

Do not commit real HTTP provider tokens, private endpoints, local headset paths,
or generated build/package artifacts. Treat downloaded and uploaded lyrics as
untrusted input. Keep HTTP lookup disabled by default and avoid hardcoded
credential-backed lyric services.
