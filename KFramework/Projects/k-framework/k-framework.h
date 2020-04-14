#pragma once
#include <Windows.h>
#include <string>
#include <vector>

namespace kfw {
    namespace core {
        #define PATCHTYPE_ADDRESS 0x0
        #define PATCHTYPE_PATTERN 0x1

        struct HookData;
        class IBaseHack;

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

        class HackManager final {
            std::vector<IBaseHack*>* hacks;
        public:
            void registerHack(IBaseHack* hack) const;
            void unregisterHack(const std::string& identifier) const;
            IBaseHack* getHackByIdentifier(const std::string& identifier) const;
            bool doesIdentifierExist(const std::string& identifier) const;

            bool onEvent(std::string event, void* args) const;

            HackManager();
            ~HackManager();
        };

        class IBaseHack {
        protected:
            IBaseHack(std::string identifier, std::string hrIdentifier);
        public:
            virtual ~IBaseHack() = default;

            HackManager* manager;

            std::string identifier;
            std::string hrIdentifier; // Human-Readable Identifier

            virtual void onRegister() = 0;
            virtual void onUnregister() = 0;

            virtual bool onEvent(std::string event, void* args) = 0;
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
    
        class PatchInfo {
        private:
            bool patchPattern();
            bool patchAddress();
            char* oldBytes;
            int type = 0;
        public:
            DWORD64 address = 0;
            bool patched = false;
            char* pattern;
            char* mask;
            char* module;
            char* name;
            char* toWrite;
            bool prefetchAddress();
            bool patch();
            bool unpatch();

            PatchInfo(DWORD64 address, const char* toWrite, const char* name);
            PatchInfo(const char* module, const char* pattern, const char* mask, const char* toWrite, const char* name);
        };
    
        class Factory {
            static HookManager * hookManager;
            static HackManager * hackManager;
        public:
            static HookManager * getDefaultHookManager();
            static HackManager * getDefaultHackManager();
            static void cleanup();
        };
    }
}