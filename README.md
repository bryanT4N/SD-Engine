# SD-Engine

A custom C++ game engine, plus two games built with it. Made at SMU Guildhall.

The engine is a static library. Each game links to it.

![NetChess3D](docs/netchess3d.png)

*NetChess3D — a world-space normal debug view.*

> **Build note:** FMOD is not included. It is paid middleware, so it is kept out of this repo. To build, put the FMOD SDK in `Engine/Code/ThirdParty/fmod/` (see the README there).

## Layout

```
Engine/                      The engine (static lib). Shared by both games.
NetChess3D/                  3D chess.
AceAttorneyApproximation/    Ace Attorney style adventure game (Directed Focus Study).
```

## The games

### NetChess3D
3D chess, played from the dev console. Full move rules, including en passant and castling. A normal-mapped renderer with many debug views (UV, tangents, normals, light).

### AceAttorneyApproximation
An Ace Attorney style adventure game. It has a dialogue system driven by XML, a UI widget framework, and present-evidence interaction.

## Engine parts

- **Renderer** — DirectX 11. Shaders, textures, normal mapping.
- **Math** — vectors, matrices, easing curves. Unit tested.
- **Core** — events, dev console, XML config, image loading.
- **Input** — keyboard, mouse, Xbox controller.
- **Audio** — FMOD wrapper.
- **Window** — Win32 window.
- **UI** — widget framework.
- **Narrative** — XML dialogue.

## Build

- Visual Studio 2022, x64.
- Install the FMOD SDK into `Engine/Code/ThirdParty/fmod/`.
- Open a game's `.sln` and build. The engine builds first.
- Run from the game's `Run/` folder.

## Use

See [`NOTICE.md`](NOTICE.md). This is SMU Guildhall coursework, shared for portfolio and review only. Not for sale.
