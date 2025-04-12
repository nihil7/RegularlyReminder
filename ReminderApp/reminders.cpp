#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <Windows.h>
#include <ctime>
#include <string>

#define SOUND_FILE "C:\\Users\\ishel\\Desktop\\定时提醒\\8766.wav"  // 提示音文件路径

// 结构体用于存储提醒时间
struct Reminder {
    int hour;
    int minute;
};

// 读取提醒时间列表（从TXT文件）
std::vector<Reminder> readRemindersFromTxt(const std::string& filename) {
    std::vector<Reminder> reminders;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "无法打开文件：" << filename << std::endl;
        return reminders;
    }

    int hour, minute;
    while (file >> hour >> minute) {
        reminders.push_back({ hour, minute });
    }

    return reminders;
}

// 播放提示音
void playSound() {
    PlaySound(TEXT(SOUND_FILE), NULL, SND_FILENAME | SND_ASYNC);
    std::cout << "提醒: 播放提示音 " << SOUND_FILE << std::endl;
}

// 获取当前时间（小时，分钟）
std::pair<int, int> getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&now_time);
    return { local_time->tm_hour, local_time->tm_min };
}

// 检查是否需要提醒
bool checkForReminder(const std::vector<Reminder>& reminders, const std::pair<int, int>& currentTime) {
    for (const auto& reminder : reminders) {
        if (reminder.hour == currentTime.first && reminder.minute == currentTime.second) {
            return true;
        }
    }
    return false;
}

// 主函数：定时检查提醒并播放音频
void reminderLoop(const std::vector<Reminder>& reminders) {
    while (true) {
        auto currentTime = getCurrentTime();
        if (checkForReminder(reminders, currentTime)) {
            playSound();
        }
        std::this_thread::sleep_for(std::chrono::seconds(60));  // 每分钟检查一次
    }
}

int main() {
    // 读取提醒时间列表
    std::string filename = "C:\\Users\\ishel\\Desktop\\定时提醒\\reminders.txt";
    std::vector<Reminder> reminders = readRemindersFromTxt(filename);

    if (reminders.empty()) {
        std::cerr << "没有提醒时间列表可用!" << std::endl;
        return 1;
    }

    // 启动定时提醒线程
    std::thread reminderThread(reminderLoop, reminders);
    reminderThread.join();

    return 0;
}
