#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include "MapleHooks.h"
#include <ZXString.h>
#include <Common.h>
#include <hooker.h>
#include <memedit.h>
#include <timeapi.h>
#include <LeoSpecial.h>
#include <exception/CrashHandler.h>

//Force laptops to use GPU
extern "C"
__declspec(dllexport) DWORD NvOptimusEnablement = 1;
extern "C"
__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;

#define relative_address(frm, to) (int)(((int)to - (int)frm) - 5)
#define CUserLocal__UseFuncKeyMapped_Press_Jump 0x00B58326

//Double Jump
static int CuserLocal__DoActiveSkill = 0x00B6B450;
static int Jump_Ret = 0x00B584A1;

//IWzProperty::Getitem
static int jmpBack_RenderInvPointer_onFail = 0x00404E37;
static int jmpBack_RenderInvPointer_onOK = 0x00404DCE;

static int m_nGameWidth = 1366;

void testVeh() {
    MessageBoxA(0, "VEH Hook triggered!", "VEH Hook triggered!", MB_OK);
}

// Jump back on null pointer error - credits to F0
// needs adjustment for v111... addresses found
__declspec(naked) void ASM_FixRenderInvPointer() {
    __asm {
        cmp ecx, 0
        je fail
        mov esi, ecx
        and dword ptr[ebp - 04], 00
        jmp[jmpBack_RenderInvPointer_onOK]
        fail:
        jmp[jmpBack_RenderInvPointer_onFail]
    }
}

DWORD EBPCallAddr = 0;
DWORD ErrorUnkReason = 0;
char Error[256];
__declspec(naked) void ErrorUnk() {
    __asm {
        CMP EBP, 0
        JE FAIL
        PUSH EBX
        XOR EBX, EBX
        MOV EBX, DWORD PTR[EBP + 4]
        MOV DWORD PTR[EBPCallAddr], EBX
        MOV EBX, DWORD PTR[ESP + 8]
        MOV DWORD PTR[ErrorUnkReason], EBX
        POP EBX
        JMP START
        FAIL :
        RETN 0x4
            START :
    }
    __asm pushad //Start C++ hack here
    sprintf(Error, "%x occured at address %x, \nMapleStory will continue to run, but you may experience additonal issues.\nIt is recommended that you restart the client.", ErrorUnkReason, EBPCallAddr);
    MessageBoxA(nullptr, Error, "pcom", MB_OK);
    __asm popad //End
    __asm {
        RETN 4
    }
}

int __cdecl is_resistance_job(int a1)
{
    return a1 / 1000 == 3;
}

int __cdecl is_dual_job(int a1)
{
    return a1 / 10 == 43;
}

int __cdecl is_night_lord(int a1)
{
    return a1 / 10 == 41;
}

int __cdecl is_shadower(int a1)
{
    return a1 / 10 == 42;
}

int __cdecl is_night_walker(int a1)
{
    return a1 / 100 == 14;
}

int __cdecl is_dawn_warrior(int a1)
{
    return a1 / 100 == 11;
}

int __cdecl is_aran(int a1)
{
    return a1 / 100 == 11;
}

//Enable Double Jump by MiLin
void __declspec(naked) EnableDoubleJump()
{
    __asm
    {
        jnz    JAGUAR_JUMP
        jz     CHECK_IF_DB
        jmp    Jump_Ret

        JAGUAR_JUMP :

            push    0
            push    0
            push    0
            push    0
            push    0
            push[33001002]
            mov     ecx, esi
            call    CuserLocal__DoActiveSkill

            CHECK_IF_DB :

        mov     edx, [esi]
            mov     eax, [edx + 88]
            mov     ecx, esi
            call    eax
            push    eax
            call    is_dual_job
            add     esp, 4
            test    eax, eax
            jz      CHECK_IF_NL
            push    0
            push    0
            push    0
            push    0
            push    0
            push[4321003]
            mov     ecx, esi
            call    CuserLocal__DoActiveSkill

            CHECK_IF_NL :

        mov     edx, [esi]
            mov     eax, [edx + 88]
            mov     ecx, esi
            call    eax
            push    eax
            call    is_night_lord
            add     esp, 4
            test    eax, eax
            jz      CHECK_IF_SD
            push    0
            push    0
            push    0
            push    0
            push    0
            push[4111006]
            mov     ecx, esi
            call    CuserLocal__DoActiveSkill

            CHECK_IF_SD :

        mov     edx, [esi]
            mov     eax, [edx + 88]
            mov     ecx, esi
            call    eax
            push    eax
            call    is_shadower
            add     esp, 4
            test    eax, eax
            jz      CHECK_IF_NW
            push    0
            push    0
            push    0
            push    0
            push    0
            push[4211009]
            mov     ecx, esi
            call    CuserLocal__DoActiveSkill

            CHECK_IF_NW :

        mov     edx, [esi]
            mov     eax, [edx + 88]
            mov     ecx, esi
            call    eax
            push    eax
            call    is_night_walker
            add     esp, 4
            test    eax, eax
            jz      DB_JUMP
            push    0
            push    0
            push    0
            push    0
            push    0
            push[14101004]
            mov     ecx, esi
            call    CuserLocal__DoActiveSkill
            jmp     Jump_Ret

            DB_JUMP :
        jmp    Jump_Ret
    }
}

void init() {
	::AllocConsole();::SetConsoleTitleA("MapleStory Console");
	 
	std::freopen("CONOUT$", "w", stdout);
	std::freopen("CONIN$", "r", stdin);
	 
	::AddVectoredExceptionHandler(true, MapleCrashHandler);
	::SetUnhandledExceptionFilter(MapleCrashHandler);

	SetProcessAffinityMask(GetCurrentProcess(), 0); // Set amount of processors to maximum
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS); // Set priority to realtime
}

// executed after the client is unpacked
VOID MainFunc()
{

    //Enable Double Jump by MiLin
	*(unsigned char*)CUserLocal__UseFuncKeyMapped_Press_Jump = 0xE9;
	*(unsigned long*)(CUserLocal__UseFuncKeyMapped_Press_Jump + 1) = relative_address(CUserLocal__UseFuncKeyMapped_Press_Jump, EnableDoubleJump);

	// Allow foreign characters in chat - credits to yeehaw and sonkub
    // 
	// CWndMan::TranslateMessage
	 PatchNop(0x00BEEE6D, 9);
	// CWndMan::EnableIME
	 PatchNop(0x00BEE769, 2);
	// IsDBCSLeadByte 1
	 WriteValue(0x0058F990, (char)0xEB);
	// IsDBCSLeadByte 2
	 WriteValue(0x00A5180F, (char)0xEB);
	// IsDBCSLeadByte 3
	 WriteValue(0x00AED532, (char)0xEB);
	// IsDBCSLeadByte 4
	 WriteValue(0x00B3C7E2, (char)0xEB);
	// IsDBCSLeadByte 5
	 WriteValue(0x00C3CAF8, (char)0xEB);

//  Create Legends Button by MiLin
	WriteValue(0x684BB0 + 1, 9999);
	PatchJmp(0x684BA9, 0x684BE7);

//	Adjust clickable width by Ozzy
	WriteValue(0xC04B56 + 1, m_nGameWidth);

//  Attempt to ignore invalid pointers from WZ. Does not work as of now...
//  PatchCall(0x00404DC4, ASM_FixRenderInvPointer);

//  Example VEH hook (OriginFunc, (uintptr_t)HookFunc)
//  LeoHook::Hook(0x000001, (uintptr_t)testVeh);

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