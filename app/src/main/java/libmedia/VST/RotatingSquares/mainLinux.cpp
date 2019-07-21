//
// Created by konek on 7/20/2019.
//

#include "../../libvst/API/VST.h"
#include "SDL.h"
#include <cstdlib>
#include <string.h>

int VST_API_IMPLEMENTATION = VST_API_IMPLEMENTATION_LINUX;

extern Renderer* g_renderer;
extern void printGlString(const char* name, GLenum s);
extern GLboolean gl3stubInit();

CALLBACKNATIVE VST_FUNCTION_CREATE {
    if (g_renderer) {
        delete g_renderer;
        g_renderer = NULL;
    }

    printGlString("Version", GL_VERSION);
    printGlString("Vendor", GL_VENDOR);
    printGlString("Renderer", GL_RENDERER);
    printGlString("Extensions", GL_EXTENSIONS);

    const char* versionStr = (const char*)glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.") && gl3stubInit()) {
        g_renderer = createES3Renderer();
    } else if (strstr(versionStr, "OpenGL ES 2.")) {
        g_renderer = createES2Renderer();
        ALOGE("Unsupported OpenGL ES version");
    } else {
        ALOGE("Unsupported OpenGL ES version");
    }
    return VST_RETURN_CONTINUE;
}

CALLBACKNATIVE VST_FUNCTION_RESIZE {
    if (g_renderer) {
        g_renderer->resize(width, height);
        return VST_RETURN_CONTINUE;
    }
    return VST_RETURN_STOP;
}

CALLBACKNATIVE VST_FUNCTION_DRAW {
    if (g_renderer) {
        g_renderer->render();
        return VST_RETURN_CONTINUE;
    }
    return VST_RETURN_STOP;
}
