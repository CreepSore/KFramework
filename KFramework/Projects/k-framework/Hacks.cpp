#include "k-framework.h"

kfw::core::IBaseHack::IBaseHack(std::string identifier, std::string hrIdentifier)
{
    this->identifier = std::move(identifier);
    this->hrIdentifier = std::move(hrIdentifier);
    this->manager = nullptr; // Gets set during registration inside the HackManager
}


kfw::core::HackManager::HackManager() {
    hacks = new std::vector<IBaseHack*>();
}

kfw::core::HackManager::~HackManager() {
    delete hacks;
}

void kfw::core::HackManager::registerHack(IBaseHack* hack) const {
    if (doesIdentifierExist(hack->identifier))
    {
        return;
    }

    hacks->push_back(hack);
    hack->onRegister();
}

void kfw::core::HackManager::unregisterHack(const std::string& identifier) const {
    auto ir = hacks->begin();
    while (ir != hacks->end())
    {
        IBaseHack* hack = *ir;
        if (hack->identifier != identifier) {
            ++ir;
            continue;
        }

        hack->onUnregister();
        hacks->erase(ir);
        delete hack;
        return;
    }
}

kfw::core::IBaseHack* kfw::core::HackManager::getHackByIdentifier(const std::string& identifier) const {
    for (auto hack : *hacks)
    {
        if (hack->identifier == identifier) return hack;
    }

    return nullptr;
}

bool kfw::core::HackManager::doesIdentifierExist(const std::string& identifier) const {
    return getHackByIdentifier(identifier) != nullptr;
}

bool kfw::core::HackManager::onEvent(std::string event, void* args) const {
    bool result = false;
    for (IBaseHack* hack : *hacks)
    {
        if (hack->onEvent(event, args))
        {
            result = true;
        }
    }
    return result;
}
