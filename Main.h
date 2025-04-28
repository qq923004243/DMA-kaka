
#pragma once
#include <winsock2.h>
#include <windows.h>
#include <Overlay/Overlay.h>
#include <Common/Data.h>
#include <Utils/Utils.h>
#include <Hack/Hack.h>
#include <Common/Offset.h>
#include <thread>
#include <DMALibrary/Memory/Memory.h>
#include <cstdint>
#include <Common/Config.h>

#include <Utils/MachineCodeGenerator.h>

#include <string>
#include <random>
#include <iterator>
#include <algorithm>
#include <cstring>

//
#include "VMP/VMProtectSDK.h"
#include <Utils/w3c/w3c.h>
extern int currentLanguageIndex;  // 声明全局变量
enum class Language1
{
    Chinese,
    English
};

extern Language1 currentLanguage1;
inline std::string kamisj = "";
using namespace std;
static std::string wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], sizeNeeded, NULL, NULL);
    return strTo;
}

wchar_t s_buffer[33];
WNDPROC OldEditProc;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
// 定义红色文本属性（红色文本，黑色背景）
WORD redTextAttr = FOREGROUND_RED | FOREGROUND_INTENSITY;
WORD greenTextAttr = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
// 定义默认文本属性（白色文本，黑色背景）
WORD defaultTextAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
static std::wstring stringToWString(std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
// 显示下载进度条的函数
static void ShowProgress(HWND hwnd, DWORD totalSize, DWORD downloadedSize) {
    int progress = static_cast<int>((static_cast<float>(downloadedSize) / totalSize) * 100);
    std::wstring progressText = L"下载进度: " + std::to_wstring(progress) + L"%";

    // 更新窗口标题显示下载进度
    SetWindowTextW(hwnd, progressText.c_str()); // 假设你在窗口标题中显示进度
}
// 下载文件并显示进度
static bool DownloadNewVersionWithProgress(HWND hwnd, const std::string& downloadUrl, const std::string& savePath) {
    HINTERNET hInternet = InternetOpenA("UpdateAgent", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

    if (!hInternet) return false;

    HINTERNET hConnect = InternetOpenUrlA(hInternet, downloadUrl.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    // 获取文件的总大小
    DWORD fileSize = 0;
    DWORD fileSizeLen = sizeof(fileSize);
    HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &fileSize, &fileSizeLen, NULL);

    // 打开要保存的文件
    FILE* file = fopen(savePath.c_str(), "wb");
    if (!file) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    // 下载文件并显示进度
    char buffer[4096];
    DWORD bytesRead = 0;
    DWORD totalDownloaded = 0;

    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
        fwrite(buffer, 1, bytesRead, file);
        totalDownloaded += bytesRead;

        // 更新下载进度
        ShowProgress(hwnd, fileSize, totalDownloaded);
    }

    fclose(file);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return true;
}

static bool IsRunningAsAdmin()
{
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;

    // 获取管理员组的 SID
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&NtAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &adminGroup))
    {
        // 检查当前令牌是否包含管理员 SID
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    return isAdmin;
}





const char* FILE_NAME = "kami.txt"; // 文件名
static void SaveKami(const char* kami) {
    std::ofstream outfile(FILE_NAME);
    if (outfile.is_open()) {
        outfile << kami;
        outfile.close();
    }
}

static void LoadKami(char* kami, int size) {
    std::ifstream infile(FILE_NAME);
    if (infile.is_open()) {
        infile.getline(kami, size);
        infile.close();
    }
}
HWND hwndEdit;
HBRUSH hbrBackground;
HBRUSH hbrBtnBackground;
HBRUSH hbrStaticBackground;
HBRUSH hbrEditBackground;
HWND hwndLanguageComboBox; // 定义一个全局变量来存储语言选择下拉框的句柄
HWND hwndStatic;
HWND hLoginButton;
HWND hUnbindButton;
HWND Language23;










// 从文件加载语言设置
static int LoadLanguageSetting() {
    FILE* file = fopen("language_setting.txt", "r");
    int languageIndex = 0; // 默认中文
    if (file) {
        fscanf(file, "%d", &languageIndex); // 读取语言索引
        fclose(file);
    }
    return languageIndex;
}




static void SetFontForButton(HWND hButton, const wchar_t* fontName, int fontSize) {
    LOGFONTW logFont = { 0 };
    logFont.lfHeight = fontSize;
    wcscpy_s(logFont.lfFaceName, LF_FACESIZE, fontName);
    HFONT hFont = CreateFontIndirectW(&logFont);
    SendMessageW(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);

}


static void ConvertWCharArrayToCharArray(wchar_t* wstr, char* str, int strSize) {
    // 获取转换后的字符串长度
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (size_needed == 0 || size_needed > strSize) {
        std::cerr << "请重新打开本软件" << std::endl;
        return;
    }

    // 执行转换
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, strSize, NULL, NULL);
}
// 新的窗口过程，用于拦截回车键

static std::string ConvertUTF8toGBK(const std::string& strUTF8) {
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, wstr, len);

    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len];
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);

    std::string strGBK(str);
    delete[] wstr;
    delete[] str;
    return strGBK;
}


// 辅助函数：将日期时间字符串转换为time_t类型
time_t StringToTimeT(const std::string& dateTimeStr) {
    struct tm tm = { 0 }; // 初始化
    // 假设日期时间格式为 YYYY-MM-DD HH:MM:SS
    sscanf_s(dateTimeStr.c_str(), "%d-%d-%d %d:%d:%d",
        &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
        &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    tm.tm_year -= 1900; // tm_year 是从1900年开始计数
    tm.tm_mon -= 1; // tm_mon 是从0开始计数
    return mktime(&tm); // 返回time_t时间
}



