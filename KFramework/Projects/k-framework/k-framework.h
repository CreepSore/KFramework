#pragma once
#include <Windows.h>
#include <string>
#include <vector>

namespace kfw {
    namespace core {
        struct HookData {
            std::string identifier;
            std::string hrIdentifier;

            HookData(void* toHook, void* hookedFunc, const size_t patchSize,
                const std::string& identifier, const std::string& hrIdentifier);
            virtual ~HookData();
            bool bIsSettedUp = false;
            bool bIsHooked = false;

            void* vpToHook = nullptr;
            void* vpHookedFunc = nullptr;

            // Trampoline Stuff
            int patchSize = 0;
            DWORD jmpToAddr = 0;
            void* header;
            unsigned char * oldBytes;

            void setupHook();
            bool hook();
            bool unhook();
        };

        class HookManager {
            std::vector<HookData*>* hooks;
        public:
            void registerHook(HookData* hook) const;
            void unregisterHook(const std::string& identifier) const;
            HookData* getHookByIdentifier(const std::string& identifier) const;
            bool doesHookExist(const std::string& identifier) const;

            void hookAll() const;
            void unhookAll() const;

            HookManager();
            ~HookManager();
        };

        class HackManager {
        public:
        };

        class Logger {
        public:
            void log(const std::string& msg, const std::string& context);
        };

        class Utils {
        public:
            static bool setupConsole();
            static DWORD64 getModuleAddress(const wchar_t* modulename);
            static DWORD64 getFunctionAddress(const wchar_t* modulename, const char* funcname);
            static DWORD64 findPattern(const DWORD64 addrFrom, const DWORD64 addrTo, const char* pattern, const char* mask);
            static DWORD64 findPattern(const HMODULE module, const char* pattern, const char* mask);
            static DWORD64 findPattern(const char* module, const char* pattern, const char* mask);
        };
    }
}