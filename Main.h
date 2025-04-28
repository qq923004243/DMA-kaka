
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
extern int currentLanguageIndex;  // ����ȫ�ֱ���
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
// �����ɫ�ı����ԣ���ɫ�ı�����ɫ������
WORD redTextAttr = FOREGROUND_RED | FOREGROUND_INTENSITY;
WORD greenTextAttr = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
// ����Ĭ���ı����ԣ���ɫ�ı�����ɫ������
WORD defaultTextAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
static std::wstring stringToWString(std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
// ��ʾ���ؽ������ĺ���
static void ShowProgress(HWND hwnd, DWORD totalSize, DWORD downloadedSize) {
    int progress = static_cast<int>((static_cast<float>(downloadedSize) / totalSize) * 100);
    std::wstring progressText = L"���ؽ���: " + std::to_wstring(progress) + L"%";

    // ���´��ڱ�����ʾ���ؽ���
    SetWindowTextW(hwnd, progressText.c_str()); // �������ڴ��ڱ�������ʾ����
}
// �����ļ�����ʾ����
static bool DownloadNewVersionWithProgress(HWND hwnd, const std::string& downloadUrl, const std::string& savePath) {
    HINTERNET hInternet = InternetOpenA("UpdateAgent", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

    if (!hInternet) return false;

    HINTERNET hConnect = InternetOpenUrlA(hInternet, downloadUrl.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    // ��ȡ�ļ����ܴ�С
    DWORD fileSize = 0;
    DWORD fileSizeLen = sizeof(fileSize);
    HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &fileSize, &fileSizeLen, NULL);

    // ��Ҫ������ļ�
    FILE* file = fopen(savePath.c_str(), "wb");
    if (!file) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    // �����ļ�����ʾ����
    char buffer[4096];
    DWORD bytesRead = 0;
    DWORD totalDownloaded = 0;

    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
        fwrite(buffer, 1, bytesRead, file);
        totalDownloaded += bytesRead;

        // �������ؽ���
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

    // ��ȡ����Ա��� SID
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&NtAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &adminGroup))
    {
        // ��鵱ǰ�����Ƿ��������Ա SID
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    return isAdmin;
}





const char* FILE_NAME = "kami.txt"; // �ļ���
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
HWND hwndLanguageComboBox; // ����һ��ȫ�ֱ������洢����ѡ��������ľ��
HWND hwndStatic;
HWND hLoginButton;
HWND hUnbindButton;
HWND Language23;










// ���ļ�������������
static int LoadLanguageSetting() {
    FILE* file = fopen("language_setting.txt", "r");
    int languageIndex = 0; // Ĭ������
    if (file) {
        fscanf(file, "%d", &languageIndex); // ��ȡ��������
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
    // ��ȡת������ַ�������
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (size_needed == 0 || size_needed > strSize) {
        std::cerr << "�����´򿪱����" << std::endl;
        return;
    }

    // ִ��ת��
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, strSize, NULL, NULL);
}
// �µĴ��ڹ��̣��������ػس���

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


// ����������������ʱ���ַ���ת��Ϊtime_t����
time_t StringToTimeT(const std::string& dateTimeStr) {
    struct tm tm = { 0 }; // ��ʼ��
    // ��������ʱ���ʽΪ YYYY-MM-DD HH:MM:SS
    sscanf_s(dateTimeStr.c_str(), "%d-%d-%d %d:%d:%d",
        &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
        &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    tm.tm_year -= 1900; // tm_year �Ǵ�1900�꿪ʼ����
    tm.tm_mon -= 1; // tm_mon �Ǵ�0��ʼ����
    return mktime(&tm); // ����time_tʱ��
}



