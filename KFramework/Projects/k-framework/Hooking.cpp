#include <iostream>

#include "k-framework.h"

#pragma region HookData
kfw::core::HookData::HookData(void* toHook, void* hookedFunc, const size_t patchSize,
    const std::string& identifier, const std::string& hrIdentifier)
{
    this->identifier = identifier;
    this->hrIdentifier = hrIdentifier;
    this->patchSize = patchSize;
    this->oldBytes = new unsigned char[patchSize];
    this->vpToHook = toHook;
    this->vpHookedFunc = hookedFunc;
    this->jmpToAddr = 0;
}

kfw::core::HookData::~HookData()
{
    this->unhook();
}

void kfw::core::HookData::setupHook()
{
    memcpy(oldBytes, vpToHook, patchSize);
    bIsSettedUp = true;
}

bool kfw::core::HookData::hook()
{
    if (bIsHooked || !bIsSettedUp || patchSize < 5) return false;

    DWORD oldProtection;
    VirtualProtect(this->vpToHook, patchSize, PAGE_EXECUTE_READWRITE, &oldProtection);

    const VDWORD relAddress = reinterpret_cast<VDWORD>(this->vpHookedFunc) - reinterpret_cast<VDWORD>(this->vpToHook) - 5;

    *static_cast<BYTE*>(this->vpToHook) = 0xE9;
    *reinterpret_cast<VDWORD*>(reinterpret_cast<VDWORD>(this->vpToHook) + 0x1) = relAddress;
    memset(reinterpret_cast<void*>(reinterpret_cast<VDWORD>(this->vpToHook) + 0x5), 0x90, patchSize - 5);
    jmpToAddr = reinterpret_cast<VDWORD>(this->vpToHook) + patchSize;

    DWORD temp;
    VirtualProtect(this->vpToHook, patchSize, oldProtection, &temp);

    header = VirtualAlloc(NULL, patchSize + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(header, this->oldBytes, patchSize);
    memset(reinterpret_cast<void*>(reinterpret_cast<VDWORD>(header) + patchSize), 0xE9, 1);
    *reinterpret_cast<VDWORD*>(reinterpret_cast<VDWORD>(header) + patchSize + 1) = (reinterpret_cast<VDWORD>(this->vpToHook) - (reinterpret_cast<VDWORD>(header) + patchSize));

    this->origFunction = VirtualAlloc(NULL, patchSize + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(this->origFunction, this->oldBytes, patchSize);

    if(((BYTE*)this->origFunction)[0] == 0xE9)
    {
        DWORD* jmpAddr = reinterpret_cast<DWORD*>(reinterpret_cast<DWORD>(this->origFunction) + 1);
        DWORD originalTarget = reinterpret_cast<DWORD>(this->vpToHook) + *jmpAddr;
        DWORD newTarget = originalTarget - reinterpret_cast<VDWORD>(this->origFunction);
        *jmpAddr = newTarget;
    }

    const VDWORD relAddressOrig = (reinterpret_cast<VDWORD>(this->vpToHook) - reinterpret_cast<VDWORD>(this->origFunction)) - patchSize;
    *reinterpret_cast<BYTE*>(reinterpret_cast<VDWORD>(this->origFunction) + patchSize) = 0xE9;
    *reinterpret_cast<VDWORD*>(reinterpret_cast<VDWORD>(this->origFunction) + patchSize + 0x1) = relAddressOrig;

    this->bIsHooked = true;
    return true;
}

bool kfw::core::HookData::unhook()
{
    if (!bIsHooked) return false;

    DWORD oldProtection;
    VirtualProtect(this->vpToHook, patchSize, PAGE_EXECUTE_READWRITE, &oldProtection);
    memcpy(vpToHook, this->oldBytes, patchSize);

    VirtualProtect(this->vpToHook, patchSize, oldProtection, &oldProtection);
    delete[] this->oldBytes;

    VirtualFree(this->origFunction, this->patchSize + 5, MEM_RELEASE);
    VirtualFree(this->header, this->patchSize + 5, MEM_RELEASE);
    VirtualFree(this->origFunction, this->patchSize + 5, MEM_RELEASE);

    bIsHooked = false;

    return true;
}
#pragma endregion

#pragma region HookManager
kfw::core::HookManager::HookManager() {
    hooks = new std::vector<HookData*>();
}

kfw::core::HookManager::~HookManager() {
    this->unhookAll();
}

void kfw::core::HookManager::registerHook(kfw::core::HookData* hook) const {
    if (doesHookExist(hook->identifier)) return;
    hooks->push_back(hook);
}

void kfw::core::HookManager::unregisterHook(const std::string& identifier) const {
    auto ir = hooks->begin();
    while (ir != hooks->end())
    {
        HookData* hook = *ir;
        if (hook->identifier != identifier) {
            ++ir;
            continue;
        }

        hooks->erase(ir);
        delete hook;
        return;
    }
}

kfw::core::HookData* kfw::core::HookManager::getHookByIdentifier(const std::string& identifier) const {
    for (auto hook : *hooks)
    {
        if (hook->identifier == identifier) return hook;
    }

    return nullptr;
}

bool kfw::core::HookManager::doesHookExist(const std::string& identifier) const {
    return getHookByIdentifier(identifier) != nullptr;
}

void kfw::core::HookManager::hookAll() const {
    for (auto hook : *hooks)
    {
        hook->setupHook();
        hook->hook();
    }
}

void kfw::core::HookManager::unhookAll() const {
    for (auto hook : *hooks)
    {
         hook->unhook();
    }
}
#pragma endregion