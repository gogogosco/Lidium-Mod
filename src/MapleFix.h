#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include "ExampleHooks.h"
#include <cstdint>
#include <stdio.h>

// This is a collection of fixes for various client related issues

namespace MapleFix
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

}