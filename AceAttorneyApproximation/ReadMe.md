# AceAttorneyApproximation

DFS1 project — Capcom 逆转裁判 (Ace Attorney) 风 ADV with RPG framework hooks.

## 1 How to Use

### 1.1 Build and Run

Open `AceAttorneyApproximation\AceAttorneyApproximation.sln` in Visual Studio, then press `CTRL + SHIFT + B` to build the solution. `AceAttorneyApproximation_Release_x64.exe` will be under the `AceAttorneyApproximation\Run\` folder.

Build configurations: `Debug` / `DebugInline` / `FastBreak` / `Release` × `x64` / `Win32`.

### 1.2 Controls

- `~` opens the DevConsole.
- Mouse left-click to interact with NPCs, dialogue options, and inventory items.
- `Esc` opens the pause menu (or returns to attract screen).

## 2 Project Structure

- `Code/Game/` — Game-side code (App / Game / Entity / Player / Prop / GameCommon)
- `Run/Data/Shaders/` — HLSL shaders
- `Run/Data/Audio/Fonts/Images/` — assets

Engine is shared at `../Engine/`.

## 3 Status

Sprint 1 / Milestone 1 — foundation (Basic UI System + Linear Dialogue + Test Conversation).

Governance documents: `../../Documents/SMUCourses/HGME6226-DFS1/Governance/`.
