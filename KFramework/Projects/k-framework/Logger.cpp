#include "k-framework.h"
#include <stdio.h>
#include <iostream>
#include <ctime>

void kfw::core::Logger::log(const std::string& msg, const std::string& context)
{
    // Next-Level hacky ISO-Date thing
    // thanks @ https://stackoverflow.com/questions/9527960/how-do-i-construct-an-iso-8601-datetime-in-c
    const time_t now = time(nullptr);
    struct tm timeinfo;
    char buf[21];
    gmtime_s(&timeinfo, &now);
    strftime(buf, sizeof buf, "%FT%TZ", &timeinfo);

    std::cout << "[" << buf << "]" << "[" << context << "] " << msg << std::endl;
}
