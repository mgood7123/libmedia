//
// Created by konek on 7/17/2019.
//

#include <dlfcn.h>
#include "WindowsAPIProcess.h"
#include "../functionPointer.h"
#include "WindowsAPIThread.h"

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
    // load process
    // create a new thread
    // invoke main() in a new thread
    // return
    PVOID APP = dlopen(lpApplicationName, RTLD_NOW);
    if (APP == nullptr) {
        return 0;
    } else {
        __MAIN_STRUCT __MAIN__STRUCT;
        functionPointerAssign4(DWORD, __MAIN__STRUCT.MAIN, dlsym(APP, "WinMain"), HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);
        functionPointerAssign4(LRESULT, __MAIN__STRUCT.WINPROC, dlsym(APP, "WinProc"), HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
        __MAIN__STRUCT.lpProcessInformation = lpProcessInformation;
        lpProcessInformation->hProcess = KERNEL.newHandle(ObjectTypeProcess, &__MAIN__STRUCT);
        lpProcessInformation->dwProcessId = KERNEL.PID_LAST+=4;
        lpProcessInformation->hThread = CreateThread(nullptr, 0,__MAIN, &__MAIN__STRUCT,0,&lpProcessInformation->dwThreadId);
        return 1;
    }
}

// should we move this into KERNEL?
DWORD __MAIN(LPVOID lpParameter) {
    struct __MAIN_STRUCT * app = (struct __MAIN_STRUCT*) lpParameter;
    return app->MAIN(app->lpProcessInformation->hProcess, nullptr, nullptr, 0);
}
