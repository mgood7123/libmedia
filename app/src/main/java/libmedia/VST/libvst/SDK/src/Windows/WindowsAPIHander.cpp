//
// Created by konek on 7/16/2019.
//

#include <Windows/WindowsAPIHander.h>

ATOM RegisterClassA(CONST WNDCLASSA *lpWndClass) {
    // TODO
    return 0;
}

ATOM RegisterClassW(CONST WNDCLASSW *lpWndClass) {
    return RegisterClassA(reinterpret_cast<const WNDCLASSA *>(lpWndClass));
}

BOOL UnregisterClassA(LPCSTR lpClassName, HINSTANCE hInstance) {
    // TODO
    return 0;
}

BOOL UnregisterClassW(LPCWSTR lpClassName, HINSTANCE hInstance) {
    // TODO
    return 0;
}

BOOL GetClassInfoA(HINSTANCE hInstance, LPCSTR lpClassName, LPWNDCLASSA lpWndClass) {
    // TODO
    return 0;
}

BOOL GetClassInfoW(HINSTANCE hInstance, LPCWSTR lpClassName, LPWNDCLASSW lpWndClass) {
    // TODO
    return 0;
}

HWND CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth,
                     int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    // TODO: class support
    CREATESTRUCTA a;
    a.dwExStyle = dwExStyle;
    a.lpszClass = lpClassName;
    a.lpszName = lpWindowName;
    a.x = X;
    a.y = Y;
    a.hInstance = hInstance;
    a.hwndParent = hWndParent;
    a.hMenu = hMenu;
    a.lpCreateParams = lpParam;
    a.cx = nWidth;
    a.cy = nHeight;
    a.style = dwStyle;
    SendMessageA(hInstance, WM_CREATE, 0, 0);
    return hInstance;

}

HWND
CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth,
                int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    return CreateWindowExA(
            dwExStyle,
            reinterpret_cast<LPCSTR>(lpClassName),
            reinterpret_cast<LPCSTR>(lpWindowName),
            dwStyle,
            X,
            Y,
            nWidth,
            nHeight,
            hWndParent,
            hMenu,
            hInstance,
            lpParam);
}
