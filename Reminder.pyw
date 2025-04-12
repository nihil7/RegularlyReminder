import time
import datetime
import pygame
import sys
import threading
from pystray import Icon, MenuItem, Menu
from PIL import Image
import os

# 获取当前脚本所在目录
script_dir = os.path.dirname(os.path.abspath(__file__))

# 使用相对路径定义文件
TXT_PATH = os.path.join(script_dir, "reminders.txt")  # 提醒时间列表文件
SOUND_FILE = os.path.join(script_dir, "8766.mp3")     # 提示音文件
ICON_PATH = os.path.join(script_dir, "favicon.ico")      # 托盘图标
# 打印路径检查
print("提醒列表路径:", TXT_PATH)
print("提示音路径:", SOUND_FILE)
print("托盘图标路径:", ICON_PATH)


# ========== 初始化 ========== #
pygame.mixer.init()

def read_reminders_from_txt():
    """ 从 TXT 读取提醒时间列表，并转换为 (小时, 分钟) 格式 """
    reminders = []
    try:
        with open(TXT_PATH, "r") as file:
            for line in file:
                # 去除空白字符，并按空格分割
                time_str = line.strip()
                hour, minute = map(int, time_str.split())  # 用空格分隔
                reminders.append((hour, minute))
        return sorted(reminders)  # 按时间排序
    except Exception as e:
        print(f"读取 TXT 文件失败: {e}")
        return []

def get_next_reminder():
    """ 获取下一个最近的提醒时间 """
    now = datetime.datetime.now()
    reminders = read_reminders_from_txt()

    for hour, minute in reminders:
        reminder_time = now.replace(hour=hour, minute=minute, second=0, microsecond=0)
        if reminder_time > now:
            return reminder_time

    # 如果今天的提醒时间都过了，就返回明天的第一个提醒时间
    if reminders:
        hour, minute = reminders[0]
        return now.replace(day=now.day + 1, hour=hour, minute=minute, second=0, microsecond=0)

    return None  # 没有提醒时间

def play_sound():
    """ 播放提醒音 """
    try:
        pygame.mixer.music.load(SOUND_FILE)
        pygame.mixer.music.play()
        print(f"提醒: 播放提示音 {SOUND_FILE}")
    except Exception as e:
        print(f"播放提示音失败: {e}")

def reminder_loop():
    """ 按提醒列表精准等待 """
    while True:
        next_reminder = get_next_reminder()
        if next_reminder:
            wait_time = (next_reminder - datetime.datetime.now()).total_seconds()
            print(f"下次提醒时间: {next_reminder}，等待 {wait_time} 秒")
            time.sleep(wait_time)
            play_sound()
        else:
            print("没有可用的提醒时间，等待 1 小时后重新检查")
            time.sleep(3600)  # 没有提醒时，每小时检查一次

def exit_program(icon):
    """ 退出程序 """
    icon.stop()
    sys.exit()

def run_tray():
    """ 运行系统托盘 """
    image = Image.open(ICON_PATH)
    menu = Menu(MenuItem("退出", exit_program))
    icon = Icon("reminder", image, menu=menu)
    icon.run()

if __name__ == "__main__":
    # 启动定时提醒线程
    reminder_thread = threading.Thread(target=reminder_loop, daemon=True)
    reminder_thread.start()

    # 运行托盘
    print("程序已最小化到托盘")
    run_tray()
