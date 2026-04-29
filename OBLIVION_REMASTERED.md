# NirnLabUIPlatform Oblivion Remastered Patch

This directory vendors the MIT-licensed upstream source from:

```text
https://github.com/kkEngine/NirnLabUIPlatform
```

Fork point:

```text
main, shallow clone taken for CyrodiilMP integration work on 2026-04-29
```

## Integration Goal

This fork is the general Oblivion Remastered host implementation for
NirnLabUIPlatform. CyrodiilMP is the first local consumer, but host detection,
runtime naming, and API surface describe the game/runtime host rather than the
CyrodiilMP mod.

Skyrim, SKSE, CommonLibSSE, and Address Library compatibility is no longer a
goal for this fork. Remaining upstream Skyrim code is porting scaffolding only.

CyrodiilMP game/runtime code should talk to an owned native UI boundary in the
launcher/bootstrap path; this vendor fork should sit behind that boundary as the
Chromium backend.

## Expected Porting Work

- Remove SKSE and CommonLibSSE entry points.
- Replace Skyrim native-menu assumptions with Oblivion Remastered standalone
  hook equivalents.
- Keep the reusable CEF pieces: browser lifetime, subprocess management,
  JavaScript callbacks, native-to-JS events, and CEF resource packaging.
- Replace Skyrim key-code and cursor handling with Oblivion Remastered input
  focus routing.
- Rename runtime output paths from `Data/NirnLabUIPlatform` to an Oblivion
  Remastered-compatible path under `Binaries/Win64`.

## Initial Source Triage

Likely reusable with minimal game-specific work:

- `src/CEFSubprocess`
- `src/UIPlatform/CEF`
- `src/UIPlatform/JS`
- `src/UIPlatform/Converters`
- `src/UIPlatform/Providers`
- `src/UIPlatform/Services/CEFService.*`

Skyrim/SKSE-specific and expected to be deleted or replaced:

- `src/UIPlatform/main.cpp`
- `src/UIPlugin`
- `src/UIPlatformTest`
- `src/UIPlatform/Menus`
- `src/UIPlatform/Services/UIPlatformService.*`
- `src/UIPlatform/Services/InputLangSwitchService.*`
- `src/UIPlatform/Converters/KeyInputConverter.*`
- code using `SKSE::`, `RE::`, `REL::`, `CommonLibSSE`, or Address Library

DirectX 11 rendering is present in the upstream CMake and render layer code.
Oblivion Remastered's actual renderer path still needs confirmation before we
choose between porting the existing D3D11 render layer, adapting it for another
swapchain path, or rendering through a UE/UE4SS surface.

## First Adapter Target

`UiRuntime` should eventually map to the public NirnLab-style API:

- `CreateView` -> `IUIPlatformAPI::AddOrGetBrowser`
- `HideView` / `ShowView` -> `IBrowser::SetBrowserVisible`
- `SendEvent` -> `IBrowser::ExecEventFunction`
- UI command registration -> `IBrowser::AddFunctionCallback`

Keep changes documented so we can still compare this fork with upstream when
useful, but do not preserve Skyrim behavior as a compatibility requirement.

## Host Detection Contract

This fork adds `src/UIPlatform/NirnLabUIPlatformAPI/Host.h`.

Consumers can detect which runtime provides the UI platform by either:

- calling the optional DLL export `GetUIPlatformHostInfo()` through
  `DllLoader::GetUIPlatformHostInfo()`
- dispatching `APIMessageType::RequestHostInfo` and reading
  `ResponseHostInfoMessage::hostInfo`

Known host value:

- `NL::UI::HostRuntime::OblivionRemastered`

This fork defines `NL_UI_HOST_OBLIVION_REMASTERED` for the UI platform target.
`Host.h` also defaults to Oblivion Remastered when consumed directly.

Example:

```cpp
auto host = NL::UI::DllLoader::GetUIPlatformHostInfo();
if (host.runtime == NL::UI::HostRuntime::OblivionRemastered)
{
    // Use Oblivion Remastered-specific setup.
}
```
