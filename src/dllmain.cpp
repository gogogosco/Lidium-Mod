#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"

#include "ExampleHooks.h"
#include <ZXString.h>
#include <Common.h>
#include <hooker.h>
#include <memedit.h>
#include <timeapi.h>

//Force laptops to use GPU
extern "C"
__declspec(dllexport) DWORD NvOptimusEnablement = 1;
extern "C"
__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;

void init() {
	::AllocConsole();::SetConsoleTitleA("MapleStory Console");

	std::freopen("CONOUT$", "w", stdout);
	std::freopen("CONIN$", "r", stdin);

	printf("DLL_PROCESS_ATTACH\n");
	
	//::AddVectoredExceptionHandler(true, OurCrashHandler);
	//::SetUnhandledExceptionFilter(OurCrashHandler);

	SetProcessAffinityMask(GetCurrentProcess(), 0); // Set amount of processors to maximum
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS); // Set priority to realtime
}

// executed after the client is unpacked
VOID MainFunc()
{
	int m_nGameWidth = 1366;

	printf(__FUNCTION__);
	printf("\n");

//  Create Legends Button by MiLin
	WriteValue(0x684BB0 + 1, 9999);
	PatchJmpRaj(0x684BA9, 0x684BE7);

//	Adjust clickable width by Ozzy
	WriteValue(0xC04B56 + 1, m_nGameWidth);

	return;
}

// prolly don't edit this region if youre a noob
#pragma region EntryThread

// main thread
VOID MainProc()
{
	printf(__FUNCTION__);
	printf("\n");

	MainFunc();
}

// dll entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		init();
		DisableThreadLibraryCalls(hModule);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&MainProc, NULL, 0, 0);
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		printf("DLL_PROCESS_DETACH");
		Common::GetInstance()->~Common();
		break;
	}
	}
	return TRUE;
}

#pragma endregion