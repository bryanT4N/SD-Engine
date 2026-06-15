# SD-Engine

A custom C++ game engine (DirectX 11) and two games built on top of it, developed at the SMU Guildhall. The engine is a static library shared by every project; each game is a thin client that links against it.

> ⚠️ **Build note — FMOD not included.** The audio subsystem links against [FMOD](https://www.fmod.com/), which is proprietary middleware and is **not** redistributed here. To build, install the FMOD Engine SDK into `Engine/Code/ThirdParty/fmod/` (see that folder's README). Everything else builds out of the box.

<!-- TODO: add a real gameplay screenshot here, e.g. ![NetChess3D](docs/netchess3d.png) -->

## Repository layout

```
Engine/                        Custom C++ engine (static lib) — shared by all clients
  Code/Engine/
    Renderer/   DirectX 11 renderer: shaders, constant buffers, textures, PCUTBN vertices, normal mapping
    Math/       Vec/Mat/AABB/OBB, easing curves, RNG (unit-tested)
    Core/       Event system, dev console, XML config, image loading (stb), strings, clock
    Input/      Keyboard / mouse / Xbox controller
    Audio/      FMOD wrapper
    Window/     Win32 window + message pump
    UI/         Retained-mode widget framework (containers, anchors, layout, theming)
    Narrative/  XML-driven dialogue system
  Code/ThirdParty/   stb_image, tinyxml2  (FMOD installed locally, not committed)

NetChess3D/                    3D chess — full rules engine + tangent-space normal mapping
AceAttorneyApproximation/      Ace Attorney-style ADV — dialogue + UI widget framework
```

## The games

### NetChess3D — 3D chess with normal-mapped lighting
A playable 3D chess game driven from the dev console. Two halves:
- **Rules engine** — every move is validated through one `ChessMatch::TryExecuteMove` "security-checkpoint" pipeline: per-piece geometry, sliding-path blocking, kings-apart, pawn promotion, **en passant**, and **castling** (dispatched before geometry so the king's two-square move isn't rejected). `teleport`/`ChessOverride` dev cheats for setup.
- **Graphics** — tangent-space normal mapping: model-space TBN transformed to world space, Gram-Schmidt orthonormalized, and a reverse-multiply against the `float3x3(T,B,N)` matrix to compensate for HLSL's row-major convention. 1–19 debug visualization modes (UV, model/world TBN, decoded normals, light strength, …).

Interesting code: [`NetChess3D/Code/Game/ChessMatch.cpp`](NetChess3D/Code/Game/ChessMatch.cpp) (rules) · [`NetChess3D/Run/Data/Shaders/BlinnPhong.hlsl`](NetChess3D/Run/Data/Shaders/BlinnPhong.hlsl) (normal mapping).

### AceAttorneyApproximation — courtroom-ADV framework (Directed Focus Study)
An Ace Attorney-inspired adventure framework: an **XML-driven dialogue system**, a reusable **UI widget framework** (anchor/stack containers, capture-based input, theming), and present-as-evidence interaction.

## Building

- **Visual Studio 2022**, platform **x64**, configurations Debug / Release.
- Install the **FMOD Engine SDK** into `Engine/Code/ThirdParty/fmod/` (headers + `*_vc.lib`), and drop `fmod.dll` / `fmod64.dll` next to each game's `Run/` executable.
- Open `NetChess3D/NetChess3D.sln` or `AceAttorneyApproximation/AceAttorneyApproximation.sln` and build; the Engine project builds first as a static lib.
- Run from each project's `Run/` directory (it contains the `Data/` assets).

## License / use

See [`NOTICE.md`](NOTICE.md). This is SMU Guildhall coursework, published for portfolio and evaluation purposes only.
