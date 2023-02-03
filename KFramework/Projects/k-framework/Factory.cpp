#include "k-framework.h"

kfw::core::HackManager* kfw::core::Factory::hackManager = nullptr;
kfw::core::HookManager* kfw::core::Factory::hookManager = nullptr;
kfw::core::Logger* kfw::core::Factory::logger = nullptr;

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

kfw::core::Logger* kfw::core::Factory::getDefaultLogger()
{
    if (logger == nullptr) logger = new Logger();
    return logger;
}

void kfw::core::Factory::cleanup()
{
    delete hackManager;
    delete hookManager;
}
