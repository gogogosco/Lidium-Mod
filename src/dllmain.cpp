#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"

//#include "MapleHooks.h"
#include <ZXString.h>
#include <Common.h>
#include <hooker.h>
#include <memedit.h>
#include <timeapi.h>
#include <exception/CrashHandler.h>

//Force laptops to use GPU
extern "C"
__declspec(dllexport) DWORD NvOptimusEnablement = 1;
extern "C"
__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;

void init() {
	//::AllocConsole();::SetConsoleTitleA("MapleStory Console");
	 
	//std::freopen("CONOUT$", "w", stdout);
	//std::freopen("CONIN$", "r", stdin);
	 
	::AddVectoredExceptionHandler(true, MapleCrashHandler);
	::SetUnhandledExceptionFilter(MapleCrashHandler);

	SetProcessAffinityMask(GetCurrentProcess(), 0); // Set amount of processors to maximum
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS); // Set priority to realtime
}

// executed after the client is unpacked
VOID MainFunc()
{
	int m_nGameWidth = 1366;
	// Allow foreign characters in chat - credits to yeehaw and sonkub
	// CWndMan::TranslateMessage
	// PatchNop(0x00BEEE6D, 9);
	// CWndMan::EnableIME
	// PatchNop(0x00BEE769, 2);
	// IsDBCSLeadByte 1
	// WriteValue(0x0058F990, (char)0xEB);
	// IsDBCSLeadByte 2
	// WriteValue(0x00A5180F, (char)0xEB);
	// IsDBCSLeadByte 3
	// WriteValue(0x00AED532, (char)0xEB);
	// IsDBCSLeadByte 4
	// WriteValue(0x00B3C7E2, (char)0xEB);
	// IsDBCSLeadByte 5
	// WriteValue(0x00C3CAF8, (char)0xEB);

//  Create Legends Button by MiLin
	WriteValue(0x684BB0 + 1, 9999);
	PatchJmp(0x684BA9, 0x684BE7);

//	Adjust clickable width by Ozzy
	WriteValue(0xC04B56 + 1, m_nGameWidth);

	return;
}

// prolly don't edit this region if youre a noob
#pragma region EntryThread

// main thread
VOID MainProc()
{
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
		Common::GetInstance()->~Common();
		break;
	}
	}
	return TRUE;
}

#pragma endregion