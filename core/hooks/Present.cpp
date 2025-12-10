#include "Present.h"
#include "..\hack.h"

#include <util\defines.h>

#include <d3d11.h>

#include "..\..\menu\menu.h"
#include "..\..\menu\imgui\imgui.h"
#include "..\..\menu\imgui\imgui_impl_dx11.h"
#include "..\..\menu\imgui\imgui_impl_win32.h"

inline bool init = false;

HRESULT __stdcall Present(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags) {
    if (!gHack.pRTV)
        gHack.CreateRenderTarget();

    gHack.pContext->OMSetRenderTargets(1, &gHack.pRTV, nullptr);

    if (!init) {
        gMenu.init();
        init = true;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Do the funny drawing stuff here (menu, visuals, etc.)
    {
        gMenu.OnFrame();
    }

    ImGui::EndFrame();
    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return hkPresent.original()(pSwapChain, uSyncInterval, uFlags);
}
