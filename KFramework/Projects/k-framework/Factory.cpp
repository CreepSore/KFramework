#include "k-framework.h"

kfw::core::HackManager* kfw::core::Factory::hackManager = nullptr;
kfw::core::HookManager* kfw::core::Factory::hookManager = nullptr;

kfw::core::HookManager * kfw::core::Factory::getDefaultHookManager()
{
    if (hookManager == nullptr) hookManager = new HookManager();
    return hookManager;
}

kfw::core::HackManager* kfw::core::Factory::getDefaultHackManager()
{
    if (hackManager == nullptr) hackManager = new HackManager();
    return hackManager;
}

void kfw::core::Factory::cleanup()
{
    delete hackManager;
    delete hookManager;
}
