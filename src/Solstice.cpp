//
// Created by vastrakai on 6/24/2024.
//
//Test commit
#include "Solstice.hpp"


#include <fstream>
#include <Features/FeatureManager.hpp>
#include <Features/Configs/ConfigManager.hpp>
#include <Hook/HookManager.hpp>
#include <Hook/Hooks/RenderHooks/D3DHook.hpp>

#include <SDK/OffsetProvider.hpp>

#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftGame.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>

#include <Features/Auth/Authorization.hpp>

#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <winrt/base.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Core.h>
#include <build_info.h>
#include <Features/IRC/IrcClient.hpp>
#include <Features/Modules/Misc/IRC.hpp>
#include <SDK/Minecraft/Rendering/GuiData.hpp>
#include <Utils/OAuthUtils.hpp>
#include <Utils/SysUtils/xorstr.hpp>

#include <wininet.h>
#pragma comment(lib, "wininet.lib")


#ifdef __DEBUG__
std::string title = "[1.26.45] [" + std::string(SOLSTICE_BUILD_VERSION_SHORT) + "-" + std::string(SOLSTICE_BUILD_BRANCH) + "] [debug]";
#elif __PRIVATE_BUILD__
std::string title = "[1.26.45] [" + std::string(SOLSTICE_BUILD_VERSION_SHORT) + "-" + std::string(SOLSTICE_BUILD_BRANCH) + "] [private]";
#else
std::string title = "[1.26.45] [" + std::string(SOLSTICE_BUILD_VERSION_SHORT) + "-" + std::string(SOLSTICE_BUILD_BRANCH) + "]";
#endif


void setTitle(std::string title)
{
    if (!title.empty())
    {
        title += " - ";
    }

    title += "Minecraft";

    int s = MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, nullptr, 0);
    std::wstring wtitle(s, 0);
    MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, wtitle.data(), s);

    SetWindowTextW(Solstice::GameWindow, wtitle.c_str());
    Solstice::mReadyWindow = true;
    /*
    auto w = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [title]() {
        winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().Title(winrt::to_hstring(title));
    });*/
}

std::vector<unsigned char> gBpBytes = {0x1c}; // Defines the new offset for mInHandSlot
DEFINE_PATCH_FUNC(patchInHandSlot, SigManager::ItemInHandRenderer_renderItem_bytepatch2+2, gBpBytes);

// called using winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [&]()
void Solstice::init(HMODULE hModule)
{
    Solstice::GameWindow = FindWindowA(NULL, "Minecraft"); 

    // Not doing this could cause crashes if you inject too soon
    // Honestly, I don't think this helps much but it's not a bad idea to have it here
    while (ProcUtils::getModuleCount() < 130) std::this_thread::sleep_for(std::chrono::milliseconds(1));

    int64_t start = NOW;

    mModule = hModule;
    mInitialized = true;

#ifdef __DEBUG__
    Logger::initialize();
#endif

    console = spdlog::stdout_color_mt(CC(21, 207, 148) + "solstice" + ANSI_COLOR_RESET, spdlog::color_mode::automatic);

    // Create a file logger sink
    std::string logFile = FileUtils::getSolsticeDir() + xorstr_("solstice.log");

    // Don't use the file sink if the log file doesn't exist
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[" + CC(255, 135, 0) + "%H:%M:%S.%e" + ANSI_COLOR_RESET + "] [%n] [%^%l%$] %v");
    console_sink->set_level(spdlog::level::trace);
    console->set_level(spdlog::level::trace);
    console->set_pattern("[" + CC(255, 135, 0) + "%H:%M:%S.%e" + ANSI_COLOR_RESET + "] [%n] [%^%l%$] %v");
    spdlog::set_default_logger(std::make_shared<spdlog::logger>(CC(21, 207, 148) + "solstice" + ANSI_COLOR_RESET, spdlog::sinks_init_list{console_sink}));

    console->info("Welcome to " + CC(0, 255, 0) + "Solstice" + ANSI_COLOR_RESET + "!"
#ifdef __DEBUG__
        + CC(255, 0, 0) + " [Debug] " + ANSI_COLOR_RESET
#endif
);

    spdlog::info("Target version: 1.26.45 | Detected Minecraft version: {}", ProcUtils::getVersion());

    ExceptionHandler::init();

    FileUtils::validateDirectories();

    // Change the window title


    setTitle(title);

    sHWID = GET_HWID().toString();
    spdlog::info("HWID: {}", sHWID);

    if (MH_Initialize() != MH_OK)
    {
        console->critical("Failed to initialize MinHook!");
    }

    Prefs = PreferenceManager::load();

    HWND hwnd = ProcUtils::getMinecraftWindow(); // Cache the window handle

#ifdef __DEBUG__
    if (Prefs->mEnforceDebugging)
    {
        while (!IsDebuggerPresent())
        {
            MessageBoxA(nullptr, "Please attach a debugger to continue.\nThis message is being shown because of your preference settings.", "Solstice", MB_OK | MB_ICONERROR);
            spdlog::info("Waiting for debugger...");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
#endif

    /* // fard
    if (!InternetCheckConnectionA(xorstr_("https://www.timeapi.io/api/Time/current/zone?timeZone=UTC"), FLAG_ICC_FORCE_CONNECTION, 0)) {
        __fastfail(0);
    }*/

    console->info("initializing signatures...");
    int64_t sstart = NOW;
    OffsetProvider::initialize();
    SigManager::initialize();
    int64_t send = NOW;

    int failedSigs = 0;

    // Go through all signatures and print any that failed
    for (const auto& [sig, result] : SigManager::mSigs)
    {
        if (result == 0)
        {
            console->critical("[signatures] Failed to find signature: {}", sig);
            failedSigs++;
        }
    }

    for (const auto& [sig, result] : OffsetProvider::mSigs)
    {
        if (result == 0)
        {
            console->critical("[offsets] Failed to find offset: {}", sig);
            failedSigs++;
        }
    }

    if (failedSigs > 0)
    {
        std::string failedList;
        for (const auto& [sig, result] : SigManager::mSigs)
        {
            if (result == 0) failedList += " - [Sig] " + sig + "\n";
        }
        for (const auto& [sig, result] : OffsetProvider::mSigs)
        {
            if (result == 0) failedList += " - [Offset] " + sig + "\n";
        }

        std::string report = "Failed to resolve " + std::to_string(failedSigs) + " signatures/offsets for Minecraft 1.26.45:\n" + failedList;
        console->critical("{}", report);
        ExceptionHandler::makeCrashLog(report, 0xDEAD0001);

        std::string userMsg = "Solstice Initialization Failed!\n\n"
                              "The following " + std::to_string(failedSigs) + " signature(s)/offset(s) could not be found for Minecraft 1.26.45:\n"
                              + failedList + "\nSolstice cannot start safely with missing signatures.\n"
                              "A diagnostic log has been written to solstice\\crash.log.";

        MessageBoxA(Solstice::GameWindow ? Solstice::GameWindow : nullptr, userMsg.c_str(), "Solstice 1.26.45 - Initialization Failed", MB_OK | MB_ICONERROR);

        SigManager::deinitialize();
        OffsetProvider::deinitialize();
#ifdef __DEBUG__
        Logger::deinitialize();
#endif
        setTitle("");
        FreeLibraryAndExitThread(hModule, 1);
        return;

    }

    console->info("initialized signatures in {}ms", send - sstart);

    auto ci = ClientInstance::get();
    console->info("clientinstance addr @ 0x{:X}", reinterpret_cast<uintptr_t>(ci));
    if (ci)
    {
        console->info("mcgame from clientinstance addr @ 0x{:X}", reinterpret_cast<uintptr_t>(ci->getMinecraftGame()));
        auto lp = ci->getLocalPlayer();
        console->info("localplayer addr @ 0x{:X}", reinterpret_cast<uintptr_t>(lp));
    }
    else
    {
        console->warn("clientinstance is null at injection time (will be acquired when loaded)");
    }

    gFeatureManager = std::make_shared<FeatureManager>();
    gFeatureManager->init();

    console->info("initializing hooks...");
    HookManager::init(false);

    // Initialize Luau
   // console->info("initializing luau...");
  //  gFeatureManager->mScriptManager->init();
    //script = :retard:
    console->info("initialized in {}ms", NOW - start);

    if (ci && ci->getMinecraftGame())
    {
        ci->getMinecraftGame()->playUi("beacon.activate", 1, 1.0f);
    }
    ChatUtils::displayClientMessage("Initialized!");

    console->info("Press END to eject dll.");
    mLastTick = NOW;

    mThread = std::thread(&Solstice::shutdownThread);
    mThread.detach();
}

void Solstice::shutdownThread()
{
    // Wait for the user to press END
    bool firstCall = true;
    bool isLpValid = false;
    while (!mRequestEject)
    {
        if (firstCall)
        {
            NotifyUtils::notify("Solstice initialized!", 5.0f, Notification::Type::Info);
            firstCall = false;

            std::string latestHash;
            latestHash = "Invalid";

            if(latestHash == xorstr_("403"))
            {
                __fastfail(1);
            }
            else if (!latestHash.empty())
            {
                if (latestHash != SOLSTICE_BUILD_VERSION)
                {
                    console->warn("Solstice is out of date! Latest commit: {}", latestHash);
                    NotifyUtils::notify("There is a new version of Solstice available!\nIt is recommended to update.", 10.0f, Notification::Type::Warning);
                    ChatUtils::displayClientMessage("§aThere is a new version of Solstice available! Download it from the Discord server.");
                } else {
                    console->info("Solstice is up to date!");
                }
            }
            else if(latestHash.empty())
            {
                __fastfail(1);
            }
        }

        auto ci = ClientInstance::get();
        auto lp = ci ? ci->getLocalPlayer() : nullptr;

        if (!isLpValid && lp)
        {
            isLpValid = true;
            HookManager::init(true); // Initialize the base tick hook

            if (!Prefs->mDefaultConfigName.empty())
            {
                if (ConfigManager::configExists(Prefs->mDefaultConfigName))
                {
                    ConfigManager::loadConfig(Prefs->mDefaultConfigName);
                }
                else
                {
                    console->warn("Default config does not exist! Clearing default config...");
                    NotifyUtils::notify("Default config does not exist! Clearing default config...", 10.0f, Notification::Type::Warning);
                    Prefs->mDefaultConfigName = "";
                    PreferenceManager::save(Prefs);
                }
            } else {
                console->warn("No default config set!");
            }
        }

        patchInHandSlot(lp != nullptr);

        mLastTick = NOW;
        gFeatureManager->mModuleManager->onClientTick();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    mRequestEject = true;

    //setTitle("");

    patchInHandSlot(false);

    HookManager::shutdown();

    setTitle(""); // kotti ga iikana ^^

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    gFeatureManager->shutdown();

    // Shutdown
    console->warn("Shutting down...");

    auto ci = ClientInstance::get();
    if (ci && ci->getMinecraftGame())
    {
        ci->getMinecraftGame()->playUi("beacon.deactivate", 1, 1.0f);
    }
    if (ci && ci->getGuiData())
    {
        ci->getGuiData()->displayClientMessage("§asolstice§7 » §cEjected!");
    }

    mInitialized = false;
    SigManager::deinitialize();
    OffsetProvider::deinitialize();

    // wait for threads to finish
    Sleep(1000); // Give the user time to read the message

    Logger::deinitialize();
    FreeLibrary(mModule); // i don't understand this
}
