#pragma once
#include <util\hook.h>

class IDXGISwapChain;
extern HRESULT __stdcall Present(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags);
inline Hook<decltype(&Present)> hkPresent = {};