#include "framework.h"

#include "util.h"

#include "MinHook.h"

#include "curl.h"

#include "pch.h"

EXTERN_C typedef struct EOS_PlatformHandle* EOS_HPlatform;
EXTERN_C typedef struct EOS_AntiCheatClientHandle* EOS_HAntiCheatClient;

EOS_HAntiCheatClient CustomFunction(EOS_HPlatform Handle) {
    Log("Bypassed AC Check!");
    return EOS_HAntiCheatClient();
}
void NOTHING(EOS_HPlatform Handle) {
    Log("Tick Was Called!");
    //return;
}
void Hook() {
    MH_Initialize();
    FILE* a;
#ifdef SENDLOGSTOCONSOLE
    AllocConsole(); // BUG: Console does not get written to without logs enabled.
    freopen_s(&a, ENC("CONOUT$"), "w", stdout);
    freopen_s(&a, ENC("CONOUT$"), "w", stdin);
    freopen_s(&a, ENC("CONOUT$"), "w", stderr);
#endif
#ifdef WRITELOGSTOFILE
    freopen_s(&a, ENC("logs.txt"), "w", stdout); // Writes processevent (logs) to file.
#endif

    checkSig(CurlEasyOptP, ENC("curl_easy_set_opt"));
    checkSig(CurlSetOptP, ENC("curl_set_opt"));

    MH_CreateHook((void*)CurlEasyOptP, changeeasyopt, (void**)&_curl_easy_setopt);

    if (INGAME) {
        LPVOID lpCurlSetopt;
        INT(*CurlSetoptCast)(LPVOID, INT, va_list) = NULL;
        CurlSetoptCast = reinterpret_cast <decltype(CurlSetoptCast)> (CurlSetOptP);
    }

    MH_STATUS status = MH_EnableHook((void*)CurlEasyOptP);
    if (status != MH_OK) {
        const char* StatusStr = MH_StatusToString(status);
        MessageBoxA(0, ENC("Error while hooking!"), ENC("Atomic Fatal Error!"), MB_ICONERROR);
        if (status == MH_ERROR_NOT_INITIALIZED) {
            Log(ENC("Somehow, Minhook was not initialized. The builder of this dll can add \"MH_Initialize();\" at the beginning of the hook function."));
            return; // TODO: Create Exit Function.
        }
        Log(charcmb(ENC("MH Error (report this to devs): "), StatusStr)); // TOOD: Add better error handling, with maybe solutions for the user.
        return;
    }
    HMODULE moduleHandle = GetModuleHandle(L"EOSSDK-Win64-Shipping.dll");
    auto Address = GetProcAddress(moduleHandle, "EOS_Platform_GetAntiCheatClientInterface");
    //auto Addresss = GetProcAddress(moduleHandle, "EOS_Platform_Tick");
    MH_CreateHook(Address, CustomFunction, NULL);
    MH_STATUS statuss = MH_EnableHook(Address);
    if (statuss == MH_OK) {
        Log("Successfully Hooked AntiCheat Check!");
    }
}


void Main() {
    AllocConsole();
    FILE* pFile;
    freopen_s(&pFile, "CONOUT$", "w", stdout);
    system("color 003");
    //Log devname and Welcome
    SetConsoleTitleA("Atomic | discord.gg/fndev");
    const char* ascii = (R"(

    _____   __                 .__        
  /  _  \_/  |_  ____   _____ |__| ____  
 /  /_\  \   __\/  _ \ /     \|  |/ ___\ 
/    |    \  | (  <_> )  Y Y  \  \  \___ 
\____|__  /__|  \____/|__|_|  /__|\___  >
        \/                  \/        \/ 
  )");
    Log(ascii);
    ("\n\n");
    Log("[+] Welcome to Atomic");
    Log("[+] DLL By Psycho");
    Log("[+] DLL Credits: violent and basic");
    Log("[+] Loaded DLL!");
    Sleep(3000);
}


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved
) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        Hook();
        Main();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}