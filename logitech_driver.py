import ctypes
import os
import sys
import pyautogui
import math
import time


try:
    root = os.path.abspath(os.path.dirname(__file__))
    driver = ctypes.CDLL(f"{root}/x64/Release/Logitech_driver.dll")
    ok = driver.device_open() == 1  # 该驱动每个进程可打开一个实例
    sys.stdout.write("\r" + "罗技驱动加载成功!" + " " * 100 + "\n")
    sys.stdout.flush()
    if not ok:
        print("Error, GHUB or LGS driver not found")
except FileNotFoundError:
    print(f"Error, DLL file not found")


Mousedict = {
    "released": 0,
    "lmb_down": 1,
    "rmb_down": 2,
    "lmb_rmb_down": 3,
    "mmb_down": 4,
    "lmb_mmb_down": 5,
    "rmb_mmb_down": 6,
    "lmb_rmb_mmb_down": 7,
}


def sleep(s):
    start_time = time.time()
    while (time.time() - start_time) < s:
        pass


class Logitech:
    @staticmethod
    def close():
        driver.device_close()

    class mouse:
        """
        code: 1:左键, 2:右键, 3:中键
        """

        @staticmethod
        def press(code):
            if not ok:
                return
            if code == 1:
                driver.lmbDown()
            elif code == 2:
                driver.rmbDown()
            elif code == 3:
                driver.mmbDown()

        @staticmethod
        def release(code):
            if not ok:
                return
            driver.mouseUp()

        @staticmethod
        def click(code):
            if not ok:
                return
            if code == 1:
                driver.lmbDown()
            elif code == 2:
                driver.rmbDown()
            elif code == 3:
                driver.mmbDown()
            driver.mouseUp()

        @staticmethod
        def scroll(a):
            """
            鼠标滚轮

            :param a: 正数向上,负数向下
            """
            if not ok:
                return
            driver.wheel(a)

        @staticmethod
        def move(x, y):
            """
            相对移动,xy参数范围 -127 到 127

            :param x: 水平移动的方向和距离. 正数向右, 负数向左
            :param y: 垂直移动的方向和距离. 正数向下, 负数向上
            """
            if not ok:
                return
            if x == 0 and y == 0:
                return
            driver.move(x, y)

        @staticmethod
        def move_with_button(x, y, btn):
            """
            按下鼠标按键键移动

            :param x: 水平位移
            :param y: 竖直位移
            :param btn: 1:左键, 2:右键, 3:中键, 4:左右键, 5:左中键, 6:右中键, 7:左中右键
            """
            if not ok:
                return
            if x == 0 and y == 0:
                return

            if btn == 1:
                code = "lmb_down"
                driver.lmbDown()
            elif btn == 2:
                code = "rmb_down"
                driver.rmbDown()
            elif btn == 3:
                code = "mmb_down"
                driver.mmbDown()
            elif btn == 4:
                code = "lmb_rmb_down"
                driver.lmbDown()
                driver.rmbDown()
            elif btn == 5:
                code = "lmb_mmb_down"
                driver.lmbDown()
                driver.mmbDown()
            elif btn == 6:
                code = "rmb_mmb_down"
                driver.rmbDown()
                driver.mmbDown()
            elif btn == 7:
                code = "lmb_rmb_mmb_down"
                driver.lmbDown()
                driver.rmbDown()
                driver.mmbDown()

            driver.move_with_button(x, y, Mousedict[code])
            driver.mouseUp()

        @staticmethod
        def moveto(x, y, duration=0.8):
            return pyautogui.moveTo(x, y, duration)

            # if not ok:
            #     return
            # if x == 0 and y == 0:
            #     return
            # sleep_time = round(d / max(abs(x - current_x), abs(y - current_y)), 3)
            n = 4
            current_x, current_y = pyautogui.position()
            while abs(x - current_x) > n - 1 or abs(y - current_y) > n - 1:
                current_x, current_y = pyautogui.position()
                # print(current_x, current_y)
                dx = x - current_x
                dy = y - current_y
                # 计算方向
                direction_x = 1 if dx > 0 else (-1 if dx < 0 else 0)
                direction_y = 1 if dy > 0 else (-1 if dy < 0 else 0)

                if x == current_x:
                    direction_x = 0
                if y == current_y:
                    direction_y = 0
                # 调用移动方法
                driver.move(direction_x * n, direction_y * n)
                sleep(0.001)

        def moveto_(target_x, target_y, duration=0.5):
            """
            平滑移动鼠标到绝对位置 (target_x, target_y)

            :param target_x: 目标位置的 x 坐标
            :param target_y: 目标位置的 y 坐标
            :param total_time: 整个移动过程所需的时间（秒）
            """
            step = 5
            # 获取当前鼠标位置
            current_x, current_y = pyautogui.position()
            # print(f"Current position: ({current_x}, {current_y})")
            if target_x == current_x and target_y == current_y:
                return
            # 计算总移动量
            dx_total = target_x - current_x
            dy_total = target_y - current_y

            # 计算每个轴走多少步
            stepx = math.ceil(abs(dx_total) / step)
            stepy = math.ceil(abs(dy_total) / step)

            # 计算每步的延迟时间
            delay = round(duration / max(stepx, stepy), 3)
            print(f"每步延时: {delay}\n移动步数: {max(stepx, stepy)}")

            # 逐步移动鼠标
            for i in range(max(stepx, stepy)):
                if i + 1 < stepx:
                    dx = step
                elif i + 1 == stepx:
                    dx = abs(dx_total) % step
                else:
                    dx = 0

                if i + 1 < stepy:
                    dy = step
                elif i + 1 == stepy:
                    dy = abs(dy_total) % step
                else:
                    dy = 0

                # print((dx_total / abs(dx_total)) * dx, (dy_total / abs(dy_total)) * dy)

                driver.move(
                    int((dx_total / abs(dx_total)) * dx),
                    int((dy_total / abs(dy_total)) * dy),
                )

                # 添加延迟以实现平滑移动
                sleep(delay)

            # 确保鼠标最终到达目标位置
            # pyautogui.moveTo(target_x, target_y)

    class keyboard:
        """
        键盘按键函数中，传入的参数采用的是键盘按键对应的键码
        code:a-z 0-9 f1-f24 enter esc back_space tab space minus equal square_bracket_left square_bracket_right back_slash back_slash_  column  quote  back_tick  comma  period  slash  cap printscreen  scroll_lock  pause  insert  home  page_up  del  end  page_down  right  left  down  up  numlock  numpad_div  numpad_mul  numpad_minus  numpad_plus   numpad_enter  numpad_1  numpad_2  numpad_3  numpad_4  numpad_5  numpad_6  numpad_7  numpad_8  numpad_9  numpad_0  numpad_dec  apps  lctrl  lshift  lalt  lwin  rctrl  rshift  ralt
        """

        @staticmethod
        def press(code):

            if not ok:
                return
            driver.press_key(code.encode("utf-8"))

        @staticmethod
        def release(code):
            if not ok:
                return
            driver.release_key(code.encode("utf-8"))

        @staticmethod
        def release_all():
            if not ok:
                return
            driver.release_key_all()

        @staticmethod
        def click(code):
            if not ok:
                return
            driver.press_key(code.encode("utf-8"))
            driver.release_key(code.encode("utf-8"))


if __name__ == "__main__":
    time.sleep(1)
    # Logitech.keyboard.click("space")
    # Logitech.mouse.click(3)
    # Logitech.mouse.scroll(129)
    # Logitech.mouse.move(-100, 0)
    # Logitech.mouse.moveto(100, 100)
    # 记录开始时间
    start_time = time.time()
    Logitech.mouse.moveto(100, 100)
    # 记录结束时间
    end_time = time.time()

    # 计算执行时间
    elapsed_time = end_time - start_time
    print(f"代码执行时间: {elapsed_time:.6f} 秒")
    # Logitech.mouse.move_with_button(100, 100, 1)

    # Logitech.keyboard.press("lalt")
    # time.sleep(0.3)
    # Logitech.keyboard.press("tab")
    # Logitech.keyboard.release("tab")
    # Logitech.keyboard.release("lalt")
    Logitech.close()
