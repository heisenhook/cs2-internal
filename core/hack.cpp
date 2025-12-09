#include "hack.h"

#include "interfaces.h"

#include "../sdk/util/defines.h"
#include <minhook\minhook.h>

void Hack::init() {
	{
		gInterfaces.addModule(X("tier0.dll"));
		gInterfaces.addModule(X("schemasystem.dll"));
	}
	gInterfaces.init();

	gInterfaces.findInterface(HASH("SchemaSystem_00"));

	MH_Initialize();
	{	

	}
}

void Hack::exit() {
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
	gInterfaces.exit();
}