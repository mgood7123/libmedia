//
// Created by konek on 7/18/2019.
//

#ifndef MEDIA_PLAYER_PRO_WINDOWSAPIHANDLE_H
#define MEDIA_PLAYER_PRO_WINDOWSAPIHANDLE_H

//
// Constants
//
#include "../WindowsAPIDefinitions.h"

#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

typedef class Handle {
public:
    Handle() {
        invalidated = true;
        object = nullptr;
    }
    ~Handle() {
        invalidated = true;
        object = nullptr;
    }
    bool invalidated;
    Object * object;
} Handle;

//
// Prototypes
//
WINBASEAPI
        BOOL
WINAPI
CloseHandle(
        _In_ _Post_ptr_invalid_ HANDLE hObject
);


WINBASEAPI
        BOOL
WINAPI
DuplicateHandle(
        _In_ HANDLE hSourceProcessHandle,
        _In_ HANDLE hSourceHandle,
        _In_ HANDLE hTargetProcessHandle,
        _Outptr_ LPHANDLE lpTargetHandle,
        _In_ DWORD dwDesiredAccess,
        _In_ BOOL bInheritHandle,
        _In_ DWORD dwOptions
);


WINBASEAPI
        BOOL
WINAPI
CompareObjectHandles(
        _In_ HANDLE hFirstObjectHandle,
        _In_ HANDLE hSecondObjectHandle
);

WINBASEAPI
        BOOL
WINAPI
GetHandleInformation(
        _In_ HANDLE hObject,
        _Out_ LPDWORD lpdwFlags
);


WINBASEAPI
        BOOL
WINAPI
SetHandleInformation(
        _In_ HANDLE hObject,
        _In_ DWORD dwMask,
        _In_ DWORD dwFlags
);

#endif //MEDIA_PLAYER_PRO_WINDOWSAPIHANDLE_H
