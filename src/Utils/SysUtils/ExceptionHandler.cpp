//
// Created by vastrakai on 7/7/2024.
// Updated for Minecraft 1.26.45
//

#include "ExceptionHandler.hpp"

#include <build_info.h>
#include <Windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <spdlog/spdlog.h>

#include "StackWalker.hpp"
#include <Utils/FileUtils.hpp>
#include <Utils/ProcUtils.hpp>
#include <Utils/SysUtils/xorstr.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/OffsetProvider.hpp>
#include <Features/FeatureManager.hpp>
#include <Features/Modules/Module.hpp>
#include <Features/Modules/ModuleManager.hpp>

static const char* GetExceptionCodeString(DWORD code)
{
    switch (code)
    {
    case EXCEPTION_ACCESS_VIOLATION:         return "EXCEPTION_ACCESS_VIOLATION";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
    case EXCEPTION_BREAKPOINT:               return "EXCEPTION_BREAKPOINT";
    case EXCEPTION_DATATYPE_MISALIGNMENT:    return "EXCEPTION_DATATYPE_MISALIGNMENT";
    case EXCEPTION_FLT_DENORMAL_OPERAND:     return "EXCEPTION_FLT_DENORMAL_OPERAND";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
    case EXCEPTION_FLT_INEXACT_RESULT:       return "EXCEPTION_FLT_INEXACT_RESULT";
    case EXCEPTION_FLT_INVALID_OPERATION:    return "EXCEPTION_FLT_INVALID_OPERATION";
    case EXCEPTION_FLT_OVERFLOW:             return "EXCEPTION_FLT_OVERFLOW";
    case EXCEPTION_FLT_STACK_CHECK:          return "EXCEPTION_FLT_STACK_CHECK";
    case EXCEPTION_FLT_UNDERFLOW:            return "EXCEPTION_FLT_UNDERFLOW";
    case EXCEPTION_ILLEGAL_INSTRUCTION:      return "EXCEPTION_ILLEGAL_INSTRUCTION";
    case EXCEPTION_IN_PAGE_ERROR:            return "EXCEPTION_IN_PAGE_ERROR";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "EXCEPTION_INT_DIVIDE_BY_ZERO";
    case EXCEPTION_INT_OVERFLOW:             return "EXCEPTION_INT_OVERFLOW";
    case EXCEPTION_INVALID_DISPOSITION:      return "EXCEPTION_INVALID_DISPOSITION";
    case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
    case EXCEPTION_PRIV_INSTRUCTION:         return "EXCEPTION_PRIV_INSTRUCTION";
    case EXCEPTION_SINGLE_STEP:              return "EXCEPTION_SINGLE_STEP";
    case EXCEPTION_STACK_OVERFLOW:           return "EXCEPTION_STACK_OVERFLOW";
    case 0xDEAD0001:                         return "SOLSTICE_SIGNATURE_ERROR";
    default:                                 return "UNKNOWN_EXCEPTION";
    }
}

static std::string GetModuleAndOffset(uintptr_t address)
{
    HMODULE hModule = nullptr;
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCSTR>(address), &hModule) && hModule)
    {
        char path[MAX_PATH] = { 0 };
        GetModuleFileNameA(hModule, path, MAX_PATH);
        char* fileName = strrchr(path, '\\');
        std::string modName = fileName ? (fileName + 1) : path;
        uintptr_t rva = address - reinterpret_cast<uintptr_t>(hModule);
        return fmt::format("{} + 0x{:X}", modName, rva);
    }
    return "Unknown Module";
}

LONG WINAPI TopLevelExceptionHandler(const PEXCEPTION_POINTERS pExceptionInfo)
{
    auto exceptionCode = pExceptionInfo->ExceptionRecord->ExceptionCode;
    const char* exceptionName = GetExceptionCodeString(exceptionCode);
    uintptr_t faultAddress = reinterpret_cast<uintptr_t>(pExceptionInfo->ExceptionRecord->ExceptionAddress);
    std::string faultLocation = GetModuleAndOffset(faultAddress);

    std::stringstream report;
    report << "================================================================================\n";
    report << "CRASH REPORT - Solstice for Minecraft 1.26.45\n";

    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    report << "Crash Time: " << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %X") << "\n";
    report << "Target Client Version: 1.26.45\n";
    report << "Detected Minecraft Version: " << ProcUtils::getVersion() << "\n";
    report << "================================================================================\n\n";

    report << fmt::format("Exception: {} (0x{:08X})\n", exceptionName, exceptionCode);
    report << fmt::format("Fault Address: 0x{:016X} ({})\n", faultAddress, faultLocation);

    // If Access Violation, parse details
    if (exceptionCode == EXCEPTION_ACCESS_VIOLATION && pExceptionInfo->ExceptionRecord->NumberParameters >= 2)
    {
        ULONG_PTR accessType = pExceptionInfo->ExceptionRecord->ExceptionInformation[0];
        ULONG_PTR targetAddr = pExceptionInfo->ExceptionRecord->ExceptionInformation[1];
        std::string typeStr;
        if (accessType == 0) typeStr = "Read from";
        else if (accessType == 1) typeStr = "Write to";
        else if (accessType == 8) typeStr = "DEP execution at";
        else typeStr = "Access at";

        report << fmt::format("Access Violation: Attempt to {} address 0x{:016X}", typeStr, targetAddr);
        if (targetAddr == 0)
        {
            report << " [NULL POINTER DEREFERENCE]";
        }
        report << "\n";
    }

    // Registers Dump (x64)
    if (pExceptionInfo->ContextRecord)
    {
        const auto& ctx = *pExceptionInfo->ContextRecord;
        report << "\nRegisters (x64):\n";
        report << fmt::format("  RIP: 0x{:016X} ({})\n", ctx.Rip, GetModuleAndOffset(ctx.Rip));
        report << fmt::format("  RAX: 0x{:016X}  RBX: 0x{:016X}\n", ctx.Rax, ctx.Rbx);
        report << fmt::format("  RCX: 0x{:016X}  RDX: 0x{:016X}\n", ctx.Rcx, ctx.Rdx);
        report << fmt::format("  RSI: 0x{:016X}  RDI: 0x{:016X}\n", ctx.Rsi, ctx.Rdi);
        report << fmt::format("  RBP: 0x{:016X}  RSP: 0x{:016X}\n", ctx.Rbp, ctx.Rsp);
        report << fmt::format("  R8:  0x{:016X}  R9:  0x{:016X}\n", ctx.R8,  ctx.R9);
        report << fmt::format("  R10: 0x{:016X}  R11: 0x{:016X}\n", ctx.R10, ctx.R11);
        report << fmt::format("  R12: 0x{:016X}  R13: 0x{:016X}\n", ctx.R12, ctx.R13);
        report << fmt::format("  R14: 0x{:016X}  R15: 0x{:016X}\n", ctx.R14, ctx.R15);
        report << fmt::format("  EFLAGS: 0x{:08X}\n", ctx.EFlags);
    }

    // Check failed signatures
    std::vector<std::string> failedSigs;
    if (SigManager::mIsInitialized)
    {
        for (const auto& [name, addr] : SigManager::mSigs)
        {
            if (addr == 0) failedSigs.push_back("[Sig] " + name);
        }
    }
    if (OffsetProvider::mIsInitialized)
    {
        for (const auto& [name, addr] : OffsetProvider::mSigs)
        {
            if (addr == 0) failedSigs.push_back("[Offset] " + name);
        }
    }

    if (!failedSigs.empty())
    {
        report << fmt::format("\n*** WARNING: {} FAILED SIGNATURES / OFFSETS DETECTED ***\n", failedSigs.size());
        report << "The following were not found (NULL pointer in memory) and likely caused the crash:\n";
        for (const auto& s : failedSigs)
        {
            report << "  ! " << s << " = 0 (NULL)\n";
        }
    }
    else
    {
        report << "\nSignatures Status: All registered signatures and offsets resolved successfully.\n";
    }

    // Stack Trace
    report << "\nStack Trace:\n";
    StackWalker sw;
    std::vector<std::string> stackTrace = sw.ShowCallstack(GetCurrentThread(), pExceptionInfo->ContextRecord);
    for (const auto& line : stackTrace)
    {
        report << line << "\n";
    }

    // Solstice Modules
    report << "\nSolstice Modules:\n";
    auto modules = gFeatureManager ? gFeatureManager->mModuleManager->getModules() : std::vector<std::shared_ptr<Module>>();
    for (const auto& module : modules)
    {
        report << "  - " << module->mName << " : " << (module->mEnabled ? "Enabled" : "Disabled") << "\n";
    }
    if (gFeatureManager)
    {
        report << "Total modules: " << modules.size() << "\n";
    }
    else
    {
        report << "(FeatureManager was not initialized yet)\n";
    }

    // Build details
    report << "\nBuild Details:\n";
    report << "Solstice Version: " << SOLSTICE_VERSION << "\n";
    report << "Target Minecraft Version: 1.26.45\n";
    report << "Solstice Commit: " << SOLSTICE_BUILD_VERSION << "\n";
    report << "Solstice Branch: " << SOLSTICE_BUILD_BRANCH << "\n";
    report << "Solstice Commit Msg: " << SOLSTICE_BUILD_COMMIT_MESSAGE << "\n";
    report << "================================================================================\n";

    std::string fullReport = report.str();
    spdlog::error(fullReport);

    ExceptionHandler::makeCrashLog(fullReport, exceptionCode);

    std::string boxMsg = fmt::format(
        "Solstice Crash Detected!\n\n"
        "Exception: {} (0x{:X})\n"
        "Fault Address: 0x{:X} ({})\n",
        exceptionName, exceptionCode, faultAddress, faultLocation
    );
    if (!failedSigs.empty())
    {
        boxMsg += fmt::format("\nWarning: {} missing signature(s) detected!\n", failedSigs.size());
    }
    boxMsg += "\nA full detailed diagnostic report was saved to:\n" + FileUtils::getSolsticeDir() + "crash.log";

    auto result = MessageBoxA(nullptr, boxMsg.c_str(), "Solstice 1.26.45 - Crash Handler", MB_ABORTRETRYIGNORE | MB_ICONERROR);

    stackTrace.clear();
    sw.~StackWalker();

    switch (result)
    {
    case IDABORT:
        exit(1);
    case IDRETRY:
        return EXCEPTION_CONTINUE_EXECUTION;
    case IDIGNORE:
        return EXCEPTION_CONTINUE_SEARCH;
    default:
        return EXCEPTION_EXECUTE_HANDLER;
    }
}

void ExceptionHandler::init()
{
    SetUnhandledExceptionFilter(TopLevelExceptionHandler);
}

void ExceptionHandler::makeCrashLog(const std::string& text, DWORD exceptionCode)
{
    std::string excPath = FileUtils::getSolsticeDir() + xorstr_("crash.log");
    std::ofstream excFile(excPath, std::ios::app);
    if (excFile.is_open())
    {
        excFile << text << "\n";
        excFile.flush();
        excFile.close();
    }
}
