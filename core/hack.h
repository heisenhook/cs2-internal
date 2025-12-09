#pragma once

class Hack {
public:
	void init();
	void exit();
}; inline Hack gHack;

// Example hook definition
// extern HRESULT __stdcall Present(IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
// inline Hook<decltype(&Present)> hkPresent = {};
//
// Example hook usage
// hkPresent.detour(gMemory.get_method(device, 17), Present);