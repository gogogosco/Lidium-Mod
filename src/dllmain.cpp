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
#include <console.hpp>

//Force laptops to use GPU
extern "C"
__declspec(dllexport) DWORD NvOptimusEnablement = 1;
extern "C"
__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;

#define relative_address(frm, to) (int)(((int)to - (int)frm) - 5)

//Maple addresses
static int CUserLocal__UseFuncKeyMapped_Press_Jump = 0x00B58326;
static int IWzProperty__Getitem = 0x00404DA0;
static int CUser__OnChat = 0x00AED430;
static int CWvsContext__GetCharacterData = 0x0042FFE0;

//Double Jump
static int CuserLocal__DoActiveSkill = 0x00B6B450;
static int Jump_Ret = 0x00B584A1;

//IWzProperty__Getitem
static int jmpBack_RenderInvPointer_onFail = 0x00404E37;
static int jmpBack_RenderInvPointer_onOK = 0x00404DCE;

//Game width
static int m_nGameWidth = 1366;

//Discord RPC
std::uint32_t RPC_PlayerName = 0;
std::uint32_t RPC_GetLevel = 0;
std::uint32_t RPC_GetMapName = 0;
std::uint32_t RPC_GetJobID = 600200;

// v83 - needs updating for v111
DWORD call_GetJobID = 0x008D8298;
DWORD call_GetLevel = 0x008D8289;
DWORD jmpBack_MaplePatchGame = 0x008D817D;
DWORD jmpBack_MapleGetMap = 0x00855E10;
//

//v83 - needs updating for v111
__declspec(naked)void ASM_GameRPCGetInfo()
{
    __asm
    {
        or dword ptr[ebp - 04], -01 //not really needed but im including it here anyways.
        ADD ESI, 4  //the playername is 4 bytes over the esi offset.
        MOV DWORD PTR[RPC_PlayerName], ESI
        SUB ESI, 4
        MOV ECX, ESI
        CALL[call_GetJobID]
        movsx eax, ax
        MOV[RPC_GetJobID], EAX
        PUSH EAX
        mov ecx, esi
        CALL[call_GetLevel]
        mov ecx, [ebp - 0x10]
        movzx eax, al
        MOV[RPC_GetLevel], EAX
        PUSH EAX
        JMP[jmpBack_MaplePatchGame]
    }
}

//v83 - needs updating for v111
__declspec(naked)void ASM_GameRPCGetMapInfo()
{
    __asm
    {
        mov eax, [ebp - 0x10] // char pointer of map name here
        MOV[RPC_GetMapName], EAX
        CMP EAX, EBX
        JMP[jmpBack_MapleGetMap]
    }
}


//v83 - needs updating for v111
std::uint32_t jmpBack_OnExitTitleScreen = 0x005F4557;
__declspec(naked)void ASM_GameRPCClearRPCOnExit()
{
    __asm
    {
        MOV DWORD PTR[RPC_PlayerName], 0x0
        MOV DWORD PTR[RPC_GetLevel], 0x0
        MOV DWORD PTR[RPC_GetMapName], 0x0
        MOV DWORD PTR[RPC_GetJobID], 600200
        PUSH 0x00000B4F // "Ver. %d.%d", Text appear on the top right of the game, when you log off.
        JMP[jmpBack_OnExitTitleScreen]
    }
}

DWORD EBPCallAddr = 0;
DWORD ErrorUnkReason = 0;
char Error[256];

__declspec(naked) void ErrorUnk() {
    __asm
    {
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

void TestVeh()
{
    MessageBoxA(0, "VEH Hook triggered!", "VEH Hook triggered!", MB_OK);
}

//todo - add missing jobs
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

    ::AddVectoredExceptionHandler(true, MapleCrashHandler);
    ::SetUnhandledExceptionFilter(MapleCrashHandler);

    console::init();
    //drpc::init();

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

//  Example VEH hook (OriginFunc, (uintptr_t)HookFunc)
//  LeoHook::Hook(0x000000, (uintptr_t)TestVeh);

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