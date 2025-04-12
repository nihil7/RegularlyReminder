#include <Windows.h>
#include <Shellapi.h>
#include <iostream>

// ��������ͼ����Ϣ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {  // "�˳�" �˵�
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

// ��������ͼ��
void createTrayIcon(HINSTANCE hInstance) {
    const char szClassName[] = "TrayApp";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = szClassName;
    wc.hInstance = hInstance;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, szClassName, "Tray Application", 0, 0, 0, 0, 0, 0, 0, hInstance, 0);

    // ��������ͼ��
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_COMMAND;
    nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
    strcpy_s(nid.szTip, "��ʱ���ѳ���");

    Shell_NotifyIcon(NIM_ADD, &nid);

    // �˵�
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, 1, "�˳�");

    // ��Ӧ����ͼ���Ҽ����
    POINT pt;
    GetCursorPos(&pt);
    TrackPopupMenu(hMenu, TPM_LEFTBUTTON, pt.x, pt.y, 0, hwnd, NULL);

    // ��Ϣѭ��
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // ɾ������ͼ��
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // ��������ͼ��
    createTrayIcon(hInstance);

    return 0;
}
