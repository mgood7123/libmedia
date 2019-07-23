    //
// Created by konek on 7/16/2019.
//

#include <cstddef>
#include <Windows/WindowsAPIMessage.h>
#include <Windows/WindowsAPIProcess.h>
#include <deque>

std::deque<struct tagMSG> WINDOWS_MESSAGE_QUEUE;

// IGNORE UNICODE FOR NOW

WINUSERAPI
        BOOL
WINAPI
GetMessageA(
        _Out_ LPMSG lpMsg,
        _In_opt_ HWND hWnd,
        _In_ UINT wMsgFilterMin,
        _In_ UINT wMsgFilterMax) {
    if (!WINDOWS_MESSAGE_QUEUE.empty()) {
        if (((wMsgFilterMin == 0) && (wMsgFilterMax == 0)) || (WINDOWS_MESSAGE_QUEUE.front().message == WM_QUIT)) {
            *lpMsg = WINDOWS_MESSAGE_QUEUE.front();
            WINDOWS_MESSAGE_QUEUE.pop_front();
            return 1;
        }
        else if ((WINDOWS_MESSAGE_QUEUE.front().message <= wMsgFilterMin) && (WINDOWS_MESSAGE_QUEUE.front().message >= wMsgFilterMax)) {
            *lpMsg = WINDOWS_MESSAGE_QUEUE.front();
            WINDOWS_MESSAGE_QUEUE.pop_front();
            return 1;
        }
        else return 0;
    }
    else {
        // wait for message
        while(WINDOWS_MESSAGE_QUEUE.empty()) {}
        return GetMessageA(lpMsg, hWnd, wMsgFilterMax, wMsgFilterMin);
    }
}

WINUSERAPI
        BOOL
WINAPI
GetMessageW(
        _Out_ LPMSG lpMsg,
        _In_opt_ HWND hWnd,
        _In_ UINT wMsgFilterMin,
        _In_ UINT wMsgFilterMax) {
    return GetMessageA(lpMsg, hWnd, wMsgFilterMax, wMsgFilterMin);
}

WINUSERAPI
BOOL
WINAPI
TranslateMessage(
        _In_ CONST MSG *lpMsg) {
        switch (lpMsg->message) {
            // If the message is WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, or WM_SYSKEYUP,
            // the return value is nonzero, regardless of the translation.
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
                return 1;

            // If the message is translated (that is, a character message is posted to
            // the thread's message queue), the return value is nonzero.

            // do something

            // If the message is not translated (that is, a character message is not
            // posted to the thread's message queue), the return value is zero.

            default:
                return 0;
        }
}

WINUSERAPI
LRESULT
WINAPI
DispatchMessageA(
        _In_ CONST MSG *lpMsg) {
    return SendMessageA(lpMsg->hwnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
}

WINUSERAPI
LRESULT
WINAPI
DispatchMessageW(
        _In_ CONST MSG *lpMsg) {
    return DispatchMessageA(lpMsg);
}

WINUSERAPI
BOOL
WINAPI
SetMessageQueue(
        _In_ int cMessagesMax) {
    // does NOTHING in windows 10! o.o
    return 1;
}

WINUSERAPI
BOOL
WINAPI
PeekMessageA(
        _Out_ LPMSG lpMsg,
        _In_opt_ HWND hWnd,
        _In_ UINT wMsgFilterMin,
        _In_ UINT wMsgFilterMax,
        _In_ UINT wRemoveMsg) {
    if (!WINDOWS_MESSAGE_QUEUE.empty()) {
        if (((wMsgFilterMin == 0) && (wMsgFilterMax == 0)) || (WINDOWS_MESSAGE_QUEUE.front().message == WM_QUIT)) {
            *lpMsg = WINDOWS_MESSAGE_QUEUE.front();
            if (wRemoveMsg & PM_REMOVE) WINDOWS_MESSAGE_QUEUE.pop_front();
            return 1;
        }
        else if ((WINDOWS_MESSAGE_QUEUE.front().message <= wMsgFilterMin) && (WINDOWS_MESSAGE_QUEUE.front().message >= wMsgFilterMax)) {
            *lpMsg = WINDOWS_MESSAGE_QUEUE.front();
            if (wRemoveMsg & PM_REMOVE) WINDOWS_MESSAGE_QUEUE.pop_front();
            return 1;
        }
        else return 0;
    }
    else return 0;
}
WINUSERAPI
BOOL
WINAPI
PeekMessageW(
        _Out_ LPMSG lpMsg,
        _In_opt_ HWND hWnd,
        _In_ UINT wMsgFilterMin,
        _In_ UINT wMsgFilterMax,
        _In_ UINT wRemoveMsg) {
    return PeekMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
}

WINUSERAPI
BOOL
WINAPI
PostMessageA(
        _In_opt_ HWND hWnd,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam) {
    if (hWnd == NULL) PostThreadMessageA(0, Msg, wParam, lParam);
    else {
        struct tagMSG a = {0};
        a.hwnd = hWnd;
        a.message = Msg;
        a.lParam = lParam;
        a.wParam = wParam;
        WINDOWS_MESSAGE_QUEUE.push_front(a);
    }
    return 1;
}
WINUSERAPI
BOOL
WINAPI
PostMessageW(
        _In_opt_ HWND hWnd,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam) {
    return PostMessageA(hWnd, Msg, wParam, lParam);
}
#ifdef UNICODE
#define PostMessage  PostMessageW
#else
#define PostMessage  PostMessageA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
PostThreadMessageA(
        _In_ DWORD idThread,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam) {
    // TODO: implement Threads
    return 0;
}
WINUSERAPI
BOOL
WINAPI
PostThreadMessageW(
        _In_ DWORD idThread,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam) {
    return  PostThreadMessageA(idThread, Msg, wParam, lParam);
}
#ifdef UNICODE
#define PostThreadMessage  PostThreadMessageW
#else
#define PostThreadMessage  PostThreadMessageA
#endif // !UNICODE

WINUSERAPI
LPARAM
WINAPI
SetMessageExtraInfo(
        _In_ LPARAM lParam) {
    // TODO
    return lParam;
}

WINUSERAPI
LRESULT
WINAPI
SendMessageA(
        _In_ HWND hWnd,
        _In_ UINT Msg,
        _Pre_maybenull_ _Post_valid_ WPARAM wParam,
        _Pre_maybenull_ _Post_valid_ LPARAM lParam) {
    if (static_cast<PROCESS_MAIN_STRUCT*>(hWnd)->WINPROC == nullptr) return 0;
    return static_cast<PROCESS_MAIN_STRUCT*>(hWnd)->WINPROC(hWnd, Msg, wParam, lParam);
}
WINUSERAPI
LRESULT
WINAPI
SendMessageW(
        _In_ HWND hWnd,
        _In_ UINT Msg,
        _Pre_maybenull_ _Post_valid_ WPARAM wParam,
        _Pre_maybenull_ _Post_valid_ LPARAM lParam) {
    return SendMessageA(hWnd, Msg, wParam, lParam);
}
