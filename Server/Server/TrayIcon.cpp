#include "TrayIcon.h"
#include <windows.h>
#include <shellapi.h>

#define WM_TRAYICON (WM_USER + 1)

NOTIFYICONDATA nid = {};
HMENU hMenu = NULL;
HWND hwndConsole = NULL;
HWND hwndMessage = NULL;

void ShowConsole() { ShowWindow(hwndConsole, SW_SHOW); }
void HideConsole() { ShowWindow(hwndConsole, SW_HIDE); }

LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TRAYICON) {
        if (lParam == WM_RBUTTONUP) {
            POINT p;
            GetCursorPos(&p);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, p.x, p.y, 0, hwnd, NULL);
            PostMessage(hwnd, WM_NULL, 0, 0);
        }
    }
    else if (msg == WM_COMMAND) {
        switch (LOWORD(wParam)) {
        case 1:
            ShowConsole();
            break;
        case 2:
            HideConsole();
            break;
        case 3:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            ExitProcess(0);
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void InitTrayIcon() {
    hwndConsole = GetConsoleWindow();

    WNDCLASS wc = {};
    wc.lpfnWndProc = MsgWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"TrayIconMsgWindow";
    RegisterClass(&wc);

    hwndMessage = CreateWindowEx(0, L"TrayIconMsgWindow", NULL, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    ShowWindow(hwndMessage, SW_HIDE);

    HICON hIcon = LoadIcon(NULL, IDI_APPLICATION);
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwndMessage;
    nid.uID = 1;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = hIcon;
    lstrcpy(nid.szTip, L"My Server");

    Shell_NotifyIcon(NIM_ADD, &nid);

    hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, 1, L"Show Console");
    AppendMenu(hMenu, MF_STRING, 2, L"Hide Console");
    AppendMenu(hMenu, MF_STRING, 3, L"Exit");

    HideConsole();
}

void ShutdownTrayIcon() {
    Shell_NotifyIcon(NIM_DELETE, &nid);
}
