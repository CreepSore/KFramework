#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include "k-framework.h"

bool kfw::core::Utils::setupConsole()
{
    if (AllocConsole()) {
        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
        return true;
    }
    return false;
}

DWORD64 kfw::core::Utils::getModuleAddress(const wchar_t* modulename)
{
    return DWORD64(GetModuleHandleW(modulename));
}

DWORD64 kfw::core::Utils::getFunctionAddress(const wchar_t* modulename, const char* funcname)
{
    return DWORD64(GetProcAddress(GetModuleHandleW(modulename), funcname));
}

DWORD64 kfw::core::Utils::findPattern(const DWORD64 addrFrom, const DWORD64 addrTo, const char* pattern, const char* mask)
{
    const size_t patternLength = strlen(mask);
    if (patternLength == 0) return 0;

    for (DWORD i = addrFrom; i < addrTo; i++)
    {
        char* current = reinterpret_cast<char*>(i);
        bool patternFailed = false;
        for (size_t pOff = 0; pOff < patternLength; pOff++)
        {
            if (mask[pOff] == '?' || pattern[pOff] == current[pOff])
            {
                continue;
            }
            patternFailed = true;
        }
        if (!patternFailed)
        {
            return DWORD(i);
        }
    }

    return 0;
}

DWORD64 kfw::core::Utils::findPattern(const HMODULE module, const char* pattern, const char* mask)
{
    DWORD from = DWORD(module);
    DWORD to = DWORD(module);
    MEMORY_BASIC_INFORMATION meminfo = MEMORY_BASIC_INFORMATION();

    while (VirtualQuery(reinterpret_cast<LPCVOID>(to), &meminfo, sizeof(MEMORY_BASIC_INFORMATION))) {
        if (DWORD(meminfo.AllocationBase) != from)
            break;

        to += meminfo.RegionSize;
    }

    return findPattern(from, to, pattern, mask);
}

DWORD64 kfw::core::Utils::findPattern(const char* module, const char* pattern, const char* mask)
{
    return findPattern(GetModuleHandleA(module), pattern, mask);
}
