#include "k-framework/k-framework.h"

auto logger = kfw::core::Logger();

int main()
{
    auto hackManager = kfw::core::Factory::getDefaultHackManager();
    auto hookManager = kfw::core::Factory::getDefaultHookManager();
    logger.log("Initialized managers", "main");


    kfw::core::Factory::cleanup();
    logger.log("Cleaned up. Exiting ..", "main");
}
