#pragma once

class Menu {
public:
	void* hwnd;
public:
	void OnFrame();

	void init();
	void exit();
}; inline Menu gMenu;