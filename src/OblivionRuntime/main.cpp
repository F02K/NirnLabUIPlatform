#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "NirnLabUIPlatformAPI/API.h"

#define DLLEXPORT __declspec(dllexport)

namespace {

void DebugLog(const char* message)
{
    OutputDebugStringA("[NirnLabUIPlatformOR] ");
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
}

struct StoredCallback
{
    std::string object_name;
    std::string function_name;
    NL::JS::JSFuncCallbackData callback_data;
};

class StubBrowser final : public NL::CEF::IBrowser
{
public:
    bool __cdecl IsBrowserReady() override { return true; }
    bool __cdecl IsPageLoaded() override { return true; }

    void __cdecl SetBrowserVisible(bool value) override { visible_ = value; }
    bool __cdecl IsBrowserVisible() override { return visible_; }
    void __cdecl ToggleBrowserVisibleByKeys(std::uint32_t, std::uint32_t) override {}

    void __cdecl SetBrowserFocused(bool value) override { focused_ = value; }
    bool __cdecl IsBrowserFocused() override { return focused_; }
    void __cdecl ToggleBrowserFocusByKeys(std::uint32_t, std::uint32_t) override {}

    void __cdecl LoadBrowserURL(const char* url, bool clear_callbacks = true) override
    {
        if (clear_callbacks)
        {
            callbacks_.clear();
        }

        url_ = url == nullptr ? "" : url;
        DebugLog(("Stub browser loaded URL: " + url_).c_str());
    }

    void __cdecl ExecuteJavaScript(const char*, const char* = JS_EXECUTE_SCRIPT_URL) override {}

    void __cdecl AddFunctionCallback(const NL::JS::JSFuncInfo& callback_info) override
    {
        callbacks_.push_back(StoredCallback{
            callback_info.objectName == nullptr ? "" : callback_info.objectName,
            callback_info.funcName == nullptr ? "" : callback_info.funcName,
            callback_info.callbackData
        });
    }

    void __cdecl RemoveFunctionCallback(const char* object_name, const char* func_name) override
    {
        std::erase_if(callbacks_, [&](const StoredCallback& callback) {
            return callback.object_name == (object_name == nullptr ? "" : object_name) &&
                   callback.function_name == (func_name == nullptr ? "" : func_name);
        });
    }

    void __cdecl RemoveFunctionCallback(const NL::JS::JSFuncInfo& callback_info) override
    {
        RemoveFunctionCallback(callback_info.objectName, callback_info.funcName);
    }

    void __cdecl ExecEventFunction(const char*, const char*) override {}

private:
    bool visible_ = false;
    bool focused_ = false;
    std::string url_;
    std::vector<StoredCallback> callbacks_;
};

class StubUIPlatformAPI final : public NL::UI::IUIPlatformAPI
{
public:
    BrowserRefHandle __cdecl AddOrGetBrowser(const char* browser_name,
                                             NL::JS::JSFuncInfo* const* callbacks,
                                             std::uint32_t callback_count,
                                             const char* start_url,
                                             NL::CEF::IBrowser*& out_browser) override
    {
        return AddOrGetBrowser(browser_name, callbacks, callback_count, start_url, nullptr, out_browser);
    }

    void __cdecl ReleaseBrowserHandle(BrowserRefHandle) override {}

    BrowserRefHandle __cdecl AddOrGetBrowser(const char*,
                                             NL::JS::JSFuncInfo* const* callbacks,
                                             std::uint32_t callback_count,
                                             const char* start_url,
                                             NL::UI::BrowserSettings*,
                                             NL::CEF::IBrowser*& out_browser) override
    {
        browser_.LoadBrowserURL(start_url, true);
        for (std::uint32_t index = 0; index < callback_count; ++index)
        {
            if (callbacks != nullptr && callbacks[index] != nullptr)
            {
                browser_.AddFunctionCallback(*callbacks[index]);
            }
        }

        out_browser = &browser_;
        DebugLog("Created Oblivion-safe stub browser");
        return 1;
    }

    void RegisterOnShutdown(OnShutdownFunc_t callback) override
    {
        shutdown_callbacks_.push_back(callback);
    }

    void Shutdown()
    {
        for (auto* callback : shutdown_callbacks_)
        {
            if (callback != nullptr)
            {
                callback();
            }
        }

        shutdown_callbacks_.clear();
    }

private:
    StubBrowser browser_;
    std::vector<OnShutdownFunc_t> shutdown_callbacks_;
};

StubUIPlatformAPI g_api;

}

extern "C"
{
    DLLEXPORT NL::UI::ResponseVersionMessage GetUIPlatformAPIVersion()
    {
        return {};
    }

    DLLEXPORT NL::UI::HostInfo GetUIPlatformHostInfo()
    {
        return NL::UI::GetCompileTimeHostInfo();
    }

    DLLEXPORT bool CreateOrGetUIPlatformAPI(NL::UI::IUIPlatformAPI** out_api, NL::UI::Settings*)
    {
        if (out_api == nullptr)
        {
            return false;
        }

        *out_api = &g_api;
        DebugLog("Returned Oblivion-safe stub API");
        return true;
    }

    DLLEXPORT bool CreateOrGetUIPlatformAPIWithVersionCheck(NL::UI::IUIPlatformAPI** out_api,
                                                            NL::UI::Settings* settings,
                                                            std::uint32_t,
                                                            const char*)
    {
        return CreateOrGetUIPlatformAPI(out_api, settings);
    }
}

BOOL APIENTRY DllMain(HMODULE, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DebugLog("Loaded Oblivion-safe runtime stub");
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        g_api.Shutdown();
    }

    return TRUE;
}
