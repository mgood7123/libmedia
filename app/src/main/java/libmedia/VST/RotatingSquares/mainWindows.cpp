//
// Created by konek on 7/20/2019.
//

#include "SDL.h"
#include <VST.h>
#include <Windows/windows.h>
#include <cstdlib>
#include <string.h>

int VST_API_IMPLEMENTATION = VST_API_IMPLEMENTATION_WINDOWS;

extern Renderer* g_renderer;
extern void printGlString(const char* name, GLenum s);
extern GLboolean gl3stubInit();

CALLBACKNATIVE int APIENTRY WinMain(_In_ HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_ PWSTR    lpCmdLine,
        _In_ int       nCmdShow)
{
    ALOGV("WinMain Called");
//    MSG msg;
//
//    // Main message loop:
//    ALOGV("starting message loop");
//    while (GetMessage(&msg, nullptr, 0, 0))
//    {
//        ALOGV("dispatching message");
//        DispatchMessage(&msg);
//    }
//    return (int) msg.wParam;
    return 0;
}