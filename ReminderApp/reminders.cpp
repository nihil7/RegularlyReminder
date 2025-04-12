#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <Windows.h>
#include <ctime>
#include <string>

#define SOUND_FILE "C:\\Users\\ishel\\Desktop\\��ʱ����\\8766.wav"  // ��ʾ���ļ�·��

// �ṹ�����ڴ洢����ʱ��
struct Reminder {
    int hour;
    int minute;
};

// ��ȡ����ʱ���б���TXT�ļ���
std::vector<Reminder> readRemindersFromTxt(const std::string& filename) {
    std::vector<Reminder> reminders;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "�޷����ļ���" << filename << std::endl;
        return reminders;
    }

    int hour, minute;
    while (file >> hour >> minute) {
        reminders.push_back({ hour, minute });
    }

    return reminders;
}

// ������ʾ��
void playSound() {
    PlaySound(TEXT(SOUND_FILE), NULL, SND_FILENAME | SND_ASYNC);
    std::cout << "����: ������ʾ�� " << SOUND_FILE << std::endl;
}

// ��ȡ��ǰʱ�䣨Сʱ�����ӣ�
std::pair<int, int> getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&now_time);
    return { local_time->tm_hour, local_time->tm_min };
}

// ����Ƿ���Ҫ����
bool checkForReminder(const std::vector<Reminder>& reminders, const std::pair<int, int>& currentTime) {
    for (const auto& reminder : reminders) {
        if (reminder.hour == currentTime.first && reminder.minute == currentTime.second) {
            return true;
        }
    }
    return false;
}

// ����������ʱ������Ѳ�������Ƶ
void reminderLoop(const std::vector<Reminder>& reminders) {
    while (true) {
        auto currentTime = getCurrentTime();
        if (checkForReminder(reminders, currentTime)) {
            playSound();
        }
        std::this_thread::sleep_for(std::chrono::seconds(60));  // ÿ���Ӽ��һ��
    }
}

int main() {
    // ��ȡ����ʱ���б�
    std::string filename = "C:\\Users\\ishel\\Desktop\\��ʱ����\\reminders.txt";
    std::vector<Reminder> reminders = readRemindersFromTxt(filename);

    if (reminders.empty()) {
        std::cerr << "û������ʱ���б����!" << std::endl;
        return 1;
    }

    // ������ʱ�����߳�
    std::thread reminderThread(reminderLoop, reminders);
    reminderThread.join();

    return 0;
}
