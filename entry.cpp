#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "core/hack.h"
#include <thread>

DWORD WINAPI ThreadProc(LPVOID lpParameter) {
    gHack.init();

    while (!GetAsyncKeyState(VK_END)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    gHack.exit();

    FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), EXIT_SUCCESS);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        HANDLE Thread = CreateThread(
            nullptr,
            NULL,
            ThreadProc,
            hModule,
            NULL,
            nullptr
        );
        break;
    }

    return TRUE;
}