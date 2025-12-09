#include "hooks.h"
#include "../dependencies/include/minhook/minhook.h"

void Hooks::init() {
	MH_Initialize();
	
	

	MH_EnableHook(MH_ALL_HOOKS);
}