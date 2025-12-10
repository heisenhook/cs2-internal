#include "menu.h"

#include <wtypes.h>
#include <util\defines.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "..\core\hack.h"


void Menu::OnFrame() {
    ImGui::Begin(X("Test"));
    {
        bool value = true;
        ImGui::Text(X("text"));
        ImGui::Checkbox(X("checkbox"), &value);
    }
    ImGui::End();
}

static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
    const auto MainWindow = [handle]()
        {
            return GetWindow(handle, GW_OWNER) == nullptr &&
                IsWindowVisible(handle) && handle != GetConsoleWindow();
        };

    DWORD nPID = 0;
    GetWindowThreadProcessId(handle, &nPID);

    if (GetCurrentProcessId() != nPID || !MainWindow())
        return TRUE;

    *reinterpret_cast<HWND*>(lParam) = handle;
    return FALSE;
}

void Menu::init() {

	while (hwnd == nullptr)
	{
		EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&hwnd));
		::Sleep(200U);
	}

	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(gHack.pDevice, gHack.pContext);

	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
}

void Menu::exit() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}