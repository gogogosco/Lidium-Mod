#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include "ExampleHooks.h"
#include <cstdint>

// define your hook functions

namespace MapleHooks
{

    // Jump back on null pointer error - credits to F0 and whoever updates these addresses

    std::uint32_t jmpBack_RenderInvPointer_onFail = 0x00403999; // needs adjustment
    std::uint32_t jmpBack_RenderInvPointer_onOK = 0x0040394E; //needs adjustment
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
        sprintf(Error, "%x occured at address %x, \nMapleStory will continue to run, but you may experince additonal issues.\nIt is recommended that you restart the client.", ErrorUnkReason, EBPCallAddr);
        MessageBoxA(nullptr, Error, "pcom", MB_OK);
        __asm popad //End
        __asm {
            RETN 4
        }
    }

	/// <summary>
	/// Example regular cdecl function hook
	/// </summary>
	/// <param name="pArg1">First arg</param>
	/// <param name="nArg2">second arg</param>
	/// <returns>Nothing</returns>

	void __cdecl ExampleCDecl_Hook(void* pArg1, int nArg2)
	{
		Log("Detour triggered!");
		// we only want to execute this function if nArg2 is more than 420, otherwise we return without doing anything
		if (nArg2 > 420)
		{
			_ExampleFunc_cdecl(pArg1, nArg2);
		}
	}

	/// <summary>
	/// Example member function call hook
	/// </summary>
	/// <param name="pThis">Member object</param>4
	/// <param name="edx">Dummy parameter because we are swapping calling conventions.</param>
	/// <param name="nArg1">Some value</param>
	/// <returns>An integer</returns>
	
	int __fastcall ExampleFunc_thiscall2(void* pThis, void* edx, int nArg1)
	{
		Log("Detour func called, arg1: %d", nArg1);
		if (nArg1 == 420)
		{
			nArg1 = 69; // possible to change args or do whatever
			Log("Arg changed to 69");
		}
		return _ExampleFunc_thiscall(pThis, NULL, nArg1); // second argument should be null because of the __thiscall -> __fastcall conversion
	}

}