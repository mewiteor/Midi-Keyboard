/***************************************************************************
 *	Midi Keyboard -  MIDI keyboard software.
 *  Copyright (C) 2015
 *  Author:  Mewiteor <mewiteor@hotmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

 /****************************************************************************
  *
  * 简介:
  *     一个Midi键盘模拟软件,可选择音色(共128种)，可自定义按键(在模式里选择)
  *     
  ***************************************************************************/
 
#include<windows.h>
#include<windowsx.h>
#include<strsafe.h>
#include<stdio.h>
#include<string.h>
#include<CommCtrl.h>
#include <shlwapi.h>

#pragma comment(lib,"Comctl32.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Imm32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

/*****************************************************************************************************
* Standard MIDI Key Assignments
* Table 10.4: Note numbers for Piano. The entries are expressed in decimal format. (From Mandal text)
* +------+---+---+---+---+---+---+---+---+---+---+---+---+
* |Octave|C  |C# |D  |D# |E  |F  |F# |G  |G# |A  |A# |B  |
* +------+---+---+---+---+---+---+---+---+---+---+---+---+
* |-2    |00 |01 |02 |03 |04 |05 |06 |07 |08 |09 |10 |11 |
* +------+---+---+---+---+---+---+---+---+---+---+---+---+
* |-1    |12 |13 |14 |15 |16 |17 |18 |19 |20 |21 |22 |23 |
* +------+---+---+---+---+---+---+---+---+---+---+---+---+
* |0     |24 |25 |26 |27 |28 |29 |30 |31 |32 |33 |34 |35 |
* +------+---+---+---+---+---+---+---+---+---+---+---+---+
* |1     |36 |37 |38 |39 |40 |41 |42 |43 |44 |45 |46 |47 |
* +------+---+---+---+---+---+---+---+---+---+---+---+---+
* |2     |48 |49 |50 |51 |52 |53 |54 |55 |56 |57 |58 |59 |
* +------+---+---+---+---+---+---+---+---+---+---+---+---+
* |3     |60 |61 |62 |63 |64 |65 |66 |67 |68 |69 |70 |71 |
* +------+---+---+---+---+---+---+---+---+---+---+---+---+
* |4     |72 |73 |74 |75 |76 |77 |78 |79 |80 |81 |82 |83 |
* +------+---+---+---+---+---+---+---+---+---+---+---+---+
* |5     |84 |85 |86 |87 |88 |89 |90 |91 |92 |93 |94 |95 |
* +------+---+---+---+---+---+---+---+---+---+---+---+---+
* |6     |96 |97 |98 |99 |100|101|102|103|104|105|106|107|
* +------+---+---+---+---+---+---+---+---+---+---+---+---+
* |7     |108|109|110|111|112|113|114|115|116|117|118|119|
* +------+---+---+---+---+---+---+---+---+---+---+---+---+
* |8     |120|121|122|123|124|125|126|127|   |   |   |   |
* +------+---+---+---+---+---+---+---+---+---+---+---+---+
*****************************************************************************************************/

enum _MODE
{
    IDM_PLAY_MODE = 0x1000, // 演奏模式
    IDM_SET_KEYS            // 按键设置模式
};

enum _ID
{
    IDM_STATUS_BAR,          // 状态栏ID
    TIMBRE_START = 0x2000    // 音色菜单起始ID
};

// 所有的音色(包括分类)
struct _TIMBRE
{
    const char* type_name;
    const char* names[8];
} timbres[16] =
{
    "钢琴"                  ,{ "大钢琴（声学钢琴）","明亮的钢琴","电钢琴","酒吧钢琴","柔和的电钢琴","加合唱效果的电钢琴","羽管键琴（拨弦古钢琴）","科拉维科特琴（击弦古钢琴）" },
    "色彩打击乐器"          ,{ "钢片琴","钟琴","八音盒","颤音琴","马林巴","木琴","管钟","大扬琴" },
    "风琴"                  ,{ "击杆风琴","打击式风琴","摇滚风琴","教堂风琴","簧管风琴","手风琴","口琴","探戈手风琴" },
    "吉他"                  ,{ "尼龙弦吉他","钢弦吉他","爵士电吉他","清音电吉他","闷音电吉他","加驱动效果的电吉他","加失真效果的电吉他","吉他和音" },
    "贝司"                  ,{ "大贝司（声学贝司）","电贝司（指弹）","电贝司（拨片）","无品贝司","掌击Bass 1","掌击Bass 2","电子合成Bass 1","电子合成Bass 2" },
    "弦乐"                  ,{ "小提琴","中提琴","大提琴","低音大提琴","弦乐群颤音音色","弦乐群拨弦音色","竖琴","定音鼓" },
    "合奏/合唱"             ,{ "弦乐合奏音色1","弦乐合奏音色2","合成弦乐合奏音色1","合成弦乐合奏音色2","人声合唱“啊”","人声“嘟”","合成人声","管弦乐敲击齐奏" },
    "铜管"                  ,{ "小号","长号","大号","加弱音器小号","法国号（圆号）","铜管组（铜管乐器合奏音色）","合成铜管音色1","合成铜管音色2" },
    "簧管"                  ,{ "高音萨克斯风","次中音萨克斯风","中音萨克斯风","低音萨克斯风","双簧管","英国管","巴松（大管）","单簧管（黑管）" },
    "笛"                    ,{ "短笛","长笛","竖笛","排箫","吹瓶声","日本尺八","口哨声","奥卡雷那" },
    "合成主音"              ,{ "合成主音1（方波）","合成主音2（锯齿波）","合成主音3","合成主音4","合成主音5","合成主音6（人声）","合成主音7（平行五度）","合成主音8（贝司加主音）" },
    "合成音色"              ,{ "合成音色1（新世纪）","合成音色2 （温暖）","合成音色3","合成音色4 （合唱）","合成音色5","合成音色6 （金属声）","合成音色7 （光环）","合成音色8" },
    "合成效果"              ,{ "合成效果 1 雨声","合成效果 2 音轨","合成效果 3 水晶","合成效果 4 大气","合成效果 5 明亮","合成效果 6 鬼怪","合成效果 7 回声","合成效果 8 科幻" },
    "民间乐器"              ,{ "西塔尔（印度）","班卓琴（美洲）","三昧线（日本）","十三弦筝（日本）","卡林巴","风笛","民族提琴","山奈" },
    "打击乐器"              ,{ "叮当铃","Agogo","钢鼓","木鱼","太鼓","通通鼓","合成鼓","铜钹" },
    "Sound Effects 声音效果",{ "吉他换把杂音","呼吸声","海浪声","鸟鸣","电话铃","直升机","鼓掌声","枪炮声" }
};

// 电脑键盘的按键对应的键名，用于按键设置时输出按键名称，用Segoe UI字体显示
// 有些字符需要用Segoe UI字体才能显示，在其它字体下看起来是空白
char* key_names[0x100] =
{
    NULL,   "LM",   "RM",   "BK",   "MM",   "X1",   "X2",   NULL,
    "",   "",    NULL,  NULL,   "CL",   "",    NULL,  NULL,
    "",   "CT",   "",   "PA",   "",   "IN",    NULL,  "IJ",
    "IF",   "IHK",  NULL,   "ESC",  "IC",   "INC",  "IA",   "IM",
    "︺",   "PU",   "PD",   "END",  "HOM", "←",   "↑",   "→",
    "↓",   "SL",   "",   "EXE",  "PS",   "INS",  "DEL",  "HL",
    "0",    "1",    "2",    "3",    "4",    "5",    "6",    "7",
    "8",    "9",    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   "A",    "B",    "C",    "D",    "E",    "F",    "G",
    "H",    "I",    "J",    "K",    "L",    "M",    "N",    "O",
    "P",    "Q",    "R",    "S",    "T",    "U",    "V",    "W",
    "X",    "Y",    "Z",    "L",    "R","APP",  NULL,   "",
    "◎",   "①",   "②",   "③",   "④",   "⑤",   "⑥",   "⑦",
    "⑧",   "⑨",   "N*",   "N+",   ";",     "N-",  "N.",  "N/",
    "F1",   "F2",   "F3",   "F4",   "F5",   "F6",   "F7",   "F8",
    "F9",   "F10",  "F11",  "F12",  "F13",  "F14",  "F15",  "F16",
    "F17",  "F18",  "F19",  "F20",  "F21",  "F22",  "F23",  "F24",
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    "NL",   "SL",   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    "L",  "R",  "LC",   "RC",   "L",  "R",   "",  "",
    "",   "",   "",   "",   "",   "",    "",  "",
    "",   "",   "",   "", "",   "",    "A1",  "A2",
    NULL,   NULL,   ";:",   "=+",   ",<",   "-_",   ".>",  "/?",
    "`~",   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   NULL,   NULL,   "[{",   "\\|",   "]}",   "'\"",   "O8",
    NULL,   NULL,   "O102", NULL,   NULL,   "IP",   NULL,   "PK",
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   "AT",   "CS",
    "ES",   "EE",   "Pl",   "Zo",   NULL,   "PA1",  "OC",   NULL
};

LPCSTR lpClassName = "Midi Keyboard",   // 窗口类名
lpWindowTitle = "Midi键盘";             // 窗口标题
UINT g_CurMode = IDM_PLAY_MODE;         // 当前模式
BYTE g_KeysSet[0x80] = { 0 };           // 用于记录按键设置的数组
BOOL g_KeysStatus[0x80] = { FALSE };    // 用于记录按键状态的数组

// 用于记录被鼠标点击的按键的索引,-1表示没有按键被点击
int g_MouseKeyCur = -1;
HFONT g_hFont[2] = { NULL };    // 两种字体:10号Segoe UI和6号Segoe UI
UINT g_uStatusHeight = 0;       // 状态栏高度
HMIDIOUT g_hMidiOut = NULL;     // midiOut句柄
HWND g_hMainWindow = NULL,      // 主窗口句柄
    g_hStatus = NULL;           // 状态栏句柄
HMENU g_hMainMenu = NULL,       // 主菜单句柄
    g_hTimbreMenu = NULL,       // 音色菜单句柄
    g_hModeMenu = NULL;         // 模式菜单句柄
BOOL g_bCanSetKey = FALSE;      // 在设置按键模式中，标记是否处于正在设置按键状态
HINSTANCE g_hInstance = NULL;   // 实例

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void DrawWindow(HDC hdc);               // 绘制窗口
void SetTimbre(size_t index);           // 设置音色
void SetMode(UINT mode);                // 设置模式
int GetKeyFromPoint(int x, int y);      // 根据鼠标的位置获取按键的索引
void ErrorMessage(const char *);        // 输出错误信息

void OnCreate();                        // 创建事件
void OnKeyDown(BYTE key);               // 按下按键事件
void OnKeyUp(BYTE key);                 // 弹起按键事件
void OnMouseMove(int x, int y);         // 鼠标移动事件
void OnMouseLeave();                    // 鼠标离开窗口事件
void OnMouseLButtonDown(int x, int y);  // 鼠标左键按下事件
void OnMouseLButtonUp();                // 鼠标左键弹起事件
void OnMouseRButtonUp(int x, int y);    // 鼠标右键弹起事件
void OnClose();                         // 关闭窗口事件

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex =
    {
        sizeof(WNDCLASSEX),
        CS_HREDRAW | CS_VREDRAW,
        WndProc,
        0,0,g_hInstance = hInstance,NULL,
        LoadCursor(NULL,IDC_ARROW),
        (HBRUSH)GetStockObject(WHITE_BRUSH),
        NULL,lpClassName
    };
    HWND hWnd;
    MSG msg = { 0 };

    // 用于添加菜单
    MENUITEMINFOA mii =
    {
        sizeof(MENUITEMINFOA), MIIM_FTYPE | MIIM_ID | MIIM_STRING,
        MFT_RADIOCHECK | MFT_STRING
    };

    if (!RegisterClassEx(&wcex))
    {
        ErrorMessage("RegisterClassEx");
        return 1;
    }

    g_hMainMenu = CreateMenu();
    g_hTimbreMenu = CreateMenu();
    g_hModeMenu = CreateMenu();
    AppendMenuA(g_hMainMenu, MF_POPUP, (UINT_PTR)g_hTimbreMenu, "音色"); // 添加菜单
    AppendMenuA(g_hMainMenu, MF_POPUP, (UINT_PTR)g_hModeMenu, "模式");   // 添加菜单

    for (size_t i = 0; i < sizeof timbres / sizeof timbres[0]; ++i) // 添加所有的音色子菜单
    {
        HMENU hSubMenu = CreateMenu();
        for (size_t j = 0; j < sizeof timbres[i].names / sizeof timbres[i].names[0]; ++j)
            AppendMenuA(hSubMenu, MF_STRING, TIMBRE_START | i << 3 | j, timbres[i].names[j]);
        AppendMenuA(g_hTimbreMenu, MF_POPUP, (UINT_PTR)hSubMenu, timbres[i].type_name);
    }

    // 添加模式菜单
    mii.wID = IDM_PLAY_MODE;
    mii.dwTypeData = "演奏模式";
    InsertMenuItemA(g_hModeMenu, 0, TRUE, &mii);
    mii.wID = IDM_SET_KEYS;
    mii.dwTypeData = "设置按键模式";
    InsertMenuItemA(g_hModeMenu, 1, TRUE, &mii);

    ImmDisableIME(-1); // 禁用中文输入法

    /***************************************
     * 像素列表:
     * White Key:       88*16
     * Black Key:       52*10
     * White Key Count: 7*10+5==75
     * Width:           16*75==1200
     * Height:          22+88  (Octave,Key)
     ***************************************/
    hWnd = CreateWindow(lpClassName, lpWindowTitle, WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
        0, 0, 1200, 100, NULL, g_hMainMenu, hInstance, NULL);
    if (NULL == hWnd)
    {
        ErrorMessage("CreateWindow");
        return 1;
    }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc; PAINTSTRUCT ps;
    switch (uMsg)
    {
    case WM_NCCREATE:g_hMainWindow = hWnd;return DefWindowProc(hWnd, uMsg, wParam, lParam);
    case WM_CREATE:OnCreate(); break;
    case WM_COMMAND: // 菜单消息处理
        switch (wParam)
        {
        case IDM_PLAY_MODE: case IDM_SET_KEYS: // 模式菜单消息处理
            if (g_hModeMenu&&g_hStatus)
            {
                SetMode((UINT)wParam);
                g_bCanSetKey = FALSE;
            }
            break;
        default: // 音色菜单消息处理
            if ((TIMBRE_START&~0x7f) != TIMBRE_START)
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
            else
            {
                size_t n = wParam & 0x7f;
                SetTimbre(n);
            }
            break;
        }
        break;
    case WM_PAINT: 
        hdc = BeginPaint(hWnd, &ps);
        DrawWindow(hdc);
        EndPaint(hWnd, &ps);
        break;
    case WM_KEYDOWN:
        if (IDM_PLAY_MODE != g_CurMode ||
            lParam & 0x40000000) /* lParam的第30位为1表示长按键一段时间后变成的连击状态，
                                  * 这里不响应连击状态以表示此按键一直长按 */
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        else
            OnKeyDown((BYTE)wParam);
        break;
    case WM_KEYUP: OnKeyUp((BYTE)wParam); break;
    case WM_MOUSEMOVE:
        if (!(wParam&MK_LBUTTON))
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        else // 只响应按下鼠标左键时的鼠标移动事件
            OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_MOUSELEAVE:OnMouseLeave(); break;
    case WM_LBUTTONDOWN: OnMouseLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
    case WM_LBUTTONUP: OnMouseLButtonUp(); break;
    case WM_RBUTTONUP:
        if (IDM_SET_KEYS != g_CurMode)
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        else // 只在按键设置模式响应鼠标右键的事件
            OnMouseRButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_CLOSE: OnClose(); break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

void OnCreate()
{
    RECT rc;
    UINT width, height, widths[4];
    HDC hdc;
    FILE *file;
    char FileName[MAX_PATH];

    // 调整窗口的大小，使客户区大小为1200x112
    GetClientRect(g_hMainWindow, &rc);
    rc.bottom -= 22;
    width = 2400 - (rc.right - rc.left);
    height = 22 + 190 - (rc.bottom - rc.top);
    MoveWindow(g_hMainWindow, 0, 0, width, height, TRUE);

    InitCommonControls(); // 添加状态栏时必需

    // 创建状态栏
    // _ | 音色类别 | 音色 | 模式
    g_hStatus = CreateWindowA(STATUSCLASSNAMEA, NULL, WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0, g_hMainWindow, (HMENU)IDM_STATUS_BAR, g_hInstance, NULL);

    // 设置状态栏列宽
    GetClientRect(g_hStatus, &rc);
    g_uStatusHeight = rc.bottom - rc.top;
    widths[3] = rc.right;
    widths[2] = widths[3] - 80;
    widths[1] = widths[2] - 160;
    widths[0] = widths[1] - 140;
    SendMessageA(g_hStatus, SB_SETPARTS, (WPARAM)(sizeof widths / sizeof widths[0]), (LPARAM)widths);

    // 创建10号Segoe UI和6号Segoe UI
    hdc = GetDC(g_hMainWindow);
    g_hFont[0] = CreateFontA(-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72),
        0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    g_hFont[1] = CreateFontA(-MulDiv(6, GetDeviceCaps(hdc, LOGPIXELSY), 72),
        0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    ReleaseDC(g_hMainWindow, hdc);

    CoInitializeEx(NULL, COINIT_MULTITHREADED); // 在Windows 10中用midiOut必需
    midiOutOpen(&g_hMidiOut, 0, 0, 0, CALLBACK_NULL); // 打开midiOut,获得midiOut句柄

    // 载入按键设置文件
    GetModuleFileNameA(NULL, FileName, MAX_PATH);
    *PathFindExtensionA(FileName) = 0;
    strcat_s(FileName, MAX_PATH, ".dat");
    if (!PathFileExistsA(FileName))
        MessageBoxA(g_hMainWindow, "请先到按键设置模式设置按键。\n\n"
            "提示：在按键设置模式中，用鼠标右键点击钢琴键盘，可取消对应的已设置的按键。",
            "建议", MB_ICONWARNING | MB_OK);
    else
    {
        if (!fopen_s(&file, FileName, "rb") && file)
        {
            fread_s(g_KeysSet, sizeof g_KeysSet, sizeof g_KeysSet[0], sizeof g_KeysSet / sizeof g_KeysSet[0], file);
            fclose(file);
        }
    }

    SetTimbre(0);           // 设置默认音色为第一个音色
    SetMode(IDM_PLAY_MODE); // 设置默认模式为演奏模式
}

void OnKeyDown(BYTE key)
{
    BYTE i;
    HDC hdc;
    for (i = 0; i < 0x80; ++i) // 查找midi键盘上对应的按键
        if (key == g_KeysSet[i])
        {
            g_KeysStatus[i] = TRUE;
            midiOutShortMsg(g_hMidiOut, 0x00400090 | (UINT)i << 8); // 播放对应的音阶
        }
    hdc = GetDC(g_hMainWindow);
    DrawWindow(hdc);
    ReleaseDC(g_hMainWindow, hdc);
}

void OnKeyUp(BYTE key)
{
    BYTE i;
    HDC hdc;
    switch (g_CurMode)
    {
    case IDM_PLAY_MODE: // 演奏模式
        for (i = 0; i < 0x80; ++i) // 查找midi键盘上对应的按键
            if (key == g_KeysSet[i])
            {
                g_KeysStatus[i] = FALSE;
                midiOutShortMsg(g_hMidiOut, 0x00400080 | (UINT)i << 8); // 停止播放对应的音阶
            }
        hdc = GetDC(g_hMainWindow);
        DrawWindow(hdc);
        ReleaseDC(g_hMainWindow, hdc);
        break;
    case IDM_SET_KEYS: // 按键设置模式
        if (g_bCanSetKey)
        {
            g_bCanSetKey = FALSE;
            g_KeysSet[g_MouseKeyCur] = key; // 将输入的按键保存到按键设置数组
            SendMessageA(g_hStatus, SB_SETTEXTA, (WPARAM)0, (LPARAM)"");
            g_MouseKeyCur = -1;
            hdc = GetDC(g_hMainWindow);
            DrawWindow(hdc);
            ReleaseDC(g_hMainWindow, hdc);
        }
        break;
    }
}

void OnMouseMove(int x, int y)
{
    HDC hdc;
    int cur = GetKeyFromPoint(x, y);
    TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT),TME_LEAVE,g_hMainWindow };
    TrackMouseEvent(&tme); // 使窗口接收鼠标离开的消息
    if (cur != g_MouseKeyCur)
    {
        if (IDM_PLAY_MODE == g_CurMode)
        {// 改变按下的midi按键,并改变发出的音阶
            if (g_MouseKeyCur >= 0)
                midiOutShortMsg(g_hMidiOut, 0x00400080 | g_MouseKeyCur << 8);
            g_MouseKeyCur = -1;
            if (cur >= 0)
                midiOutShortMsg(g_hMidiOut, 0x00400090 | cur << 8);
        }
        g_MouseKeyCur = cur;
        hdc = GetDC(g_hMainWindow);
        DrawWindow(hdc);
        ReleaseDC(g_hMainWindow, hdc);
    }
}

void OnMouseLeave()
{
    HDC hdc;
    if (g_MouseKeyCur >= 0)
    {// 鼠标离开后停止发声
        if (IDM_PLAY_MODE == g_CurMode)
            midiOutShortMsg(g_hMidiOut, 0x00400080 | g_MouseKeyCur << 8);
        g_MouseKeyCur = -1;
        hdc = GetDC(g_hMainWindow);
        DrawWindow(hdc);
        ReleaseDC(g_hMainWindow, hdc);
    }
}

void OnMouseLButtonDown(int x, int y)
{
    HDC hdc;
    if (IDM_PLAY_MODE == g_CurMode&&g_MouseKeyCur >= 0)
    {
        midiOutShortMsg(g_hMidiOut, 0x00400080 | g_MouseKeyCur << 8);
        g_MouseKeyCur = -1;
    }
    g_MouseKeyCur = GetKeyFromPoint(x, y); // 记录按下的midi按键的索引
    if (IDM_PLAY_MODE == g_CurMode&&g_MouseKeyCur >= 0) // 鼠标点到的midi按键对应音阶发声
        midiOutShortMsg(g_hMidiOut, 0x00400090 | g_MouseKeyCur << 8);
    hdc = GetDC(g_hMainWindow);
    DrawWindow(hdc);
    ReleaseDC(g_hMainWindow, hdc);
}

void OnMouseLButtonUp()
{
    HDC hdc;
    switch (g_CurMode)
    {
    case IDM_PLAY_MODE:
        if (g_MouseKeyCur >= 0)
        {
            midiOutShortMsg(g_hMidiOut, 0x00400080 | g_MouseKeyCur << 8); // 鼠标左键弹起后,对应的midi按键对应音阶停止发声
            g_MouseKeyCur = -1;
        }
        break;
    case IDM_SET_KEYS:
        if (g_MouseKeyCur >= 0)
        {// 在状态栏第一格显示信息,并准备设置对应按键
            char str[256], *sub[] = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
            sprintf_s(str, 256, "正在设置%d:%s,请输入按键", g_MouseKeyCur / 12 - 2, sub[g_MouseKeyCur % 12]);
            SendMessageA(g_hStatus, SB_SETTEXTA, (WPARAM)0, (LPARAM)str);
            g_bCanSetKey = TRUE;
        }
        break;
    }
    hdc = GetDC(g_hMainWindow);
    DrawWindow(hdc);
    ReleaseDC(g_hMainWindow, hdc);
}

void OnMouseRButtonUp(int x, int y)
{
    HDC hdc;
    int cur = GetKeyFromPoint(x, y);
    if (cur >= 0)
    {
        g_KeysSet[cur] = 0; // 清除被鼠标右键点到的midi按键的按键设置
        hdc = GetDC(g_hMainWindow);
        DrawWindow(hdc);
        ReleaseDC(g_hMainWindow, hdc);
    }
}

void OnClose()
{
    FILE *file;
    char FileName[MAX_PATH];

    midiOutClose(g_hMidiOut);
    CoUninitialize(); // 对应CoInitializeEx
    if (g_hFont[0])
        DeleteFont(g_hFont[0]);
    if (g_hFont[1])
        DeleteFont(g_hFont[1]);

    // 保存按键设置信息到程序名.dat
    GetModuleFileNameA(NULL, FileName, MAX_PATH);
    *PathFindExtensionA(FileName) = 0;
    strcat_s(FileName, MAX_PATH, ".dat");
    if (!fopen_s(&file, FileName, "wb") && file)
    {
        fwrite(g_KeysSet, sizeof g_KeysSet[0], sizeof g_KeysSet / sizeof g_KeysSet[0], file);
        fclose(file);
    }

    PostQuitMessage(0);
}

void DrawWindow(HDC hdc)
{
    RECT rc;
    HDC hMemDc;
    HBITMAP hMemBmp;
    HBITMAP hOldBmp;
    RECT rcKeys = { 0 };
    size_t i, j;

    GetClientRect(g_hMainWindow, &rc);
    rc.bottom -= g_uStatusHeight; // 只绘制除状态栏以外的地方，将状态栏从客户区排除
    hMemDc = CreateCompatibleDC(hdc);
    hMemBmp = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
    hOldBmp = SelectObject(hMemDc, hMemBmp);

    // 按键所在的区域
    rcKeys.left = rc.left;
    rcKeys.top = rc.top + 22;
    rcKeys.right = rc.right;
    rcKeys.bottom = rc.bottom;

    SetBkMode(hMemDc, TRANSPARENT); // 文本背景透明
    SelectFont(hMemDc, g_hFont[0]);
    FillRect(hMemDc, &rc, GetStockBrush(BLACK_BRUSH)); // 背景为黑色
    SetTextColor(hMemDc, RGB(0, 0, 0)); // 文本黑色
#pragma region 八度
    // 绘制顶部八度信息
    for (i = 0; i < 10; ++i)
    {
        RECT rect = { rc.left + 112 * i + 1,rc.top + 1,rc.left + 112 * i + 111,rc.top + 22 };
        char octave[8];
        int len;
        FillRect(hMemDc, &rect, GetStockBrush(LTGRAY_BRUSH));
        len = sprintf_s(octave, sizeof octave / sizeof octave[0], "%d", (int)i - 2);
        DrawTextA(hMemDc, octave, len, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    {
        RECT rect = { rc.left + 1121,rc.top + 1,rc.right - 1,rc.top + 22 };
        FillRect(hMemDc, &rect, GetStockBrush(LTGRAY_BRUSH));
        DrawTextA(hMemDc, "8", 1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
#pragma endregion
#pragma region 白键
    for (i = 0; i < 75; ++i)
    {
        char key = (i + 2) % 7 + 'A';
        RECT rect = { rcKeys.left + 16 * i + 1, rcKeys.top + 1,rcKeys.left + 16 * i + 15,rcKeys.bottom - 1 };
        const static BYTE sub[] = { 0,2,4,5,7,9,11 };
        BYTE val = (BYTE)(i / 7 * 12 + sub[i % 7]); // 按键索引
        // 按下时为亮灰色,否则为白色
        FillRect(hMemDc, &rect,
            GetStockBrush(g_MouseKeyCur == (int)val || g_KeysStatus[val] ?
                LTGRAY_BRUSH : WHITE_BRUSH));
        rect.top = rect.bottom - 35;
        SelectFont(hMemDc, g_hFont[0]);
        DrawTextA(hMemDc, &key, 1, &rect, DT_CENTER | DT_TOP); // 输出音阶信息
        if (g_KeysSet[val] && key_names[g_KeysSet[val]])
        {
            rect.top = rect.bottom - 15;
            SelectFont(hMemDc, g_hFont[1]);
            // 输出按键设置信息
            DrawTextA(hMemDc, key_names[g_KeysSet[val]],
                strlen(key_names[g_KeysSet[val]]), &rect, DT_CENTER | DT_TOP);
        }
    }
#pragma endregion
#pragma region 黑键
    SetTextColor(hMemDc, RGB(255, 255, 255));
    SelectFont(hMemDc, g_hFont[1]);
    for (i = 0; i < 10; ++i)
    {
        for (j = 0; j < 6; ++j)
        {
            if (j != 2)
            {
                char key[2] = { (j + 2) % 7 + 'A','#' };
                size_t n = i * 7 + j;
                RECT rect = { rcKeys.left + 16 * n + 11,rcKeys.top + 1,rcKeys.left + 16 * n + 21,rcKeys.top + 53 };
                const static BYTE sub[] = { 1,3,0,6,8,10,0 };
                BYTE val = (BYTE)(i * 12 + sub[j]); // 按键索引
                // 按下时为暗灰色,否则为黑色
                FillRect(hMemDc, &rect,
                    GetStockBrush(g_MouseKeyCur == (int)val || g_KeysStatus[val] ?
                        DKGRAY_BRUSH : BLACK_BRUSH));
                rect.top += 10;
                DrawTextA(hMemDc, key, 2, &rect, DT_CENTER | DT_TOP); // 输出音阶信息
                if (g_KeysSet[val] && key_names[g_KeysSet[val]])
                {
                    rect.top += 20;
                    // 输出按键设置信息
                    DrawTextA(hMemDc, key_names[g_KeysSet[val]],
                        strlen(key_names[g_KeysSet[val]]), &rect, DT_CENTER | DT_TOP);
                }
            }
        }
    }
    for (j = 0; j < 4; ++j)
    {
        if (j != 2)
        {
            char key[2] = { (j + 2) % 7 + 'A','#' };
            size_t n = 70 + j;
            RECT rect = { rcKeys.left + 16 * n + 11,rcKeys.top + 1,rcKeys.left + 16 * n + 21,rcKeys.top + 53 };
            const static BYTE sub[] = { 1,3,0,6,8,10,0 };
            BYTE val = (BYTE)(120 + sub[j]); // 按键索引
            // 按下时为暗灰色,否则为黑色
            FillRect(hMemDc, &rect,
                GetStockBrush(g_MouseKeyCur == (int)val || g_KeysStatus[val] ?
                    DKGRAY_BRUSH : BLACK_BRUSH));
            rect.top += 10;
            DrawTextA(hMemDc, key, 2, &rect, DT_CENTER | DT_TOP); // 输出音阶信息
            if (g_KeysSet[val] && key_names[g_KeysSet[val]])
            {
                rect.top += 20;
                // 输出按键设置信息
                DrawTextA(hMemDc, key_names[g_KeysSet[val]],
                    strlen(key_names[g_KeysSet[val]]), &rect, DT_CENTER | DT_TOP);
            }
        }
    }
#pragma endregion
    BitBlt(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMemDc, rc.left, rc.top, SRCCOPY);
    SelectObject(hMemDc, hOldBmp);
    DeleteObject(hMemBmp);
    DeleteDC(hMemDc);
}

void SetTimbre(size_t index)
{
    SendMessageA(g_hStatus, SB_SETTEXTA, (WPARAM)1, (LPARAM)timbres[index >> 3].type_name);
    SendMessageA(g_hStatus, SB_SETTEXTA, (WPARAM)2, (LPARAM)timbres[index >> 3].names[index & 7]);
    midiOutShortMsg(g_hMidiOut, 0xc0 | index << 8); // 改变音色
}

void SetMode(UINT mode)
{
    char name[13];
    CheckMenuRadioItem(g_hModeMenu, IDM_PLAY_MODE, IDM_SET_KEYS, mode, MF_BYCOMMAND); // 设置单选菜单
    GetMenuStringA(g_hModeMenu, mode, name, sizeof name / sizeof name[0], MF_BYCOMMAND);
    SendMessageA(g_hStatus, SB_SETTEXTA, (WPARAM)3, (LPARAM)name);
    SendMessageA(g_hStatus, SB_SETTEXTA, (WPARAM)0, (LPARAM)"");
    g_CurMode = mode;
}

int GetKeyFromPoint(int x, int y)
{
    int r = -1;
    if (y < 23 || y == 212)return r;
    if (y < 85 && x >= 11)
    {
        BYTE n, p;
        n = (x - 8) >> 4, p = (x - 8) & 0xf;
        if (n % 7 != 2 && n % 7 != 6 && p >= 3 && p < 13)
        {
            const static BYTE sub[] = { 1,3,0,6,8,10,0 };
            return n / 7 * 12 + sub[n % 7];
        }
    }
    {
        BYTE n = x >> 4, p = x & 0xf;
        if (!p || p == 0xf)
            return r;
        else
        {
            const static BYTE sub[] = { 0,2,4,5,7,9,11 };
            return n / 7 * 12 + sub[n % 7];
        }
    }
}

// Retrieve the system error message for the last-error code
void ErrorMessage(const char*  lpszFunction)
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}
