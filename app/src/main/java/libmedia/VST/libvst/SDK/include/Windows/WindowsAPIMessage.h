//
// Created by konek on 7/16/2019.
//

#ifndef MEDIA_PLAYER_PRO_WINDOWSAPIMESSAGE_H
#define MEDIA_PLAYER_PRO_WINDOWSAPIMESSAGE_H

#include "windows.h"

typedef struct tagMSG {
    HWND   hwnd;
    UINT   message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD  time;
    POINT  pt;
    DWORD  lPrivate;
    // TODO: move these into seperate API's
    BOOL mice; // true if a mice is connected, otherwise false
    BOOL keyboard; // true if a keyboard is connected, otherwise false
} MSG, *PMSG, *NPMSG, *LPMSG;

/*
 * Message Function Templates
 */

extern
WINUSERAPI
        BOOL
WINAPI
GetMessageA(
        _Out_ LPMSG lpMsg,
        _In_opt_ HWND hWnd,
        _In_ UINT wMsgFilterMin,
        _In_ UINT wMsgFilterMax);
extern
WINUSERAPI
        BOOL
WINAPI
GetMessageW(
        _Out_ LPMSG lpMsg,
        _In_opt_ HWND hWnd,
        _In_ UINT wMsgFilterMin,
        _In_ UINT wMsgFilterMax);

__inline
BOOL
GetMessage(
    LPMSG lpMsg,
    HWND hWnd,
    UINT wMsgFilterMin,
    UINT wMsgFilterMax
    )
{
#ifdef UNICODE
    return GetMessageW(
#else
    return GetMessageA(
#endif
        lpMsg,
        hWnd,
        wMsgFilterMin,
        wMsgFilterMax
        );
}

extern
WINUSERAPI
        BOOL
WINAPI
TranslateMessage(
        _In_ CONST MSG *lpMsg);

extern
WINUSERAPI
        LRESULT
WINAPI
DispatchMessageA(
        _In_ CONST MSG *lpMsg);
extern
WINUSERAPI
        LRESULT
WINAPI
DispatchMessageW(
        _In_ CONST MSG *lpMsg);
__inline
LRESULT
DispatchMessage(
    CONST MSG *lpMsg
    )
{
#ifdef UNICODE
    return DispatchMessageW(
#else
    return DispatchMessageA(
#endif
        lpMsg
        );
}

extern
WINUSERAPI
        BOOL
WINAPI
SetMessageQueue(
        _In_ int cMessagesMax);

extern
WINUSERAPI
        BOOL
WINAPI
PeekMessageA(
        _Out_ LPMSG lpMsg,
        _In_opt_ HWND hWnd,
        _In_ UINT wMsgFilterMin,
        _In_ UINT wMsgFilterMax,
        _In_ UINT wRemoveMsg);
extern
WINUSERAPI
        BOOL
WINAPI
PeekMessageW(
        _Out_ LPMSG lpMsg,
        _In_opt_ HWND hWnd,
        _In_ UINT wMsgFilterMin,
        _In_ UINT wMsgFilterMax,
        _In_ UINT wRemoveMsg);
#ifdef UNICODE
#define PeekMessage  PeekMessageW
#else
#define PeekMessage  PeekMessageA
#endif // !UNICODE

/*
 * PeekMessage() Options
 */
#define PM_NOREMOVE         0x0000
#define PM_REMOVE           0x0001
#define PM_NOYIELD          0x0002


#if(WINVER >= 0x0500)
#define PM_QS_INPUT         (QS_INPUT << 16)
#define PM_QS_POSTMESSAGE   ((QS_POSTMESSAGE | QS_HOTKEY | QS_TIMER) << 16)
#define PM_QS_PAINT         (QS_PAINT << 16)
#define PM_QS_SENDMESSAGE   (QS_SENDMESSAGE << 16)
#endif /* WINVER >= 0x0500 */

extern
WINUSERAPI
BOOL
WINAPI
PostMessageA(
        _In_opt_ HWND hWnd,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam);
extern
WINUSERAPI
BOOL
WINAPI
PostMessageW(
        _In_opt_ HWND hWnd,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam);
#ifdef UNICODE
#define PostMessage  PostMessageW
#else
#define PostMessage  PostMessageA
#endif // !UNICODE

extern
WINUSERAPI
BOOL
WINAPI
PostThreadMessageA(
        _In_ DWORD idThread,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam);
extern
WINUSERAPI
BOOL
WINAPI
PostThreadMessageW(
        _In_ DWORD idThread,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam);
#ifdef UNICODE
#define PostThreadMessage  PostThreadMessageW
#else
#define PostThreadMessage  PostThreadMessageA
#endif // !UNICODE

extern
WINUSERAPI
LPARAM
WINAPI
SetMessageExtraInfo(
        _In_ LPARAM lParam);

extern
WINUSERAPI
LRESULT
WINAPI
SendMessageA(
        _In_ HWND hWnd,
        _In_ UINT Msg,
        _Pre_maybenull_ _Post_valid_ WPARAM wParam,
        _Pre_maybenull_ _Post_valid_ LPARAM lParam);
extern
WINUSERAPI
LRESULT
WINAPI
SendMessageW(
        _In_ HWND hWnd,
        _In_ UINT Msg,
        _Pre_maybenull_ _Post_valid_ WPARAM wParam,
        _Pre_maybenull_ _Post_valid_ LPARAM lParam);
__inline
LRESULT
SendMessage(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam
    )
{
#ifdef UNICODE
    return SendMessageW(
#else
    return SendMessageA(
#endif
        hWnd,
        Msg,
        wParam,
        lParam
        );
}



extern
WINUSERAPI
LRESULT
WINAPI
SendMessageTimeoutA(
        _In_ HWND hWnd,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam,
        _In_ UINT fuFlags,
        _In_ UINT uTimeout,
        _Out_opt_ PDWORD_PTR lpdwResult);
extern
WINUSERAPI
LRESULT
WINAPI
SendMessageTimeoutW(
        _In_ HWND hWnd,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam,
        _In_ UINT fuFlags,
        _In_ UINT uTimeout,
        _Out_opt_ PDWORD_PTR lpdwResult);
#ifdef UNICODE
#define SendMessageTimeout  SendMessageTimeoutW
#else
#define SendMessageTimeout  SendMessageTimeoutA
#endif // !UNICODE

extern
WINUSERAPI
BOOL
WINAPI
SendNotifyMessageA(
        _In_ HWND hWnd,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam);
extern
WINUSERAPI
BOOL
WINAPI
SendNotifyMessageW(
        _In_ HWND hWnd,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam);
#ifdef UNICODE
#define SendNotifyMessage  SendNotifyMessageW
#else
#define SendNotifyMessage  SendNotifyMessageA
#endif // !UNICODE

extern
WINUSERAPI
BOOL
WINAPI
SendMessageCallbackA(
        _In_ HWND hWnd,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam,
        _In_ SENDASYNCPROC lpResultCallBack,
        _In_ ULONG_PTR dwData);
extern
WINUSERAPI
BOOL
WINAPI
SendMessageCallbackW(
        _In_ HWND hWnd,
        _In_ UINT Msg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam,
        _In_ SENDASYNCPROC lpResultCallBack,
        _In_ ULONG_PTR dwData);
#ifdef UNICODE
#define SendMessageCallback  SendMessageCallbackW
#else
#define SendMessageCallback  SendMessageCallbackA
#endif // !UNICODE

typedef struct {
    UINT  cbSize;
    HDESK hdesk;
    HWND  hwnd;
    LUID  luid;
} BSMINFO, *PBSMINFO;

extern
WINUSERAPI
long
WINAPI
BroadcastSystemMessageExA(
    _In_ DWORD flags,
    _Inout_opt_ LPDWORD lpInfo,
    _In_ UINT Msg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    _Out_opt_ PBSMINFO pbsmInfo);
extern
WINUSERAPI
long
WINAPI
BroadcastSystemMessageExW(
    _In_ DWORD flags,
    _Inout_opt_ LPDWORD lpInfo,
    _In_ UINT Msg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    _Out_opt_ PBSMINFO pbsmInfo);
#ifdef UNICODE
#define BroadcastSystemMessageEx  BroadcastSystemMessageExW
#else
#define BroadcastSystemMessageEx  BroadcastSystemMessageExA
#endif // !UNICODE


#endif //MEDIA_PLAYER_PRO_WINDOWSAPIMESSAGE_H
