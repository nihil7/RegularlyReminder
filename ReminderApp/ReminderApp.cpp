#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <windows.h>
#include <mmsystem.h>
#include <string>

#pragma comment(lib, "winmm.lib")  // 需要链接 Windows 多媒体库
#define _CRT_SECURE_NO_WARNINGS     // 解决 localtime 警告

using namespace std;

// 配置路径 (宽字符)
const wstring TXT_PATH = L"C:\\Users\\ishel\\Desktop\\定时提醒\\reminders.txt";  // 提醒时间 TXT 文件
const wstring SOUND_FILE = L"C:\\Users\\ishel\\Desktop\\定时提醒\\8766.wav";   // 提示音文件
const wstring ICON_PATH = L"C:\\Users\\ishel\\Desktop\\定时提醒\\icon.ico";    // 托盘图标文件

// 结构体存储提醒时间
struct Reminder {
    int hour;
    int minute;
};

// 读取 TXT 文件的提醒时间
vector<Reminder> readRemindersFromTxt() {
    vector<Reminder> reminders;
    wifstream file(TXT_PATH);  // 使用 wifstream 读取宽字符路径

    if (!file) {
        wcerr << L"无法打开提醒列表文件: " << TXT_PATH << endl;
        return reminders;
    }

    int hour, minute;
    while (file >> hour >> minute) {
        if (minute >= 0 && minute < 60) {  // 确保分钟有效
            reminders.push_back({ hour, minute });
        }
    }

    file.close();
    return reminders;
}

// 获取下一个最近的提醒时间
Reminder getNextReminder() {
    auto reminders = readRemindersFromTxt();
    time_t now = time(nullptr);
    tm timeinfo;
    localtime_s(&timeinfo, &now);  // 使用安全版本的 localtime

    for (const auto& reminder : reminders) {
        if (reminder.hour > timeinfo.tm_hour || (reminder.hour == timeinfo.tm_hour && reminder.minute > timeinfo.tm_min)) {
            return reminder;
        }
    }

    return reminders.empty() ? Reminder{ -1, -1 } : reminders[0];
}

// 播放提醒音
void playSound() {
    PlaySound(SOUND_FILE.c_str(), NULL, SND_FILENAME | SND_ASYNC);
    wcout << L"播放提示音：" << SOUND_FILE << endl;
}

// 提醒线程
void reminderLoop() {
    while (true) {
        Reminder next = getNextReminder();
        if (next.hour == -1) {
            wcout << L"没有可用的提醒时间，1 小时后重新检查" << endl;
            this_thread::sleep_for(chrono::hours(1));
            continue;
        }

        // 计算等待时间
        time_t now = time(nullptr);
        tm timeinfo;
        localtime_s(&timeinfo, &now);  // 使用安全版本的 localtime
        timeinfo.tm_hour = next.hour;
        timeinfo.tm_min = next.minute;
        timeinfo.tm_sec = 0;

        time_t reminder_time = mktime(&timeinfo);
        double wait_seconds = difftime(reminder_time, now);
        if (wait_seconds > 0) {
            wcout << L"下次提醒时间: " << next.hour << L":" << next.minute << L"，等待 " << wait_seconds << L" 秒" << endl;
            this_thread::sleep_for(chrono::seconds(static_cast<int>(wait_seconds)));
            playSound();
        }
    }
}

// 托盘消息处理
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_USER + 1: // 托盘事件
        if (lParam == WM_LBUTTONDOWN) { // 左键点击托盘
            MessageBox(NULL, L"提醒程序正在运行", L"定时提醒", MB_OK);
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

// 创建托盘图标
void createTrayIcon(HWND hwnd) {
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = WM_USER + 1;
    nid.hIcon = (HICON)LoadImage(NULL, ICON_PATH.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

    // 处理宽字符问题
    wcsncpy_s(nid.szTip, L"定时提醒程序", _TRUNCATE);

    Shell_NotifyIcon(NIM_ADD, &nid);
}

// 运行托盘应用
void runTray() {
    const wchar_t CLASS_NAME[] = L"ReminderTray";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"定时提醒", 0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
    createTrayIcon(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// 入口函数
int wmain() {
    thread reminderThread(reminderLoop);
    reminderThread.detach();

    runTray();
    return 0;
}
