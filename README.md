# NirnLabUIPlatform

This fork is the Oblivion Remastered port of NirnLabUIPlatform.

Original project credit goes to the NirnLabUIPlatform author and upstream
maintainer at `kkEngine/NirnLabUIPlatform`. This fork keeps that MIT-licensed
work as its base and exists only to maintain Oblivion Remastered compatibility.

Upstream supports Skyrim/SKSE and was built around CEF (Chromium Embedded
Framework). This fork does not support Skyrim as the original code does:
Oblivion Remastered is the only supported runtime target. Skyrim, SKSE,
CommonLibSSE, and Address Library compatibility is not a goal for new work.

## Direction

- Keep the reusable Chromium/CEF browser stack.
- Replace Skyrim menu, input, path, logging, and plugin entry points with
  Oblivion Remastered host integration.
- Expose a stable C/C++ UI API that CyrodiilMP and other Oblivion Remastered
  mods can use without depending on CyrodiilMP-specific names.
- Treat remaining Skyrim/SKSE files as upstream legacy code to port away from,
  not as supported compatibility code.

## Current State

This fork already declares its host as Oblivion Remastered through
`NirnLabUIPlatformAPI/Host.h`.

The codebase still contains upstream Skyrim/SKSE implementation files. Those are
temporary scaffolding while the CEF pieces are separated from the old host layer.
Do not add new features to the SKSE path.

## Public API

The public headers live in:

```text
src/UIPlatform/NirnLabUIPlatformAPI
```

Important entry points:

- `API.h` - browser creation and UI platform API contracts.
- `DllLoader.h` - direct DLL loading helpers.
- `Host.h` - Oblivion Remastered host identity.
- `IBrowser.h` - browser visibility, focus, JavaScript callback, and event APIs.

## Browser Basics

```cpp
NL::CEF::IBrowser* browser = nullptr;
auto handle = api->AddOrGetBrowser(
    "MyOblivionView",
    nullptr,
    0,
    "https://example.invalid",
    &browserSettings,
    browser);

if (handle != NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle)
{
    browser->SetBrowserVisible(true);
    browser->SetBrowserFocused(true);
    browser->ExecuteJavaScript("window.fromNative = true");
}
```

JavaScript callbacks and native-to-JS events still use the upstream callback
model:

```cpp
auto eventFunc = new NL::JS::JSFuncInfo();
eventFunc->objectName = "NL";
eventFunc->funcName = "addEventListener";
eventFunc->callbackData.isEventFunction = true;

browser->ExecEventFunction("on:message", "hello from native");
```

## Build Notes

The build files still contain temporary upstream dependencies while the port is
in progress. Expect these to be removed as the Oblivion Remastered host layer is
implemented.

Current useful dependencies:

- CMake 3.23+
- Vcpkg
- Microsoft Visual Studio 2022 C++ desktop tooling
- Chromium Embedded Framework (`cef-prebuilt`)
- DirectXTK
- spdlog
- nlohmann-json

Temporary legacy dependencies:

- CommonLibSSE-NG
- SKSE-shaped plugin/test entry points

## License

MIT. Preserve upstream credit and license text when carrying code forward.
Original NirnLabUIPlatform code is credited to the upstream author/maintainer at
`kkEngine/NirnLabUIPlatform`; Oblivion Remastered compatibility changes are
maintained in this fork.
