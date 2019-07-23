//
// Created by konek on 7/16/2019.
//

#ifndef MEDIA_PLAYER_PRO_WINDOWSAPIDEFINITIONS_H
#define MEDIA_PLAYER_PRO_WINDOWSAPIDEFINITIONS_H

#include <cstdint>
#include <cinttypes>
#include <thread>
#ifndef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#endif
#define WINAPI_FAMILY_PARTITION(Partitions)     0
#include "no_sal2.h"
#include "WinUser.h"

//
// Basics
//

#define FALSE               0
#define TRUE                1

typedef void VOID;
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
typedef int INT;

typedef void * PVOID;
typedef PVOID HANDLE;
#define DECLARE_HANDLE(name) typedef HANDLE name

#define far
#define near
#define FAR                 far
#define NEAR                near
#define CONST               const

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL near           *PBOOL;
typedef BOOL far            *LPBOOL;
typedef BYTE near           *PBYTE;
typedef BYTE far            *LPBYTE;
typedef int near            *PINT;
typedef int far             *LPINT;
typedef WORD near           *PWORD;
typedef WORD far            *LPWORD;
typedef long far            *LPLONG;
typedef DWORD near          *PDWORD;
typedef DWORD far           *LPDWORD;
typedef void far            *LPVOID;
typedef CONST void far      *LPCVOID;
typedef WORD                ATOM;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

typedef int64_t           INT_PTR;
typedef uint64_t          UINT_PTR;
typedef int64_t           LONG_PTR;
typedef uint64_t          ULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
typedef LONG_PTR    SSIZE_T;
typedef ULONG_PTR   SIZE_T;


/* Types use for passing & returning polymorphic values */
typedef UINT_PTR            WPARAM;
typedef LONG_PTR            LPARAM;
typedef LONG_PTR            LRESULT;

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT, NEAR *NPPOINT, FAR *LPPOINT;

#undef WINBASEAPI
#undef WINABLEAPI
#undef WINUSERAPI
#undef WINAPI
#undef APIENTRY
#undef CALLBACK
#define WINBASEAPI
#define WINUSERAPI
#define WINABLEAPI
#define WINAPI
#define CALLBACK
#define APIENTRY
#ifndef CALLBACKNATIVE
#define CALLBACKNATIVE extern "C"
#endif

typedef CHAR *LPSTR;
typedef const CHAR *LPCSTR;
DECLARE_HANDLE(HINSTANCE);
DECLARE_HANDLE(HWND);
DECLARE_HANDLE(HMENU);
DECLARE_HANDLE(HACCEL);
DECLARE_HANDLE(HBRUSH);
DECLARE_HANDLE(HFONT);
DECLARE_HANDLE(HDC);
DECLARE_HANDLE(HICON);
DECLARE_HANDLE(HRGN);
DECLARE_HANDLE(HMONITOR);
DECLARE_HANDLE(HDESK);
DECLARE_HANDLE(*SPHANDLE);
DECLARE_HANDLE(*LPHANDLE);
DECLARE_HANDLE(HCURSOR);

//
// Locally Unique Identifier
//

typedef struct _LUID {
    DWORD LowPart;
    LONG HighPart;
} LUID, *PLUID;

typedef wchar_t WCHAR;
typedef WCHAR   TCHAR;
typedef WCHAR *LPWSTR;
typedef TCHAR *LPTSTR;
typedef const WCHAR *LPCWSTR;
typedef const TCHAR *LPCTSTR;
typedef _Null_terminated_ WCHAR *NWPSTR, *PWSTR;

typedef pthread_t tid;
typedef DWORD pid;

#define MAXIMUM_WAIT_OBJECTS 64     // Maximum number of wait objects
#define MAXCHAR     0x7f
#define MAXIMUM_SUSPEND_COUNT MAXCHAR // Maximum times thread can be suspended

typedef VOID (CALLBACK* SENDASYNCPROC)(HWND, UINT, ULONG_PTR, LRESULT);
typedef LRESULT (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(
        LPVOID lpThreadParameter
);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;
// pthread wrappers
DWORD PROCESS_MAIN(LPVOID lpParameter);
PVOID PTHREAD_MAIN(PVOID param);

#define __drv_aliasesMem

//
// Process dwCreationFlag values
//

#define DEBUG_PROCESS                     0x00000001
#define DEBUG_ONLY_THIS_PROCESS           0x00000002
#define CREATE_SUSPENDED                  0x00000004
#define DETACHED_PROCESS                  0x00000008

#define CREATE_NEW_CONSOLE                0x00000010
#define NORMAL_PRIORITY_CLASS             0x00000020
#define IDLE_PRIORITY_CLASS               0x00000040
#define HIGH_PRIORITY_CLASS               0x00000080

#define REALTIME_PRIORITY_CLASS           0x00000100
#define CREATE_NEW_PROCESS_GROUP          0x00000200
#define CREATE_UNICODE_ENVIRONMENT        0x00000400
#define CREATE_SEPARATE_WOW_VDM           0x00000800

#define CREATE_SHARED_WOW_VDM             0x00001000
#define CREATE_FORCEDOS                   0x00002000
#define BELOW_NORMAL_PRIORITY_CLASS       0x00004000
#define ABOVE_NORMAL_PRIORITY_CLASS       0x00008000

#define INHERIT_PARENT_AFFINITY           0x00010000
#define INHERIT_CALLER_PRIORITY           0x00020000    // Deprecated
#define CREATE_PROTECTED_PROCESS          0x00040000
#define EXTENDED_STARTUPINFO_PRESENT      0x00080000

#define PROCESS_MODE_BACKGROUND_BEGIN     0x00100000
#define PROCESS_MODE_BACKGROUND_END       0x00200000
#define CREATE_SECURE_PROCESS             0x00400000

#define CREATE_BREAKAWAY_FROM_JOB         0x01000000
#define CREATE_PRESERVE_CODE_AUTHZ_LEVEL  0x02000000
#define CREATE_DEFAULT_ERROR_MODE         0x04000000
#define CREATE_NO_WINDOW                  0x08000000

#define PROFILE_USER                      0x10000000
#define PROFILE_KERNEL                    0x20000000
#define PROFILE_SERVER                    0x40000000
#define CREATE_IGNORE_SYSTEM_DEFAULT      0x80000000

//
// Thread dwCreationFlag values
//

#define STACK_SIZE_PARAM_IS_A_RESERVATION   0x00010000    // Threads only

// memory size definitions
// usage as follows: 4_byte, 4_kilobyte, 4_megabyte, 4_gigabyte
constexpr std::size_t operator"" _byte(unsigned long long v) { return v; }
constexpr std::size_t operator"" _kilobyte(unsigned long long v) { return 1024u * v; }
constexpr std::size_t operator"" _megabyte(unsigned long long v) { return 1024u * 1024u * v; }
constexpr std::size_t operator"" _gigabyte(unsigned long long v) { return 1024u * 1024u * 1024u * v; }

template <typename SIZE>
SIZE roundDown(SIZE value, SIZE size)
{
    if (value % size == 0) return value;
    else return (value/size)*size;
}

template <typename SIZE>
SIZE roundUp(SIZE value, SIZE size, bool roundZero)
{
    if (value == 0) {
        if (roundZero) return size;
        else return value;
    } else if (value % size == 0) return value;
    else return value ? size * ((value + (size - 1)) / size) : size;
}

template <typename SIZE>
SIZE roundUp(SIZE value, SIZE size)
{
    return roundUp<SIZE>(value, size, false);
}

#undef UNICODE

#endif //MEDIA_PLAYER_PRO_WINDOWSAPIDEFINITIONS_H
