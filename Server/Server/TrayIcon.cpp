#include "TrayIcon.h"
#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include "resource.h"
#include "sqlite_helper.h"
#include <string>
#include <sstream>
#include <regex>
#define WM_TRAYICON (WM_USER + 1)

bool isValidLogin(const std::string& login) {
    if (login.length() < 4) return false;
    return std::regex_match(login, std::regex("^[a-zA-Z0-9_]+$"));
}

bool isValidPassword(const std::string& password) {
    return password.length() >= 5;
}

INT_PTR CALLBACK AddUserDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_OK_BUTTON) {
            wchar_t wlogin[100], wpass[100];
            GetDlgItemText(hDlg, IDC_LOGIN_EDIT, wlogin, 100);
            GetDlgItemText(hDlg, IDC_PASS_EDIT, wpass, 100);

            char login[100], pass[100];
            size_t converted = 0;

            wcstombs_s(&converted, login, sizeof(login), wlogin, _TRUNCATE);
            wcstombs_s(&converted, pass, sizeof(pass), wpass, _TRUNCATE);

            std::string loginStr(login);
            std::string passStr(pass);

            if (!isValidLogin(loginStr)) {
                MessageBoxA(hDlg, "Login must be at least 4 characters and contain only letters, numbers, and underscores.", "Validation Error", MB_OK | MB_ICONWARNING);
                return TRUE;
            }

            if (!isValidPassword(passStr)) {
                MessageBoxA(hDlg, "Password must be at least 5 characters.", "Validation Error", MB_OK | MB_ICONWARNING);
                return TRUE;
            }

            bool result = insert_user(login, pass);

            MessageBox(hDlg,
                result ? L"User added successfully!" : L"Failed to add user.",
                L"Result",
                MB_OK | (result ? MB_ICONINFORMATION : MB_ICONERROR));

            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hDlg, 0);
        return TRUE;

    case WM_INITDIALOG:
        return TRUE;
    }

    return FALSE;
}



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
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADDUSER_DIALOG), hwnd, AddUserDialogProc);
            break;
        case 4:
            ShowConsole();
            print_all_users();
            break;
        case 5:
        {
            ShowConsole();
            print_all_users();
            std::cout << "Enter the user ID of the user you want to delete: ";

            int choice;
            std::string input;
            std::getline(std::cin, input);

            std::stringstream ss(input);
            if (ss >> choice && ss.eof())
            {
                std::string result = delete_user_by_id(choice);
                std::cout << result << std::endl;
            }
            else
            {
                std::cout << "Invalid input. Please enter a valid numeric ID." << std::endl;
            }
        }
            break;
        case 6:
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
    AppendMenu(hMenu, MF_STRING, 3, L"Add User");
    AppendMenu(hMenu, MF_STRING, 4, L"Check Users");
    AppendMenu(hMenu, MF_STRING, 5, L"Delete User");
    AppendMenu(hMenu, MF_STRING, 6, L"Exit");


    HideConsole();
}

void ShutdownTrayIcon() {
    Shell_NotifyIcon(NIM_DELETE, &nid);
}
