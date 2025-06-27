#include <stdio.h>
#include <windows.h>
#include <winternl.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>   // for tolower()
#include <string.h>  // for strcmp()
#include <wchar.h>
#include <locale.h>

//#include "keybord.c"
#pragma comment(lib, "ntdll.lib")

const char* version = "1.1.0";

typedef struct {
    unsigned char button;
    char x;
    char y;
    char wheel;
    unsigned char unk1;
} MOUSE_IO;

// 定义鼠标按钮状态枚举
typedef enum {
    released = 0,
    lmb_down = 1,
    rmb_down = 2,
    lmb_rmb_down = 3,
    mmb_down = 4,
    lmb_mmb_down = 5,
    rmb_mmb_down = 6,
    lmb_rmb_mmb_down = 7
} Mouse;

#define IO_CTL_CODE 0x2a2010
#define LOGHUB  "1abc05c0-c378-41b9-9cef-df1aba82b015"
//LGS:
              //ROOT#SYSTEM#0001#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}
              //ROOT#SYSTEM#0001#{5bada891-842b-4296-a496-68ae931aa16c}
              //Root#SYSTEM#0001#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}
              //Root#SYSTEM#0001#{5bada891-842b-4296-a496-68ae931aa16c}

              //G HUB:
              //ROOT#SYSTEM#0001#{1abc05c0-c378-41b9-9cef-df1aba82b015}
              //ROOT#SYSTEM#0002#{1abc05c0-c378-41b9-9cef-df1aba82b015} ?
              //ROOT#SYSTEM#0001#{dfbedcdb-2148-416d-9e4d-cecc2424128c}

static HANDLE g_input;
static IO_STATUS_BLOCK g_io;

BOOL g_found_mouse;

static NTSTATUS device_initialize(PCWSTR device_name) {
    UNICODE_STRING name;
    OBJECT_ATTRIBUTES attr;

    RtlInitUnicodeString(&name, device_name);
    InitializeObjectAttributes(&attr, &name, 0, NULL, NULL);

    NTSTATUS status = NtCreateFile(&g_input, GENERIC_WRITE | SYNCHRONIZE, &attr, &g_io, 0,
        FILE_ATTRIBUTE_NORMAL, 0, 3, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, 0, 0);

    return status;
}

__declspec(dllexport) BOOL device_open() {
    NTSTATUS status = FALSE;

    if (g_input == 0) {
        wchar_t buffer0[] = L"\\??\\ROOT#SYSTEM#0002#{1abc05c0-c378-41b9-9cef-df1aba82b015}";
        status = device_initialize(buffer0);
        if (NT_SUCCESS(status))
            g_found_mouse = 1;
        else {
            wchar_t buffer1[] = L"\\??\\ROOT#SYSTEM#0001#{1abc05c0-c378-41b9-9cef-df1aba82b015}";
            status = device_initialize(buffer1);
            if (NT_SUCCESS(status))
                g_found_mouse = 1;
        }
    }

    if (NT_SUCCESS(status)) {
        // 设置宽字符环境
        setlocale(LC_ALL, "");
        wprintf(L"罗技驱动加载成功! 本dll免费,欢迎加入交流QQ群:946412676");
    }
    else {
        wprintf(L"罗技驱动初始化失败!");
    }
    return NT_SUCCESS(status);
}

__declspec(dllexport) void device_close() {
    if (g_input != 0) {
        ZwClose(g_input);
        g_input = 0;
    }
}

// 核心鼠标操作函数
BOOL mouse(Mouse button, char x, char y, char wheel) {
    MOUSE_IO buffer = { 0 };
    buffer.button = (unsigned char)button;
    buffer.x = x;
    buffer.y = y;
    buffer.wheel = wheel;
    buffer.unk1 = 0;

    IO_STATUS_BLOCK block;
    NTSTATUS status = NtDeviceIoControlFile(g_input, 0, 0, 0, &block, IO_CTL_CODE, &buffer, sizeof(MOUSE_IO), 0, 0);

    if (!NT_SUCCESS(status)) {
        device_close();
        device_open();
    }

    return NT_SUCCESS(status);
}

// 滚轮
__declspec(dllexport) BOOL wheel(i) {
    return mouse(released, 0, 0, i);
}

// 滚轮向上
__declspec(dllexport) BOOL wheelup() {
    return mouse(released, 0, 0, 1);
}

// 滚轮向下
__declspec(dllexport) BOOL wheeldown() {
    return mouse(released, 0, 0, -1);
}

// 左键按下
__declspec(dllexport) BOOL lmbDown() {
    return mouse(lmb_down, 0, 0, 0);
}

// 右键按下
__declspec(dllexport) BOOL rmbDown() {
    return mouse(rmb_down, 0, 0, 0);
}

// 中键按下
__declspec(dllexport) BOOL mmbDown() {
    return mouse(mmb_down, 0, 0, 0);
}

// 鼠标松开
__declspec(dllexport) BOOL mouseUp() {
    return mouse(released, 0, 0, 0);
}

// 鼠标移动
__declspec(dllexport) BOOL move(char x, char y) {
    return mouse(released, x, y, 0);
}

// 新版本 move 函数，增加按钮状态参数
__declspec(dllexport) BOOL move_with_button(char x, char y, Mouse button_state) {
    return mouse(button_state, x, y, 0);
}



typedef enum _Keyboard {
    KEY_A = 0x4,
    KEY_B = 0x5,
    KEY_C = 0x6,
    KEY_D = 0x7,
    KEY_E = 0x8,
    KEY_F = 0x9,
    KEY_G = 0xA,
    KEY_H = 0xB,
    KEY_I = 0xC,
    KEY_J = 0xD,
    KEY_K = 0xE,
    KEY_L = 0xF,
    KEY_M = 0x10,
    KEY_N = 0x11,
    KEY_O = 0x12,
    KEY_P = 0x13,
    KEY_Q = 0x14,
    KEY_R = 0x15,
    KEY_S = 0x16,
    KEY_T = 0x17,
    KEY_U = 0x18,
    KEY_V = 0x19,
    KEY_W = 0x1A,
    KEY_X = 0x1B,
    KEY_Y = 0x1C,
    KEY_Z = 0x1D,
    KEY_N1 = 0x1E,
    KEY_N2 = 0x1F,
    KEY_N3 = 0x20,
    KEY_N4 = 0x21,
    KEY_N5 = 0x22,
    KEY_N6 = 0x23,
    KEY_N7 = 0x24,
    KEY_N8 = 0x25,
    KEY_N9 = 0x26,
    KEY_N0 = 0x27,
    KEY_ENTER = 0x28,
    KEY_ESC = 0x29,
    KEY_BACK_SPACE = 0x2A,
    KEY_TAB = 0x2B,
    KEY_SPACE = 0x2C,
    KEY_MINUS = 0x2D,
    KEY_EQUAL = 0x2E,
    KEY_SQUARE_BRACKET_LEFT = 0x2F,
    KEY_SQUARE_BRACKET_RIGHT = 0x30,
    KEY_BACK_SLASH = 0x31,
    KEY_BACK_SLASH2 = 0x32,
    KEY_COLUMN = 0x33,
    KEY_QUOTE = 0x34,
    KEY_BACK_TICK = 0x35,
    KEY_COMMA = 0x36,
    KEY_PERIOD = 0x37,
    KEY_SLASH = 0x38,
    KEY_CAP = 0x39,
    KEY_F1 = 0x3A,
    KEY_F2 = 0x3B,
    KEY_F3 = 0x3C,
    KEY_F4 = 0x3D,
    KEY_F5 = 0x3E,
    KEY_F6 = 0x3F,
    KEY_F7 = 0x40,
    KEY_F8 = 0x41,
    KEY_F9 = 0x42,
    KEY_F10 = 0x43,
    KEY_F11 = 0x44,
    KEY_F12 = 0x45,
    KEY_SNAPSHOT = 0x46,
    KEY_SCROLL_LOCK = 0x47,
    KEY_PAUSE = 0x48,
    KEY_INSERT = 0x49,
    KEY_HOME = 0x4A,
    KEY_PAGE_UP = 0x4B,
    KEY_DEL = 0x4C,
    KEY_END = 0x4D,
    KEY_PAGE_DOWN = 0x4E,
    KEY_RIGHT = 0x4F,
    KEY_LEFT = 0x50,
    KEY_DOWN = 0x51,
    KEY_UP = 0x52,
    KEY_NUMLOCK = 0x53,
    KEY_NUMPAD_DIV = 0x54,
    KEY_NUMPAD_MUL = 0x55,
    KEY_NUMPAD_MINUS = 0x56,
    KEY_NUMPAD_PLUS = 0x57,
    KEY_NUMPAD_ENTER = 0x58,
    KEY_NUMPAD_1 = 0x59,
    KEY_NUMPAD_2 = 0x5A,
    KEY_NUMPAD_3 = 0x5B,
    KEY_NUMPAD_4 = 0x5C,
    KEY_NUMPAD_5 = 0x5D,
    KEY_NUMPAD_6 = 0x5E,
    KEY_NUMPAD_7 = 0x5F,
    KEY_NUMPAD_8 = 0x60,
    KEY_NUMPAD_9 = 0x61,
    KEY_NUMPAD_0 = 0x62,
    KEY_NUMPAD_DEC = 0x63,
    KEY_APPS = 0x65,
    KEY_F13 = 0x68,
    KEY_F14 = 0x69,
    KEY_F15 = 0x6A,
    KEY_F16 = 0x6B,
    KEY_F17 = 0x6C,
    KEY_F18 = 0x6D,
    KEY_F19 = 0x6E,
    KEY_F20 = 0x6F,
    KEY_F21 = 0x70,
    KEY_F22 = 0x71,
    KEY_F23 = 0x72,
    KEY_F24 = 0x73,
    KEY_RWIN = 0x8C,
    KEY_F24_ = 0x94,
    KEY_LCTRL = 0xE0,
    KEY_LSHIFT = 0xE1,
    KEY_LALT = 0xE2,
    KEY_LWIN = 0xE3,
    KEY_RCTRL = 0xE4,
    KEY_RSHIFT = 0xE5,
    KEY_RALT = 0xE6,
    KEY_RWIN_ = 0xE7
} Keyboard;

static Keyboard char_to_key(const char* key_name) {
    // --- 单字符处理（字母/数字/符号） ---
    if (strlen(key_name) == 1) {
        char c = tolower(key_name[0]);
        switch (c) {
            // 字母键 (a-z)
        case 'a': return KEY_A;
        case 'b': return KEY_B;
        case 'c': return KEY_C;
        case 'd': return KEY_D;
        case 'e': return KEY_E;
        case 'f': return KEY_F;
        case 'g': return KEY_G;
        case 'h': return KEY_H;
        case 'i': return KEY_I;
        case 'j': return KEY_J;
        case 'k': return KEY_K;
        case 'l': return KEY_L;
        case 'm': return KEY_M;
        case 'n': return KEY_N;
        case 'o': return KEY_O;
        case 'p': return KEY_P;
        case 'q': return KEY_Q;
        case 'r': return KEY_R;
        case 's': return KEY_S;
        case 't': return KEY_T;
        case 'u': return KEY_U;
        case 'v': return KEY_V;
        case 'w': return KEY_W;
        case 'x': return KEY_X;
        case 'y': return KEY_Y;
        case 'z': return KEY_Z;

            // 数字键 (0-9)
        case '1': return KEY_N1;
        case '2': return KEY_N2;
        case '3': return KEY_N3;
        case '4': return KEY_N4;
        case '5': return KEY_N5;
        case '6': return KEY_N6;
        case '7': return KEY_N7;
        case '8': return KEY_N8;
        case '9': return KEY_N9;
        case '0': return KEY_N0;

            // 符号键
        case '\n': return KEY_ENTER;   // 回车
        case ' ':  return KEY_SPACE;   // 空格
        case '-':  return KEY_MINUS;   // 减号
        case '=':  return KEY_EQUAL;   // 等号
        case '[':  return KEY_SQUARE_BRACKET_LEFT;   // 左方括号
        case ']':  return KEY_SQUARE_BRACKET_RIGHT;  // 右方括号
        case '\\': return KEY_BACK_SLASH;  // 反斜杠
        case ';':  return KEY_COLUMN;      // 分号
        case '\'': return KEY_QUOTE;       // 单引号
        case '`':  return KEY_BACK_TICK;   // 反引号
        case ',':  return KEY_COMMA;       // 逗号
        case '.':  return KEY_PERIOD;      // 句号
        case '/':  return KEY_SLASH;       // 斜杠
        }
    }

    // --- 功能键（字符串匹配）---
    if (strcmp(key_name, "space") == 0)        return KEY_SPACE;
    if (strcmp(key_name, "enter") == 0)        return KEY_ENTER;
    if (strcmp(key_name, "esc") == 0)          return KEY_ESC;
    if (strcmp(key_name, "backspace") == 0)    return KEY_BACK_SPACE;
    if (strcmp(key_name, "tab") == 0)          return KEY_TAB;
    if (strcmp(key_name, "caps") == 0)         return KEY_CAP;
    if (strcmp(key_name, "f1") == 0)           return KEY_F1;
    if (strcmp(key_name, "f2") == 0)           return KEY_F2;
    if (strcmp(key_name, "f3") == 0)           return KEY_F3;
    if (strcmp(key_name, "f4") == 0)           return KEY_F4;
    if (strcmp(key_name, "f5") == 0)           return KEY_F5;
    if (strcmp(key_name, "f6") == 0)           return KEY_F6;
    if (strcmp(key_name, "f7") == 0)           return KEY_F7;
    if (strcmp(key_name, "f8") == 0)           return KEY_F8;
    if (strcmp(key_name, "f9") == 0)           return KEY_F9;
    if (strcmp(key_name, "f10") == 0)          return KEY_F10;
    if (strcmp(key_name, "f11") == 0)          return KEY_F11;
    if (strcmp(key_name, "f12") == 0)          return KEY_F12;
    if (strcmp(key_name, "f13") == 0)          return KEY_F13;
    if (strcmp(key_name, "f14") == 0)          return KEY_F14;
    if (strcmp(key_name, "f15") == 0)          return KEY_F15;
    if (strcmp(key_name, "f16") == 0)          return KEY_F16;
    if (strcmp(key_name, "f17") == 0)          return KEY_F17;
    if (strcmp(key_name, "f18") == 0)          return KEY_F18;
    if (strcmp(key_name, "f19") == 0)          return KEY_F19;
    if (strcmp(key_name, "f20") == 0)          return KEY_F20;
    if (strcmp(key_name, "f21") == 0)          return KEY_F21;
    if (strcmp(key_name, "f22") == 0)          return KEY_F22;
    if (strcmp(key_name, "f23") == 0)          return KEY_F23;
    if (strcmp(key_name, "f24") == 0)          return KEY_F24;
    if (strcmp(key_name, "snapshot") == 0)     return KEY_SNAPSHOT;
    if (strcmp(key_name, "scrolllock") == 0)   return KEY_SCROLL_LOCK;
    if (strcmp(key_name, "pause") == 0)        return KEY_PAUSE;
    if (strcmp(key_name, "insert") == 0)       return KEY_INSERT;
    if (strcmp(key_name, "home") == 0)         return KEY_HOME;
    if (strcmp(key_name, "pageup") == 0)       return KEY_PAGE_UP;
    if (strcmp(key_name, "delete") == 0)       return KEY_DEL;
    if (strcmp(key_name, "end") == 0)          return KEY_END;
    if (strcmp(key_name, "pagedown") == 0)     return KEY_PAGE_DOWN;
    if (strcmp(key_name, "right") == 0)        return KEY_RIGHT;
    if (strcmp(key_name, "left") == 0)         return KEY_LEFT;
    if (strcmp(key_name, "down") == 0)         return KEY_DOWN;
    if (strcmp(key_name, "up") == 0)           return KEY_UP;
    if (strcmp(key_name, "numlock") == 0)      return KEY_NUMLOCK;
    if (strcmp(key_name, "numpad_div") == 0)   return KEY_NUMPAD_DIV;
    if (strcmp(key_name, "numpad_mul") == 0)   return KEY_NUMPAD_MUL;
    if (strcmp(key_name, "numpad_minus") == 0) return KEY_NUMPAD_MINUS;
    if (strcmp(key_name, "numpad_plus") == 0)  return KEY_NUMPAD_PLUS;
    if (strcmp(key_name, "numpad_enter") == 0) return KEY_NUMPAD_ENTER;
    if (strcmp(key_name, "numpad_1") == 0)     return KEY_NUMPAD_1;
    if (strcmp(key_name, "numpad_2") == 0)     return KEY_NUMPAD_2;
    if (strcmp(key_name, "numpad_3") == 0)     return KEY_NUMPAD_3;
    if (strcmp(key_name, "numpad_4") == 0)     return KEY_NUMPAD_4;
    if (strcmp(key_name, "numpad_5") == 0)     return KEY_NUMPAD_5;
    if (strcmp(key_name, "numpad_6") == 0)     return KEY_NUMPAD_6;
    if (strcmp(key_name, "numpad_7") == 0)     return KEY_NUMPAD_7;
    if (strcmp(key_name, "numpad_8") == 0)     return KEY_NUMPAD_8;
    if (strcmp(key_name, "numpad_9") == 0)     return KEY_NUMPAD_9;
    if (strcmp(key_name, "numpad_0") == 0)     return KEY_NUMPAD_0;
    if (strcmp(key_name, "numpad_dec") == 0)   return KEY_NUMPAD_DEC;
    if (strcmp(key_name, "apps") == 0)         return KEY_APPS;
    if (strcmp(key_name, "rwindows") == 0)     return KEY_RWIN;
    if (strcmp(key_name, "f24_") == 0)         return KEY_F24_;

    // --- 修饰键 ---
    if (strcmp(key_name, "ctrl") == 0 || strcmp(key_name, "lctrl") == 0)  return KEY_LCTRL;
    if (strcmp(key_name, "shift") == 0 || strcmp(key_name, "lshift") == 0) return KEY_LSHIFT;
    if (strcmp(key_name, "alt") == 0 || strcmp(key_name, "lalt") == 0)    return KEY_LALT;
    if (strcmp(key_name, "lwindows") == 0)                                return KEY_LWIN;
    if (strcmp(key_name, "rctrl") == 0)                                   return KEY_RCTRL;
    if (strcmp(key_name, "rshift") == 0)                                  return KEY_RSHIFT;
    if (strcmp(key_name, "ralt") == 0)                                    return KEY_RALT;
    if (strcmp(key_name, "rwindows_") == 0)                               return KEY_RWIN_;

    return 0; // 无效键名
}

#define MAX_PRESSED_KEYS 6

Keyboard pressed_keys[MAX_PRESSED_KEYS] = { 0 }; // 当前按下的按键
int pressed_count = 0; // 当前按下的按键数量

#pragma pack(push, 1)  // 强制1字节对齐
struct KEYBOARD_IO {
    uint8_t unknown0;
    uint8_t unknown1;
    uint8_t button0;
    uint8_t button1;
    uint8_t button2;
    uint8_t button3;
    uint8_t button4;
    uint8_t button5;
};
#pragma pack(pop)      // 恢复默认对齐
_Static_assert(sizeof(struct KEYBOARD_IO) == 8, "KEYBOARD_IO size mismatch");

BOOL keyboard(Keyboard b0, Keyboard b1, Keyboard b2, Keyboard b3, Keyboard b4, Keyboard b5) {
    struct KEYBOARD_IO buffer = { 0, 0, b0, b1, b2, b3, b4, b5 };
    IO_STATUS_BLOCK block;
    NTSTATUS status = NT_SUCCESS(NtDeviceIoControlFile(
        g_input,
        NULL,
        NULL,
        NULL,
        &block,
        0x2A200C,
        &buffer,
        sizeof(buffer),
        NULL,
        0
    ));

    if (!NT_SUCCESS(status)) {
        device_close();
        device_open();
    }

    return NT_SUCCESS(status);
}

// 按下某个键（如果未满 6 个键）
__declspec(dllexport) void press_key(char* key_name) {
    Keyboard key = char_to_key(key_name);

    if (pressed_count >= MAX_PRESSED_KEYS) {
        return; // 已达到最大按键数
    }

    // 检查是否已经按下
    for (int i = 0; i < pressed_count; i++) {
        if (pressed_keys[i] == key) {
            return; // 已经按下，不重复添加
        }
    }

    // 添加新按下的键
    pressed_keys[pressed_count++] = key;

    //查看当前按下键码信息
    /*printf("After press, keys: ");
    for (int i = 0; i < MAX_PRESSED_KEYS; i++) {
        printf("%02X ", pressed_keys[i]);
    }
    printf("\n");*/

    // 发送新的按键状态
    keyboard(
        pressed_keys[0],
        pressed_keys[1],
        pressed_keys[2],
        pressed_keys[3],
        pressed_keys[4],
        pressed_keys[5]
    );
}

// 释放某个键（如果存在）
__declspec(dllexport) void release_key(char* key_name) {
    Keyboard key = char_to_key(key_name);
    int found_index = -1;

    // 查找要释放的键
    for (int i = 0; i < pressed_count; i++) {
        if (pressed_keys[i] == key) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        return; // 该键未被按下
    }

    // 移除该键（将后面的键前移）
    for (int i = found_index; i < MAX_PRESSED_KEYS - 1; i++) {
        pressed_keys[i] = pressed_keys[i + 1];
    }

    // 显式清零最后一个位置
    pressed_keys[MAX_PRESSED_KEYS - 1] = 0;

    if (pressed_count > 0) {
        pressed_count--;
    }

    //查看当前释放键码信息
    /*printf("After release, keys: ");
    for (int i = 0; i < MAX_PRESSED_KEYS; i++) {
        printf("%02X ", pressed_keys[i]);
    }
    printf("\n");*/

    // 发送新的按键状态（剩余按下的键）
    keyboard(
        pressed_keys[0],
        pressed_keys[1],
        pressed_keys[2],
        pressed_keys[3],
        pressed_keys[4],
        pressed_keys[5]
    );
}
// 释放全部键（如果存在）
__declspec(dllexport) void release_key_all() {
    keyboard(0, 0, 0, 0, 0, 0);
}

int main() {
    printf("测试程序\n");
    // 初始化设备
    if (!device_open()) {
        printf("初始化设备失败！\n");
        return 1;
    }
    printf("设备初始化成功\n");

    //鼠标测试
    // 测试1：鼠标移动（画一个正方形）
    printf("\n=== 测试鼠标移动 ===\n");
    for (int i = 0; i < 50; i++) {
        if (!move(10, 0)) printf("→移动失败\n");  // 右移
        Sleep(10);
    }
    for (int i = 0; i < 50; i++) {
        if (!move(0, 10)) printf("↓移动失败\n");  // 下移
        Sleep(10);
    }
    for (int i = 0; i < 50; i++) {
        if (!move(-10, 0)) printf("←移动失败\n"); // 左移
        Sleep(10);
    }
    for (int i = 0; i < 50; i++) {
        if (!move(0, -10)) printf("↑移动失败\n");  // 上移
        Sleep(10);
    }
    // 测试2：鼠标点击
    printf("\n=== 测试鼠标点击 ===\n");
    printf("左键按下...");
    if (lmbDown()) {
        printf("成功\n");
        Sleep(500);
        printf("释放按键...");
        if (mouseUp()) printf("成功\n");
        else printf("失败！\n");
    }
    else {
        printf("失败！\n");
    }

    Sleep(500);

    printf("右键按下...");
    if (rmbDown()) {
        printf("成功\n");
        Sleep(500);
        printf("释放按键...");
        if (mouseUp()) printf("成功\n");
        else printf("失败！\n");
    }
    else {
        printf("失败！\n");
    }

    Sleep(500);

    printf("中键按下...");
    if (mmbDown()) {
        printf("成功\n");
        Sleep(50);
        printf("释放按键...");
        if (mouseUp()) printf("成功\n");
        else printf("失败！\n");
    }
    else {
        printf("失败！\n");
    }
    if (mmbDown()) {
        printf("成功\n");
        Sleep(50);
        printf("释放按键...");
        if (mouseUp()) printf("成功\n");
        else printf("失败！\n");
    }
    else {
        printf("失败！\n");
    }

    // 测试3：滚轮滚动
    printf("\n=== 测试滚轮 ===\n");
    printf("向上滚动...");
    if (wheelup()) {
        printf("成功\n");
        Sleep(1000);
        printf("向下滚动...");
        if (wheeldown()) printf("成功\n");
        else printf("失败！\n");
    }
    else {
        printf("失败！\n");
    }

    // 测试4：组合操作（移动中点击）
    printf("\n=== 测试组合操作 ===\n");
    printf("移动并左键键点击...\n");
    if (!rmbDown()) printf("右键按下失败\n");
    for (int i = 0; i < 20; i++) {
        if (!move(5, 5)) printf("移动失败\n");
        Sleep(50);
    }
    mouseUp(); // 确保最终释放
    // 场景2：移动中切换按键
    printf("2. 移动中切换左右键...\n");
    for (int i = 0; i < 40; i++) {
        Mouse btn = released;
        if (i < 10) btn = lmb_down;
        else if (i < 20) btn = rmb_down;
        else if (i < 30) btn = lmb_rmb_down;

        move_with_button(i - 20, 0, btn); // 水平移动
        Sleep(50);
    }
    mouseUp();

    Sleep(1000);

    //键盘测试
    printf("\n=== 测试键盘按键 ===\n");
    printf("按下按键ABCDEF\n");
    press_key("a");
    press_key("b");
    press_key("c");
    press_key("d");
    press_key("e");
    press_key("f");
    Sleep(1000);

    printf("释放按键A\n");
    release_key("a");
    Sleep(1000);
    printf("释放按键B\n");
    release_key("b");
    Sleep(1000);
    printf("释放按键C\n");
    release_key("c");
    Sleep(1000);
    printf("释放所有按键\n");
    release_key_all();

    // 关闭设备
    device_close();
    printf("\n所有测试完成\n");
    //system("pause");

    return 0;
}