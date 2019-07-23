//
// Created by konek on 7/17/2019.
//

#include <dlfcn.h>
#include <Windows/WindowsAPIProcess.h>
#include <functionPointer.h>
#include <Windows/WindowsAPIThread.h>

WINBASEAPI
        BOOL
WINAPI
CreateProcessA(
        _In_opt_ LPCSTR lpApplicationName,
        _Inout_opt_ LPSTR lpCommandLine,
        _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
        _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
        _In_ BOOL bInheritHandles,
        _In_ DWORD dwCreationFlags,
        _In_opt_ LPVOID lpEnvironment,
        _In_opt_ LPCSTR lpCurrentDirectory,
        _In_ LPSTARTUPINFOA lpStartupInfo,
        _Out_ LPPROCESS_INFORMATION lpProcessInformation
) {
    if (lpProcessInformation == nullptr) return 0;
    PVOID APP = dlopen(lpApplicationName, RTLD_NOW);
    if (APP == nullptr) {
        return 0;
    } else {
        PROCESS_MAIN_STRUCT * PROCESS_MAIN_STRUCT = new ::PROCESS_MAIN_STRUCT;
        PVOID WM = dlsym(APP, "WinMain");
        PVOID WP = dlsym(APP, "WinProc");
        functionPointerAssign4(DWORD, PROCESS_MAIN_STRUCT->MAIN, WM, HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);
        if (PROCESS_MAIN_STRUCT->MAIN == nullptr) return 0;
        functionPointerAssign4(LRESULT, PROCESS_MAIN_STRUCT->WINPROC, WP, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
        PROCESS_MAIN_STRUCT->lpProcessInformation = new PROCESS_INFORMATION;
        lpProcessInformation = PROCESS_MAIN_STRUCT->lpProcessInformation;
        PROCESS_MAIN_STRUCT->lpProcessInformation->hProcess = KERNEL.newHandle(ObjectTypeProcess, PROCESS_MAIN_STRUCT);
        PROCESS_MAIN_STRUCT->lpProcessInformation->dwProcessId = KERNEL.PID_LAST+=4;
        PROCESS_MAIN_STRUCT->lpProcessInformation->hThread = CreateThread(nullptr, 0,PROCESS_MAIN, PROCESS_MAIN_STRUCT,0,&PROCESS_MAIN_STRUCT->lpProcessInformation->dwThreadId);
        return 1;
    }
}

// should we move this into KERNEL?
DWORD PROCESS_MAIN(LPVOID lpParameter) {
    PROCESS_MAIN_STRUCT * app = (PROCESS_MAIN_STRUCT*) lpParameter;
    DWORD ret = app->MAIN(app->lpProcessInformation->hProcess, nullptr, nullptr, 0);
    delete app->lpProcessInformation;
    delete app;
    return ret;
}
