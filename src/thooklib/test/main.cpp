#define GTEST_LANG_CXX11 1
#include "thooklib/hooklib.h"
#include "thooklib/utility.h"

#include <common/sane_windows.h>

#include <MinHook.h>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <usvfs_shared/exceptionex.h>

#include <iostream>

using namespace std;
using namespace HookLib;

static const HANDLE MARKERHANDLE = reinterpret_cast<HANDLE>(0x1CC0FFEE);
static const CHAR INVALID_FILENAME[] = "\\<>/";
static const WCHAR INVALID_FILENAMEW[] = L"\\<>/";

HANDLE WINAPI THCreateFileA_1(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                              LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                              DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    if (strcmp(lpFileName, INVALID_FILENAME) == 0) {
        return MARKERHANDLE;
    } else {
        MH_DisableHook(CreateFileA);
        HANDLE res = ::CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
                                   dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
        MH_EnableHook(CreateFileA);
        return res;
    }
}

HANDLE WINAPI THCreateFileW_1(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                              LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                              DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    if (wcscmp(lpFileName, INVALID_FILENAMEW) == 0) {
        EXPECT_EQ(0x42, dwDesiredAccess);
        EXPECT_EQ(0x43, dwShareMode);
        EXPECT_EQ(0x44, (int)lpSecurityAttributes);
        EXPECT_EQ(0x45, dwCreationDisposition);
        EXPECT_EQ(0x46, dwFlagsAndAttributes);
        EXPECT_EQ(0x47, (int)hTemplateFile);
        return MARKERHANDLE;
    } else {
        return ::CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
                             dwFlagsAndAttributes, hTemplateFile);
    }
}

class HookingTest : public testing::Test {
  public:
    void SetUp() {
        /*    typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
            boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

            // Add a stream to write log to
            sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>("c:\\temp\\testing_out.log"));

            // Register the sink in the logging core
            logging::core::get()->add_sink(sink);

            sink->set_filter(expr::attr<LogLevel>("Severity") >= LogLevel::Debug);*/
    }

    void TearDown() {}

  private:
};

static shared_ptr<spdlog::logger> logger() {
    shared_ptr<spdlog::logger> result = spdlog::get("test");
    if (result.get() == nullptr) {
        result = spdlog::stdout_logger_mt("test");
    }
    return result;
}

TEST(GetProcAddressTest, ReturnsValidResults) {
    HMODULE mh = GetModuleHandleA("KernelBase.dll");
    EXPECT_NE(nullptr, mh);
    EXPECT_EQ(GetProcAddress(mh, "CreateFileA"), MyGetProcAddress(mh, "CreateFileA"));
}

TEST_F(HookingTest, CanHook) {
    HMODULE k32Mod = GetModuleHandleA("kernel32.dll");
    HOOKHANDLE hook = InstallHook(k32Mod, "CreateFileW", THCreateFileW_1);
    if (hook == INVALID_HOOK) {
        k32Mod = GetModuleHandleA("kernelbase.dll");
        hook = InstallHook(k32Mod, "CreateFileW", THCreateFileW_1);
    }
    EXPECT_NE(INVALID_HOOK, hook);
    RemoveHook(hook);
}

TEST_F(HookingTest, RemoveHook) {
    // test that we can remove a hook
    HMODULE k32Mod = GetModuleHandleA("kernel32.dll");
    HOOKHANDLE hook = InstallHook(k32Mod, "CreateFileA", THCreateFileA_1);
    if (hook == INVALID_HOOK) {
        k32Mod = GetModuleHandleA("kernelbase.dll");
        hook = InstallHook(k32Mod, "CreateFileA", THCreateFileA_1);
    }

    EXPECT_NE(INVALID_HOOK, hook);
    RemoveHook(hook);
    HANDLE test = CreateFileA(INVALID_FILENAME, GENERIC_READ, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    EXPECT_EQ(INVALID_HANDLE_VALUE, test);
}

TEST_F(HookingTest, CreateFileHook) {
    // test if our hook works
    HMODULE k32Mod = GetModuleHandleA("kernel32.dll");
    HOOKHANDLE hook = InstallHook(k32Mod, "CreateFileA", THCreateFileA_1);
    if (hook == INVALID_HOOK) {
        k32Mod = GetModuleHandleA("kernelbase.dll");
        hook = InstallHook(k32Mod, "CreateFileA", THCreateFileA_1);
    }
    HANDLE test = CreateFileA(INVALID_FILENAME, GENERIC_READ, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    RemoveHook(hook);
    EXPECT_EQ(MARKERHANDLE, test);
}

TEST_F(HookingTest, CreateFileWHook) {
    // test if our hook works
    HMODULE k32Mod = GetModuleHandleA("kernel32.dll");
    HOOKHANDLE hook = InstallHook(k32Mod, "CreateFileW", THCreateFileW_1);
    if (hook == INVALID_HOOK) {
        k32Mod = GetModuleHandleA("kernelbase.dll");
        hook = InstallHook(k32Mod, "CreateFileW", THCreateFileW_1);
    }
    HANDLE test = CreateFileW(INVALID_FILENAMEW, 0x42, 0x43, (LPSECURITY_ATTRIBUTES)0x44, 0x45, 0x46, (HANDLE)0x47);
    RemoveHook(hook);
    EXPECT_EQ(MARKERHANDLE, test);
}

TEST_F(HookingTest, CreateFileHookRecursion) {
    // test that the trampoline works, so we can call the original function from
    // within the hook
    HMODULE k32Mod = GetModuleHandleA("kernel32.dll");
    HOOKHANDLE hook = InstallHook(k32Mod, "CreateFileA", THCreateFileA_1);
    if (hook == INVALID_HOOK) {
        k32Mod = GetModuleHandleA("kernelbase.dll");
        hook = InstallHook(k32Mod, "CreateFileA", THCreateFileA_1);
    }
    HANDLE test = CreateFileA("VALID_FILENAME", GENERIC_READ, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    RemoveHook(hook);
    EXPECT_NE(MARKERHANDLE, test);
}

// FIXME: Fails. Can't figure out why. Don't know if it passes without my changes or not anyway.
TEST_F(HookingTest, Threading) {
    // test that multiple threads can concurrently call a hooked function without
    // incorrect results.
    // TODO: this test doesn't reliably find thread-unsafeties
    // NOTE: the hooklib currently does not claim that hook installation or removal
    //   is thread-safe, only the hooked functions shouldn't become less thread-safe by
    //   being hooked!
    static const int NUM_THREADS = 100;
    static const int NUM_TRIES = 1000;

    HMODULE k32Mod = GetModuleHandleA("kernel32.dll");
    HOOKHANDLE hook = InstallHook(k32Mod, "CreateFileA", THCreateFileA_1);
    if (hook == INVALID_HOOK) {
        k32Mod = GetModuleHandleA("kernelbase.dll");
        hook = InstallHook(k32Mod, "CreateFileA", THCreateFileA_1);
    }
    std::thread threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i] = std::thread([i] {
            for (int count = 0; count < NUM_TRIES; ++count) {
                HANDLE test = CreateFileA(INVALID_FILENAME, GENERIC_READ, 0, nullptr, OPEN_ALWAYS,
                                          FILE_ATTRIBUTE_NORMAL, nullptr);
                EXPECT_EQ(MARKERHANDLE, test);
            }
        });
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i].join();
    }

    RemoveHook(hook);
}

int main(int argc, char** argv) {
    MH_Initialize();
    auto logger = spdlog::stdout_logger_mt("usvfs");
    logger->set_level(spdlog::level::warn);
    testing::InitGoogleTest(&argc, argv);
    auto tmp = RUN_ALL_TESTS();
    MH_Uninitialize();
    return tmp;
}
