//
// Created by konek on 7/15/2019.
//

#ifndef MEDIA_PLAYER_PRO_WINDOWSAPI_H
#define MEDIA_PLAYER_PRO_WINDOWSAPI_H

/*

 PROCESS LIFECYCLE

traditional behaviour of a process lifecycle:
    on a single application system:
        when an application starts up, it executes main()
        when an application shuts down, it terminates main()
    on a multi application system:
        when an application starts up, it executes main()
        when an application goes into background, it pauses main()
        when an application goes into foreground, it resumes main()
        when an application shuts down, it terminates main()

    in Windows:
        when an application starts up, it executes main() then it sends messages to WindowProc() untill
            main() returns or the application is killed

        events:
            application resizing:
                event is sent to WindowProc()
            application minimizing:
                event is sent to WindowProc()
            application dragging/repositioning:
                event is sent to WindowProc()
            application focus gain/loss:
                event is sent to WindowProc()
            application close:
                event is sent to WindowProc()
                NOTE:
                  it is up to the application to decide what to do, thus it is possible to have an
                  unclosable application that can only be terminated by the Task Manager
 */


// message system: GetMessage/PeekMessage/TranslateMessage/DispatchMessage/CallWindowProc/DefWndProc/SendMessage/PostMessage

// implement GetMessage, PeekMessage, SendMessage, and PostMessag using a Dequeue or equivilant

// window functions: RegisterWindowClass / UnregisterWindowClass / CreateWindow / DestroyWindow / SetForegroundWindow / SetWindowPos(resize/move)
// header.h : include file for standard system include files,
// or project specific include files
//

#include "Windows/Kernel/WindowsAPIKernel.h"
#include "Windows/WindowsAPIHander.h"
#include "Windows/WindowsAPIMessage.h"
#include "Windows/WindowsAPIProcess.h"
#include "Windows/WindowsAPIThread.h"

#endif //MEDIA_PLAYER_PRO_WINDOWSAPI_H
