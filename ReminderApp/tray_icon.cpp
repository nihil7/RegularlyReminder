#include <Windows.h>
#include <Shellapi.h>
#include <iostream>

// 处理托盘图标消息
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {  // "退出" 菜单
            PostQuitMessage(0);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// 启动托盘图标
void createTrayIcon(HINSTANCE hInstance) {
    const char szClassName[] = "TrayApp";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = szClassName;
    wc.hInstance = hInstance;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, szClassName, "Tray Application", 0, 0, 0, 0, 0, 0, 0, hInstance, 0);

    // 创建托盘图标
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_COMMAND;
    nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
    strcpy_s(nid.szTip, "定时提醒程序");

    Shell_NotifyIcon(NIM_ADD, &nid);

    // 菜单
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, 1, "退出");

    // 响应托盘图标右键点击
    POINT pt;
    GetCursorPos(&pt);
    TrackPopupMenu(hMenu, TPM_LEFTBUTTON, pt.x, pt.y, 0, hwnd, NULL);

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 删除托盘图标
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // 启动托盘图标
    createTrayIcon(hInstance);

    return 0;
}
