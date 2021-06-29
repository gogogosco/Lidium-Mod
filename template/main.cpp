#include <main.hpp>

#include <../include/hook/hook.hpp>

#include <../include/config/config.hpp>

#include <../include/detours/detours.h>

#include <timeapi.h>

#include <process.h>

#include <../include/exception/CrashHandler.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Detours.lib")

//Force laptops to use GPU
extern "C"
__declspec(dllexport) DWORD NvOptimusEnablement = 1;
extern "C"
__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;

//Credits to F0

namespace mm_template {
    void main() {
        ::AddVectoredExceptionHandler(true, OurCrashHandler);::SetUnhandledExceptionFilter(OurCrashHandler);

        load_patches();

        if (config::CPUMaxPerformance == 1) {
            SetProcessAffinityMask(GetCurrentProcess(), 0); // Set amount of processors to maximum
            SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS); // Set priority to realtime
        }
    }

    DWORD g_getTickCount = GetTickCount();
    DWORD(__stdcall * g_real_GetTickCount)
        (void) = GetTickCount;

    DWORD g_timeGetTime = timeGetTime();
    DWORD(__stdcall * g_real_timeGetTime)
        (void) = timeGetTime;

    DWORD corrected_GetTickCount() {
        return (g_real_GetTickCount() - g_getTickCount) % INT32_MAX;
    }

    DWORD corrected_timeGetTime() {
        return (g_real_timeGetTime() - g_timeGetTime) % INT32_MAX;
    }
    void timeGetTime_timeFix() {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(reinterpret_cast < void ** > ( & g_real_GetTickCount), corrected_GetTickCount);
        DetourAttach(reinterpret_cast < void ** > ( & g_real_timeGetTime), corrected_timeGetTime);
        DetourTransactionCommit();
    }

    std::uint32_t jmpBack_RenderInvPointer_onFail = 0x00403999;
    std::uint32_t jmpBack_RenderInvPointer_onOK = 0x0040394E;
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
            FAIL:
                RETN 0x4
            START:
        }
        __asm pushad //Start C++ hack here
        sprintf(Error, "%x occured at address %x, \nMapleStory will continue to run, but you may experince additonal issues.\nIt is recommended that you restart the client.", ErrorUnkReason, EBPCallAddr);
        MessageBoxA(nullptr, Error, "pcom", MB_OK);
        __asm popad //End
        __asm {
            RETN 4
        }
    }

    unsigned int JenkinsOneAtATimeCompute(const char * key, size_t len) {
        unsigned int hash, i;
        for (hash = i = 0; i < len; ++i) {
            hash += key[i];
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        return hash;
    }

    void BlockAPI(LPCSTR lpModule, LPCSTR lpFuncName, int iType) {
        DWORD pPrevious, pPrevious2;
        DWORD dwAddr = 0;
        dwAddr = (DWORD) GetProcAddress(GetModuleHandleA(lpModule), lpFuncName);
        if (!dwAddr) {
            return;
        }
        if (iType == 0) {
            BYTE ret[6] = {
                0xB8,
                0x00,
                0x00,
                0x00,
                0x00,
                0xC3
            };
            VirtualProtect((LPVOID) dwAddr, 6, PAGE_EXECUTE_READWRITE, & pPrevious);
            memcpy((void * )(dwAddr), & ret, 6);
            VirtualProtect((LPVOID) dwAddr, 6, pPrevious, & pPrevious2);
        } else if (iType == 1) {
            BYTE ret[1] = {
                0x90
            };
            VirtualProtect((LPVOID) dwAddr, 1, PAGE_EXECUTE_READWRITE, & pPrevious);
            memcpy((void * )(dwAddr), & ret, 1);
            VirtualProtect((LPVOID) dwAddr, 1, pPrevious, & pPrevious2);
        } else {
            return;
        }
    }

    bool HasHardwareBreakpoints() {
        CONTEXT ctx = {
            0
        };
        ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
        auto hThread = GetCurrentThread();

        if (GetThreadContext(hThread, & ctx) == 0)
            return false;
        else
            return (ctx.Dr0 != 0 || ctx.Dr1 != 0 || ctx.Dr2 != 0 || ctx.Dr3 != 0);
    }

    void load_patches() {
        //example jump patching
        //utils::hook::jump(0x00A5FDE4, ErrorUnk, 5); //pcom error handle
        //utils::hook::jump(0x00403948, ASM_FixRenderInvPointer, 5);

        //example byte writing
        //utils::hook::write(0x00A5FDE4, { 0xc2,0x01,0x02 });
        //utils::hook::write(0x00403AE9, { 0x00 });
    }
}