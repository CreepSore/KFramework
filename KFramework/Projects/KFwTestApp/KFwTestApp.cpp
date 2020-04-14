#pragma warning(disable:4244)
#include <iostream>
#include <string>
#include <sstream>
#include "k-framework/k-framework.h"

template <class T>
void Assert(T x, T y);
void Assert(unsigned char* x, unsigned char* y, int len);

void* msgboxOld;

kfw::core::Logger logger;

int __stdcall MessageBoxAHk(HWND hWnd, LPCSTR lpTest, LPCSTR lpCaption, UINT utype) {
    utype = MB_ICONQUESTION;
    lpCaption = "test";
    logger.log("MessageBoxAHk called successfully", "Hook test");
    return ((int(__stdcall*)(HWND hWnd, LPCSTR lpTest, LPCSTR lpCaption, UINT utype)) msgboxOld)(hWnd, lpTest, lpCaption, utype);
}

void __declspec(naked) test() {
    __asm {
        push eax
        mov eax, 10
        loop_dest:
        dec eax
        cmp eax, 0
        jne loop_dest
        nop
        pop eax
        ret
    }
}

int main()
{
    // Logger + SetupConsole Test
    logger = kfw::core::Logger();
    kfw::core::Utils::setupConsole();
    std::stringstream sstr;
    sstr << "0x" << std::hex << kfw::core::Utils::getModuleAddress(L"KFwTestApp.exe") << std::dec;
    logger.log(sstr.str(), "KFwTestApp::getModuleAddress");

    // Hook Test
    auto hookmgr = kfw::core::HookManager();
    hookmgr.registerHook(new kfw::core::HookData(MessageBoxA, MessageBoxAHk, 5, "WINAPI_MESSAGEBOXA", "MessageBoxA"));
    hookmgr.hookAll();
    msgboxOld = hookmgr.getHookByIdentifier("WINAPI_MESSAGEBOXA")->header;
    MessageBoxA(0, "kek", "lmao", 0);

    // Pattern-Scan Test
    sstr.str(std::string());
    test();
    DWORD res = kfw::core::Utils::findPattern("KFwTestApp.exe", "\x50\xb8\xa\x00\x00\x00\x48\x83\xf8\x00\x75\xfa\x90", "xxx???xxxxxxx");
    DWORD addr = *reinterpret_cast<DWORD*>(DWORD(test) + 1);
    addr += DWORD(test) + 5;
    sstr << "Test: 0x" << std::hex << addr << " = 0x" << std::hex << res;
    logger.log(sstr.str(), "PatternScan");

    // Factory test
    if (kfw::core::Factory::getDefaultHookManager() != nullptr) {
        DebugBreak();
    }

    if (kfw::core::Factory::getDefaultHackManager() != nullptr) {
        DebugBreak();
    }

    kfw::core::Factory::cleanup();
    std::cout << "Execution completed successfully";
}

void Assert(unsigned char* x, unsigned char* y, int len)
{
    bool doBreak = false;
    for (int i = 0; i < len; i++) {
        if (x != y) {
            doBreak = true;
            break;
        }
    }
    DebugBreak();
}

template<class T>
void Assert(T x, T y)
{
    if (x != y) DebugBreak();
}
