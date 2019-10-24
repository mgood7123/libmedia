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

/*
CALLBACKNATIVE LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE: {
            ALOGV("WINDOW CREATE");
            if (g_renderer) {
                delete g_renderer;
                g_renderer = NULL;
            }

            printGlString("Version", GL_VERSION);
            printGlString("Vendor", GL_VENDOR);
            printGlString("Renderer", GL_RENDERER);
            printGlString("Extensions", GL_EXTENSIONS);

            const char *versionStr = (const char *) glGetString(GL_VERSION);
            if (versionStr == nullptr) {
                ALOGE("No OpenGL ES version returned");
                return 0;
            }
            if (strstr(versionStr, "OpenGL ES 3.") && gl3stubInit()) {
                g_renderer = createES3Renderer();
            } else if (strstr(versionStr, "OpenGL ES 2.")) {
                g_renderer = createES2Renderer();
                ALOGE("Unsupported OpenGL ES version");
                return 0;
            } else {
                ALOGE("Unsupported OpenGL ES version");
                return 0;
            }
            return 1;
        }

        case WM_SIZE: {
            if (g_renderer) {
                g_renderer->resize(LOWORD(lParam), HIWORD(lParam));
                return 1;
            }
            return 0;
        }
        default: {
            return 0;//DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
}
*/

CALLBACKNATIVE int APIENTRY WinMain(_In_ HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_ PWSTR    lpCmdLine,
        _In_ int       nCmdShow)
{
    ALOGV("WinMain Called");
    if (g_renderer) {
        delete g_renderer;
        g_renderer = NULL;
    }

    g_renderer = createES3Renderer();
    if (g_renderer == nullptr) ALOGE("failed to create Renderer");
    else while (true) g_renderer->render();
    return 0;
}