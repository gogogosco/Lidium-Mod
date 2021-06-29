#include "pch.h"
#include "ExampleHooks.h"
#include <ZXString.h>
#include <Common.h>
#include <hooker.h>
#include <memedit.h>


// executed after the client is unpacked
VOID MainFunc()
{
	Log(__FUNCTION__);

	// Create Legends Button by MiLin
	// WriteValue(0x684BB0 + 1, 9999);
	// PatchJmpRaj(0x684BA9, 0x684BE7);

	return;
}

// don't edit this region if you are not familiar
#pragma region EntryThread

// main thread
VOID MainProc()
{
	Log(__FUNCTION__);
	MainFunc();
}

// dll entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
		Log("DLL_PROCESS_ATTACH");
		DisableThreadLibraryCalls(hModule);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&MainProc, NULL, 0, 0);
		break;
		}
		case DLL_PROCESS_DETACH:
		{
		Log("DLL_PROCESS_DETACH");
		Common::GetInstance()->~Common();
		break;
		}
	}
	return TRUE;
}

#pragma endregion