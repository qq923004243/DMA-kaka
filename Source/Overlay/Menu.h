#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <Common/Data.h>
#include <Common/Entitys.h>
#include <Common/Constant.h>
#include <DMALibrary/Memory/Memory.h>
#include <imgui/imgui.h>
#include <imgui/notify.h>

#include "RenderHelper.h"
#include <string>
#include <Utils/KmBox.h>
#include <Utils/KmBoxNet.h>
#include <Utils/Lurker.h>
#include <Utils/MoBox.h>
#include <Utils/Throttler.h>
#include <Utils/Utils.h>
#include "Common/Config.h"
#include <Utils/FNVHash.h>
#include <vector>
#include <tchar.h>
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"
#include "../Include/imgui/imgui_internal.h"
#include "../Include/imgui/imgui_freetype.h"
#include <d3d11.h>
#include "imgui/texture.h"
#pragma comment(lib, "D3DX11.lib")
#include <D3DX11tex.h>
#include <cstdlib>
//#include "QPVerify.h"
#include "Overlay/MenuPlayerLists.h"
#include "GlobalConfig.h"
#include <algorithm>
#include <fstream>
#include <iostream>
//#include"MenuWebRadar.h"
extern int currentLanguageIndex;  // 声明全局变量
// 定义骨骼点位置和名称
struct BonePosition {
	ImVec2 pos;
	const char* name;
	int index;
};


static const BonePosition bonePositions[] = {
	{ImVec2(134, 99),  "头部",     0},
	{ImVec2(134, 123), "颈部",     1},
	{ImVec2(134, 147), "胸部",     2},
	{ImVec2(134, 177), "腹部",     3},
	{ImVec2(134, 227), "骨盆",     4},
	{ImVec2(134, 257), "大腿根",   5},
	{ImVec2(86, 152),  "左肩",     6},
	{ImVec2(65, 217),  "左上臂",   7},
	{ImVec2(52, 272),  "左前臂",   8},
	{ImVec2(186, 152), "右肩",     9},
	{ImVec2(205, 217), "右上臂",  10},
	{ImVec2(218, 272), "右前臂",  11},
	{ImVec2(104, 282), "左大腿",  12},
	{ImVec2(101, 355), "左膝盖",  13},
	{ImVec2(94, 447),  "左小腿",  14},
	{ImVec2(165, 282), "右大腿",  15},
	{ImVec2(169, 355), "右膝盖",  16},
	{ImVec2(174, 447), "右小腿",  17}
};

// 保存账号和密码到文件
inline void SaveLoginConfig(const std::string& username, const std::string& password)
{
	std::ofstream outFile("config.txt");
	if (outFile.is_open())
	{
		outFile << username << std::endl;
		outFile << password << std::endl;
		outFile.close();
	}
}

// 读取账号和密码从文件
inline bool LoadLoginConfig(std::string& username, std::string& password)
{
	std::ifstream inFile("config.txt");
	if (inFile.is_open())
	{
		std::getline(inFile, username);
		std::getline(inFile, password);
		inFile.close();
		return true; // 成功读取
	}
	return false; // 文件不存在或者读取失败
}

std::string formatTime(const std::string& input) {
	std::regex timePattern(R"((\d{4})年(\d{1,2})月(\d{1,2})日(\d{1,2})时(\d{1,2})分(\d{1,2})秒)");
	std::smatch match;

	if (std::regex_match(input, match, timePattern)) {
		if (match.size() == 7) {
			// 提取各个部分并格式化为目标格式
			std::string year = match[1];
			std::string month = match[2];
			std::string day = match[3];
			std::string hour = match[4];
			std::string minute = match[5];
			std::string second = match[6];

			return year + "-" + month + "-" + day + "-" + hour + ":" + minute + ":" + second;
		}
	}

	// 如果输入格式不符合预期，返回原始输入
	return input;
}

DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

void OpenLink(const char* url) {
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

enum class MenuStyle
{
	Dark,
	Light
};

const char* strcasestr_custom(const char* haystack, const char* needle) {
	if (!haystack || !needle) {
		return nullptr;
	}

	while (*haystack) {
		const char* h = haystack;
		const char* n = needle;

		while (*h && *n && (std::tolower(*h) == std::tolower(*n))) {
			h++;
			n++;
		}

		if (!*n) {
			return haystack;
		}

		haystack++;
	}

	return nullptr;
}



int 玩家 = 0; int 物品 = 0; int 自瞄 = 0;int 雷达 = 0;int 设置 = 0;


inline bool Languages = 1;
float tab_size = 0.f;
float arrow_roll = 0.f;
bool tab_opening = false;


float   nickname_font_size = 12.0f;
float   info_font_size = 12.0f;
float   weapon_font_size = 13.0f;

static bool Headcheckbox = false;
static bool Healthcheckbox = false;
static bool Namecheckbox = false;
static bool Distancecheckbox = false;
static bool weaponcheckbox = false;
static bool Filterteams = false;
static bool Filterteams_map = false;
static bool Bonecheckbox = false;
static float boxtk = 1.f;
static float hptk = 1.f;
static float hdtk = 1.f;
static float bonetk = 1.f;
static float accent[4] = { 82.f / 255.f, 250.f / 255.f, 246.f / 255.f, 255.f / 255.f };// { 184.f / 255.f, 45.f / 255.f, 65.f / 255.f, 255.f / 255.f };
inline ImVec4 background = ImColor(27, 24, 29, 255);
inline ImVec4 outline_background = ImColor(27, 29, 32, 255);

RECT menuRect;
//RECT menuRect1;
inline std::vector<std::string> MenuItemTypes  {
		U8("设置"),
		U8("步枪"),
		U8("连狙"),
		U8("栓狙"),
		U8("散弹"),
		U8("冲锋"),
		U8("机枪"),
		U8("手枪"),
		U8("药品"),
		U8("装备"),
		U8("枪口"),
		U8("握把"),
		U8("弹匣"),
		U8("枪托"),
		U8("瞄具"),
		U8("子弹"),
		U8("投掷"),
		U8("其它"),

};
std::vector<std::string> itemsChinese = { 
	U8("头皮"), U8("头部"), U8("脖子"), U8("胸部"), U8("裆部"),
	U8("左肩"), U8("左肘"), U8("右肩"), U8("右肘"),
	U8("左手"), U8("右手"), U8("左骨盆"), U8("左腿骨"),
	U8("右骨盆"), U8("右腿骨"), U8("左脚"), U8("右脚") };

int menu_style = 0;
int text_add = 0;
int tabs = 0;
int item_tabs = 0;
MenuStyle currentMenuStyle = MenuStyle::Dark; //初始背景颜色设置

ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;





const char* top_tabs_char[] = { U8(" 自瞄"),  U8(" 视觉"), U8(" 物品分组"), U8(" 雷达"), U8(" 设置") };
const char* top_tabs_icon_char[] = { "o", "c", "m", "M", "f" };
static float tab_alpha = 0.f; /* */ static float tab_add; /* */ static int active_tab = 0; static float anim = 0.f; static float alpha = 0.f;
static std::vector<int> comboSelections(EntityLists.size(), 0); // 初始化所有 Combo 的选项为 0
const std::string WeapType[7] = { "AR", "DMR", "SR", "LMG", "HG", "SG", "SMG" };
const char* top_label = top_tabs_char[0];
const char* top_icon = top_tabs_icon_char[0];
static bool Bones[17] = {
true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false
};
static bool SecondBones[17] = {
true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false
};
static bool GroggyBones[17] = {
true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false
};
static bool visual_flags[11] = {
true, false, false, false, false, false, false, false, false, false, false
};
inline std::vector<WeaponType> MenuItemTypeIndexs  {
	   WeaponType::AR,
	   WeaponType::AR,
	   WeaponType::DMR,
	   WeaponType::SR,
	   WeaponType::SG,
	   WeaponType::SMG,
	   WeaponType::LMG,
	   WeaponType::HG,
	   WeaponType::Drug,
	   WeaponType::Armor,
	   WeaponType::Muzzle,
	   WeaponType::Grip,
	   WeaponType::Magazine,
	   WeaponType::GunButt,
	   WeaponType::Sight,
	   WeaponType::Bullet,
	   WeaponType::Grenade,
	   WeaponType::Other
};
namespace texture
{
	inline ID3D11ShaderResourceView* background = nullptr;
	inline ID3D11ShaderResourceView* logo = nullptr;
	inline ID3D11ShaderResourceView* playermoder = nullptr;
	inline ID3D11ShaderResourceView* weapon_image = nullptr;
	inline ID3D11ShaderResourceView* rank = nullptr;


}
class newMenu
{
public:

	

	static void Setting(std::string type)
	{
		auto& Config = GameData.Config.AimBot.Configs[GameData.Config.AimBot.ConfigIndex].Weapon[type];
		//auto& Config = GameData.Config.AimBot.Configs[GameData.Config.AimBot.ConfigIndex].Weapon[WeapType[item_world]];

		 ImGui::BeginGroup(); {
			
			ImGui::BeginChild(true, U8("开关自瞄"), "o", ImVec2(306, 470));
			{
				ImGui::Checkbox(U8("启动"), &Config.enable);

				//ImGui::Checkbox(U8("模型漏打"), &Config.LineTraceSingle);

				ImGui::Checkbox(U8("漏哪变色"), &GameData.Config.ESP.AdjustableDistance);

				ImGui::Checkbox(U8("锁定切换"), &Config.FOVenable);

				ImGui::Checkbox(U8("可视检测"), &Config.VisibleCheck);

				if (自瞄 == 5 || 自瞄 == 4 || 自瞄 == 0 || 自瞄 == 1 || 自瞄 == 2) {
					if (Config.VisibleCheck) {

						ImGui::Checkbox(U8("自动扳机"), &Config.AimAndShot);

						ImGui::SliderFloat(U8("扳机阈值"), &Config.Threshold, 1.0f, 15.0f, "%.1f [F]");

						ImGui::SliderInt(U8("扳机延迟"), &Config.Delay1, 1, 5000, "%d\xC2\xB0");

						ImGui::SliderInt(U8("扳机距离"), &Config.banjiAimDistance, 1, 500, "%d\xC2\xB0");
					}

				}
				

				ImGui::Checkbox(U8("启用瞄准车胎"), &Config.AimWheel);

				ImGui::Checkbox(U8("显示锁胎FOV"), &GameData.Config.AimBot.ShowWheelFOV);

				ImGui::Checkbox(U8("弹道预判"), &Config.Prediction);

				ImGui::Checkbox(U8("跳过倒地"), &Config.IgnoreGroggy);

				ImGui::Checkbox(U8("击杀切换"), &Config.AutoSwitch);

				

				ImGui::Checkbox(U8("后座抑制"), &Config.NoRecoil);
				if (!Config.NoRecoil)
					ImGui::Checkbox(U8("原始弹道"), &Config.OriginalRecoil);

				ImGui::Checkbox(U8("显示预瞄"), &GameData.Config.AimBot.ShowPoint);

				ImGui::Checkbox(U8("显示FOV"), &GameData.Config.AimBot.ShowFOV);

				ImGui::Checkbox(U8("空弹不锁"), &Config.NoBulletNotAim);

				ImGui::Checkbox(U8("开镜检测"), &Config.bIsScoping_CP);

				ImGui::Checkbox(U8("动态范围"), &Config.DynamicFov);

				ImGui::SliderInt(U8("保持压枪"), &Config.RecoilTime, 0, 20,"%d\xC2\xB0");

				ImGui::Checkbox(U8("全局压枪"), &GameData.Config.AimBot.qjyaqiang);

				ImGui::SliderFloat(U8("压枪Y轴"), &GameData.Config.AimBot.wybYSpeed, 1.0f, 10.f,"%.1f [F]");

				

				ImGui::Checkbox(U8("热键合并"), &Config.HotkeyMerge);

				ImGui::Keybind(U8("自瞄按键"), &Config.First.Key);

				ImGui::Keybind(U8("强制自瞄按键"), &Config.Second.Key);

				ImGui::Keybind(U8("倒地自瞄按键"), &Config.Groggy.Key);

				ImGui::Keybind(U8("瞄准车胎热键"), &Config.Wheel.Key);

				
				const char* aim_mode[]  { U8("范围优先"), U8("距离优先") };
				ImGui::Combo(U8("优先锁定选择"), &Config.LockMode, aim_mode, IM_ARRAYSIZE(aim_mode));

			}
			ImGui::EndChild(true);
			ImGui::SameLine(); ImGui::SetCursorPosY(GetCursorPosY() - 25);
			ImGui::BeginChild(true, U8("自瞄参数"), "o", ImVec2(306, 470)); {

				ImGui::ColorEdit4(U8("FOV颜色"), GameData.Config.AimBot.FOVColor, picker_flags);

				ImGui::ColorEdit4(U8("锁胎FOV颜色"), GameData.Config.AimBot.WheelFOVColor, picker_flags);

				ImGui::ColorEdit4(U8("预瞄颜色"), GameData.Config.AimBot.PointColor, picker_flags);

				ImGui::SliderInt(U8("预瞄比例"), &GameData.Config.AimBot.PointSize, 1, 20, "%d\xC2\xB0");

				ImGui::SliderFloat(U8("预判X"), &Config.AimPre_X, 0.1f, 5.0f, "%.1f [F]");

				ImGui::SliderFloat(U8("预判Y"), &Config.AimPre_Y, 0.1f, 5.0f, "%.1f [F]");

				ImGui::SliderInt(U8("最大距离"), &Config.AimDistance, 0, 1000, "%d\xC2\xB0");

				ImGui::SliderInt(U8("范围"), &Config.FOV, 0, 180, "%d\xC2\xB0");

				ImGui::SliderFloat(U8("Y轴上下灵敏度"), &Config.YYSpeed, 0.1f, 5.0f, "%.1f [F]");

				ImGui::SliderFloat(U8("X轴左右灵敏度"), &Config.XXSpeed, 0.1f, 5.0f, "%.1f [F]");

				ImGui::SliderFloat(U8("Y轴平滑度"), &Config.YSpeed, 0.1f, 5.0f, "%.1f [F]");

				ImGui::SliderFloat(U8("X轴平滑度"), &Config.XSpeed, 0.1f, 5.0f, "%.1f [F]");

				ImGui::SliderFloat(U8("自瞄吸附强度"), &Config.AimSpeedMaxFactor, 0.1f, 1.f, "%.1f [F]");

				ImGui::SliderInt(U8("平滑比例"), &Config.InitialValue, 100, 1500, "%d\xC2\xB0");

				ImGui::SliderInt(U8("最大帧数"), &Config.FPS, 0, 1000, "%d\xC2\xB0");

				ImGui::SliderInt(U8("切换延迟"), &Config.SwitchingDelay, 1, 10, "%d\xC2\xB0");

				ImGui::SliderInt(U8("锁胎FOV"), &Config.WheelFOV, 1, 360, "%d\xC2\xB0");

				ImGui::SliderFloat(U8("瞄准车胎平滑"), &Config.AimWheelSpeed, 0.1f, 100.f, "%.1f [F]");
			
			}
			ImGui::EndChild(true);
			ImGui::SameLine(); ImGui::SetCursorPosY(GetCursorPosY() - 95);


			//auto& Config = GameData.Config.AimBot.Configs[GameData.Config.AimBot.ConfigIndex].Weapon[WeapType[item_world]];
			ImGui::BeginChild(true, U8("部位选择"), "o", ImVec2(295, 540));
			{


				static int combo;
				const char* comboitem[] = { U8("主要部位"),U8("强制部位"),U8("倒地部位") };

				ImGui::Combo(U8("自瞄部位"), &combo, comboitem, IM_ARRAYSIZE(comboitem));
				ImGui::SetCursorPos(ImVec2(12, 65));
				ImGui::Image(texture::playermoder, ImVec2(258, 463), ImVec2(0, 0), ImVec2(1, 1), ImColor(c::image.x, c::image.y, c::image.z, c::image.w));
				if (combo == 0)
				{
					ImGui::SetCursorPos(ImVec2(134, 99));
					ImGui::Checkbox_hitbox("BONE 1", &Config.First.Bones[0]);
					ImGui::SetCursorPos(ImVec2(134, 123));
					ImGui::Checkbox_hitbox("BONE 2", &Config.First.Bones[1]);
					ImGui::SetCursorPos(ImVec2(134, 147));
					ImGui::Checkbox_hitbox("BONE 3", &Config.First.Bones[2]);
					ImGui::SetCursorPos(ImVec2(134, 177));
					ImGui::Checkbox_hitbox("BONE 4", &Config.First.Bones[3]);
					ImGui::SetCursorPos(ImVec2(134, 227));
					ImGui::Checkbox_hitbox("BONE 5", &Config.First.Bones[4]);
					ImGui::SetCursorPos(ImVec2(134, 257));
					ImGui::Checkbox_hitbox("BONE 6", &Config.First.Bones[5]);
					ImGui::SetCursorPos(ImVec2(86, 152));
					ImGui::Checkbox_hitbox("BONE 7", &Config.First.Bones[6]);
					ImGui::SetCursorPos(ImVec2(65, 217));
					ImGui::Checkbox_hitbox("BONE 8", &Config.First.Bones[7]);
					ImGui::SetCursorPos(ImVec2(52, 272));
					ImGui::Checkbox_hitbox("BONE 9", &Config.First.Bones[8]);
					ImGui::SetCursorPos(ImVec2(186, 152));
					ImGui::Checkbox_hitbox("BONE 10", &Config.First.Bones[9]);
					ImGui::SetCursorPos(ImVec2(205, 217));
					ImGui::Checkbox_hitbox("BONE 11", &Config.First.Bones[10]);
					ImGui::SetCursorPos(ImVec2(218, 272));
					ImGui::Checkbox_hitbox("BONE 12", &Config.First.Bones[11]);
					ImGui::SetCursorPos(ImVec2(104, 282));
					ImGui::Checkbox_hitbox("BONE 13", &Config.First.Bones[12]);
					ImGui::SetCursorPos(ImVec2(101, 355));
					ImGui::Checkbox_hitbox("BONE 14", &Config.First.Bones[13]);
					ImGui::SetCursorPos(ImVec2(94, 447));
					ImGui::Checkbox_hitbox("BONE 15", &Config.First.Bones[14]);
					ImGui::SetCursorPos(ImVec2(165, 282));
					ImGui::Checkbox_hitbox("BONE 16", &Config.First.Bones[15]);
					ImGui::SetCursorPos(ImVec2(169, 355));
					ImGui::Checkbox_hitbox("BONE 17", &Config.First.Bones[16]);
					ImGui::SetCursorPos(ImVec2(174, 447));
					ImGui::Checkbox_hitbox("BONE 18", &Config.First.Bones[17]);
				}
				else if (combo == 1)
				{

					ImGui::SetCursorPos(ImVec2(134, 99));
					ImGui::Checkbox_hitbox("BONE 1", &Config.Second.Bones[0]);
					ImGui::SetCursorPos(ImVec2(134, 123));
					ImGui::Checkbox_hitbox("BONE 2", &Config.Second.Bones[1]);
					ImGui::SetCursorPos(ImVec2(134, 147));
					ImGui::Checkbox_hitbox("BONE 3", &Config.Second.Bones[2]);
					ImGui::SetCursorPos(ImVec2(134, 177));
					ImGui::Checkbox_hitbox("BONE 4", &Config.Second.Bones[3]);
					ImGui::SetCursorPos(ImVec2(134, 227));
					ImGui::Checkbox_hitbox("BONE 5", &Config.Second.Bones[4]);
					ImGui::SetCursorPos(ImVec2(134, 257));
					ImGui::Checkbox_hitbox("BONE 6", &Config.Second.Bones[5]);
					ImGui::SetCursorPos(ImVec2(86, 152));
					ImGui::Checkbox_hitbox("BONE 7", &Config.Second.Bones[6]);
					ImGui::SetCursorPos(ImVec2(65, 217));
					ImGui::Checkbox_hitbox("BONE 8", &Config.Second.Bones[7]);
					ImGui::SetCursorPos(ImVec2(52, 272));
					ImGui::Checkbox_hitbox("BONE 9", &Config.Second.Bones[8]);
					ImGui::SetCursorPos(ImVec2(186, 152));
					ImGui::Checkbox_hitbox("BONE 10", &Config.Second.Bones[9]);
					ImGui::SetCursorPos(ImVec2(205, 217));
					ImGui::Checkbox_hitbox("BONE 11", &Config.Second.Bones[10]);
					ImGui::SetCursorPos(ImVec2(218, 272));
					ImGui::Checkbox_hitbox("BONE 12", &Config.Second.Bones[11]);
					ImGui::SetCursorPos(ImVec2(104, 282));
					ImGui::Checkbox_hitbox("BONE 13", &Config.Second.Bones[12]);
					ImGui::SetCursorPos(ImVec2(101, 355));
					ImGui::Checkbox_hitbox("BONE 14", &Config.Second.Bones[13]);
					ImGui::SetCursorPos(ImVec2(94, 447));
					ImGui::Checkbox_hitbox("BONE 15", &Config.Second.Bones[14]);
					ImGui::SetCursorPos(ImVec2(165, 282));
					ImGui::Checkbox_hitbox("BONE 16", &Config.Second.Bones[15]);
					ImGui::SetCursorPos(ImVec2(169, 355));
					ImGui::Checkbox_hitbox("BONE 17", &Config.Second.Bones[16]);
					ImGui::SetCursorPos(ImVec2(174, 447));
					ImGui::Checkbox_hitbox("BONE 18", &Config.Second.Bones[17]);
				}
				else if (combo == 2)
				{

					ImGui::SetCursorPos(ImVec2(134, 99));
					ImGui::Checkbox_hitbox("BONE 1", &Config.Groggy.Bones[0]);
					ImGui::SetCursorPos(ImVec2(134, 123));
					ImGui::Checkbox_hitbox("BONE 2", &Config.Groggy.Bones[1]);
					ImGui::SetCursorPos(ImVec2(134, 147));
					ImGui::Checkbox_hitbox("BONE 3", &Config.Groggy.Bones[2]);
					ImGui::SetCursorPos(ImVec2(134, 177));
					ImGui::Checkbox_hitbox("BONE 4", &Config.Groggy.Bones[3]);
					ImGui::SetCursorPos(ImVec2(134, 227));
					ImGui::Checkbox_hitbox("BONE 5", &Config.Groggy.Bones[4]);
					ImGui::SetCursorPos(ImVec2(134, 257));
					ImGui::Checkbox_hitbox("BONE 6", &Config.Groggy.Bones[5]);
					ImGui::SetCursorPos(ImVec2(86, 152));
					ImGui::Checkbox_hitbox("BONE 7", &Config.Groggy.Bones[6]);
					ImGui::SetCursorPos(ImVec2(65, 217));
					ImGui::Checkbox_hitbox("BONE 8", &Config.Groggy.Bones[7]);
					ImGui::SetCursorPos(ImVec2(52, 272));
					ImGui::Checkbox_hitbox("BONE 9", &Config.Groggy.Bones[8]);
					ImGui::SetCursorPos(ImVec2(186, 152));
					ImGui::Checkbox_hitbox("BONE 10", &Config.Groggy.Bones[9]);
					ImGui::SetCursorPos(ImVec2(205, 217));
					ImGui::Checkbox_hitbox("BONE 11", &Config.Groggy.Bones[10]);
					ImGui::SetCursorPos(ImVec2(218, 272));
					ImGui::Checkbox_hitbox("BONE 12", &Config.Groggy.Bones[11]);
					ImGui::SetCursorPos(ImVec2(104, 282));
					ImGui::Checkbox_hitbox("BONE 13", &Config.Groggy.Bones[12]);
					ImGui::SetCursorPos(ImVec2(101, 355));
					ImGui::Checkbox_hitbox("BONE 14", &Config.Groggy.Bones[13]);
					ImGui::SetCursorPos(ImVec2(94, 447));
					ImGui::Checkbox_hitbox("BONE 15", &Config.Groggy.Bones[14]);
					ImGui::SetCursorPos(ImVec2(165, 282));
					ImGui::Checkbox_hitbox("BONE 16", &Config.Groggy.Bones[15]);
					ImGui::SetCursorPos(ImVec2(169, 355));
					ImGui::Checkbox_hitbox("BONE 17", &Config.Groggy.Bones[16]);
					ImGui::SetCursorPos(ImVec2(174, 447));
					ImGui::Checkbox_hitbox("BONE 18", &Config.Groggy.Bones[17]);

					struct
					{
						int Key = VK_RBUTTON;
						bool Bones[17] = {
							true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false
						};
					} First;
				}
			}
			ImGui::EndChild(true);
		 }
		 ImGui::EndGroup();
		

	}
	

	
	static void newmenu(HWND hwnd) {

		ImGuiStyle* style = &ImGui::GetStyle();

		static float color[4] = { 217 / 255.f, 164 / 255.f, 88 / 255.f, 1.f };
		c::accent = { color[0], color[1], color[2], 1.f };
		style->WindowPadding = ImVec2(0, 0);
		style->ItemSpacing = ImVec2(15, 15);
		style->WindowBorderSize = 0;
		style->ScrollbarSize = 10.f;//滚动条宽度

		ImGui::SetNextWindowSize(ImVec2(c::bg::size));

		using namespace ImGui;
		ImGui::Begin("Menu", nullptr, window_flags);
		{
			if (currentMenuStyle == MenuStyle::Dark)
			{
				c::shadow = ImColor(2, 2, 2, 255);
				c::bg::background = ImColor(16, 16, 16, 255);
				c::bg::outline = ImColor(38, 38, 38, 255);
				c::bg::top_bg = ImColor(22, 22, 27, 255);
				c::child::background = ImColor(20, 20, 20, 255);
				c::child::border = ImColor(26, 26, 26, 255);
				c::child::lines = ImColor(36, 36, 36, 255);
				c::checkbox::background = ImColor(26, 26, 26, 255);
				c::checkbox::outline = ImColor(35, 35, 35, 255);
				c::checkbox::mark = ImColor(0, 0, 0, 255);
				c::slider::background = ImColor(26, 26, 26, 255);
				c::button::background = ImColor(26, 26, 26, 255);
				c::button::outline = ImColor(36, 36, 36, 255);
				c::combo::background = ImColor(26, 26, 26, 255);
				c::combo::outline = ImColor(36, 36, 36, 255);
				c::keybind::background = ImColor(26, 26, 26, 255);
				c::input::background = ImColor(26, 26, 26, 255);
				c::input::outline = ImColor(36, 36, 36, 255);
				c::picker::background = ImColor(26, 26, 26, 255);
				c::tabs::line = ImColor(36, 36, 36, 255);
				c::text::text_active = ImColor(255, 255, 255, 255);
				c::text::text_hov = ImColor(255, 255, 255, 185);
				c::text::text = ImColor(255, 255, 255, 75);
				c::scrollbar::bar_active = ImColor(255, 255, 255, 145);
				c::scrollbar::bar_hov = ImColor(255, 255, 255, 125);
				c::scrollbar::bar = ImColor(255, 255, 255, 75);
				c::popup_elements::filling = ImColor(10, 10, 10, 170);

				style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);

			}
			else if (currentMenuStyle == MenuStyle::Light)
			{
				c::shadow = ImColor(2, 2, 2, 30);
				c::bg::background = ImColor(243, 243, 243, 255);
				c::bg::outline = ImColor(223, 223, 223, 255);
				c::bg::top_bg = ImColor(223, 223, 223, 255);
				c::child::background = ImColor(234, 234, 234, 255);
				c::child::border = ImColor(230, 230, 230, 255);
				c::child::lines = ImColor(239, 239, 239, 255);
				c::checkbox::background = ImColor(251, 251, 251, 255);
				c::checkbox::outline = ImColor(229, 229, 229, 255);
				c::checkbox::mark = ImColor(255, 255, 255, 255);
				c::slider::background = ImColor(251, 251, 251, 255);
				c::button::background = ImColor(251, 251, 251, 255);
				c::button::outline = ImColor(229, 229, 229, 255);
				c::combo::background = ImColor(251, 251, 251, 255);
				c::combo::outline = ImColor(229, 229, 229, 255);
				c::keybind::background = ImColor(251, 251, 251, 255);
				c::input::background = ImColor(251, 251, 251, 255);
				c::input::outline = ImColor(229, 229, 229, 255);
				c::picker::background = ImColor(251, 251, 251, 255);
				c::tabs::line = ImColor(229, 229, 229, 255);
				c::text::text_active = ImColor(0, 0, 0, 255);
				c::text::text_hov = ImColor(0, 0, 0, 185);
				c::text::text = ImColor(0, 0, 0, 75);
				c::scrollbar::bar_active = ImColor(255, 255, 255, 145);
				c::scrollbar::bar_hov = ImColor(255, 255, 255, 125);
				c::scrollbar::bar = ImColor(255, 255, 255, 75);
				c::popup_elements::filling = ImColor(235, 235, 235, 170);

				style->Colors[ImGuiCol_Text] = ImColor(0, 0, 0, 255);

			}


			//// 设置菜单区域的坐标和大小用于穿透效果


			if (menu_style == 0)
				currentMenuStyle = MenuStyle::Dark;
			else if (menu_style == 1)
				currentMenuStyle = MenuStyle::Light;

			const ImVec2& pos = ImGui::GetWindowPos();
			const ImVec2& region = ImGui::GetContentRegionMax();

			const ImVec2 size = ImGui::GetWindowSize() ;

			const ImVec2& spacing = style->ItemSpacing;
			menuRect.left = pos.x;
			menuRect.right = size.x + pos.x;
			menuRect.top = pos.y;
			menuRect.bottom = size.y + pos.y;
			GetBackgroundDrawList()->AddShadowRect(pos, pos + ImVec2(region), ImGui::GetColorU32(c::shadow), 80, ImVec2(0, 0), NULL, c::bg::rounding);
			GetBackgroundDrawList()->AddRectFilled(pos, pos + ImVec2(region), ImGui::GetColorU32(c::bg::background), c::bg::rounding);
			GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(0, 49), pos + ImVec2(region.x, 50), ImGui::GetColorU32(c::bg::outline));

			GetBackgroundDrawList()->AddText(font::icomoon_menu, 17, pos + ImVec2((region.x / 2 - (ImGui::CalcTextSize(top_label).x / 2) - ImGui::CalcTextSize(top_icon).x) - 8, 14), GetColorU32(c::text::text), top_icon);
			GetBackgroundDrawList()->AddText(font::calibri_regular, 18, pos + ImVec2((region.x / 2 - (ImGui::CalcTextSize(top_label).x / 2) + ImGui::CalcTextSize(top_icon).x) + 4, 14), GetColorU32(c::text::text), top_label);
			GetBackgroundDrawList()->AddText(font::icomoon_menu, 16.f, pos + ImVec2(20, region.y - 40), GetColorU32(c::text::text), "z");
			GetBackgroundDrawList()->AddText(font::calibri_bold, 17.f, pos + ImVec2(44, region.y - 40), GetColorU32(c::text::text), "EX / PUBG-AKM");
			GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(0, region.y - 70), pos + ImVec2(region.x, region.y - 69), ImGui::GetColorU32(c::bg::outline), c::bg::rounding);

			GetBackgroundDrawList()->AddImage(texture::logo, pos + ImVec2(20, 10), pos + ImVec2(20, 10) + ImVec2(30, 23));
			GetBackgroundDrawList()->AddText(font::calibri_regular, 18, pos + ImVec2(70, 15), GetColorU32(c::text::text), "PUBG-AKM");
			GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(100 + CalcTextSize("PUBG-AKM").x, 20), pos + ImVec2(101 + CalcTextSize("PUBG-AKM").x, 30), ImGui::GetColorU32(c::bg::outline));

			PushFont(font::myth_bold);

			SetCursorPos(ImVec2(region.x - 60, 15));
			ImGui::BeginGroup();

			ImGui::TextColored(c::text::text, U8("-"));

			ImGui::SameLine();

			ImGui::TextColored(c::text::text, U8("×"));

			ImGui::EndGroup();

			PopFont();

			SetCursorPos(ImVec2(region.x / 3, region.y - 60));
			BeginGroup();


			for (int i = 0; i < 5; ++i) {

				if (ImGui::Tabs(tabs == i, top_tabs_icon_char[i], top_tabs_char[i], ImVec2(75, 60))) {
					{
						tabs = i; top_label = top_tabs_char[i]; top_icon = top_tabs_icon_char[i];
					}
				}

				ImGui::SameLine(0, 10.f);

			}
			EndGroup();


			tab_alpha = ImLerp(tab_alpha, (tabs == active_tab) ? 1.f : 0.f, 15.f * ImGui::GetIO().DeltaTime);
			if (tab_alpha < 0.01f && tab_add < 0.01f) active_tab = tabs;

			anim = ImLerp(anim, (tabs == active_tab) ? 1.f : 0.f, 6.f * ImGui::GetIO().DeltaTime);


			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * style->Alpha);
			{
				if (active_tab == 0)
				{
					ImGui::SetCursorPos(ImVec2(region.x - (tab_alpha * region.x - 18), 190 - (anim * 120)));
					ImGui::BeginChild(false, "Child", "o", ImVec2(1145, 565));
					{
						ImGui::BeginGroup();
						{
							ImGui::BeginChild(true, U8("自瞄控制器"), "o", ImVec2(180, 540));
							{
								
								const char* youself_char[]{ U8("KMBOX"), U8("KMNET"), U8("Lurker"), U8("键鼠魔盒") };
								ImGui::Combo(U8("控制器"), &GameData.Config.AimBot.Controller, youself_char, IM_ARRAYSIZE(youself_char));
								
								if (GameData.Config.AimBot.Controller == 1) {
								
									ImGui::InputTextEx("##IP/", U8("输入您的ip"), GameData.Config.AimBot.IP, sizeof(GameData.Config.AimBot.IP), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 25), ImGuiInputTextFlags_NoLabel);
									ImGui::InputTextEx("##PORT/", U8("输入您的port"), GameData.Config.AimBot.Port, sizeof(GameData.Config.AimBot.Port), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 25), ImGuiInputTextFlags_NoLabel);
									ImGui::InputTextEx("##UUID/", U8("输入您的uid"), GameData.Config.AimBot.UUID, sizeof(GameData.Config.AimBot.UUID), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 25), ImGuiInputTextFlags_NoLabel);
								}
								
								static std::vector<std::string> ports = Utils::GetCOMPorts();
								
								if (GameData.Config.AimBot.Controller != 1) {
									std::vector<std::pair<std::string, std::string>> portVector;
									for (const auto& port : ports) {
										portVector.push_back({ port, Utils::StringToUTF8(port) });
									}
									std::vector<const char*> items;
									for (const auto& port : portVector) {
										items.push_back(port.second.c_str());
									}

									if (!items.empty())
										ImGui::Combo(U8("COM端口"), &GameData.Config.AimBot.COM, items[0], items.size());
								}
								
								if (ImGui::Button(GameData.Config.AimBot.Connected ? U8("断开连接") : U8("连接"), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 25))) {
									bool Connected = false;
								
									std::string extractedStr;
									if (ports.size() > 0)
								
										extractedStr = Utils::ExtractSubstring(ports[GameData.Config.AimBot.COM], R"(COM(\d+))");
									int COM = 0;
									int number = 0;
									if (extractedStr != "") {
										COM = std::stoi(extractedStr);
										number = std::stoi(extractedStr);
									}
								
									switch (GameData.Config.AimBot.Controller)
									{
									case 0:
										if (!GameData.Config.AimBot.Connected)
										{
											Connected = KmBox::Init(COM);
											if (!Connected)
											{
												ImGui::OpenPopup(U8("连接失败"));
												//notify->AddNotification(U8("提示"), U8("连接失败"), 3000, gui->get_clr(ImColor(255, 0, 0)));
											}
										}
										else {
											KmBox::Close();
											Connected = false;
										}
										break;
									case 1:
										if (!GameData.Config.AimBot.Connected)
										{
											Connected = KmBoxNet::Init(GameData.Config.AimBot.IP, GameData.Config.AimBot.Port, GameData.Config.AimBot.UUID);
											if (!Connected)
											{
												ImGui::OpenPopup(U8("连接失败"));
												//notify->AddNotification(U8("提示"), U8("连接失败"), 3000, gui->get_clr(ImColor(255, 0, 0)));
											}
										}
										else {
											KmBoxNet::Close();
											Connected = false;
										}
										break;
									case 2:
										if (!GameData.Config.AimBot.Connected)
										{
											Connected = Lurker::Init(COM);
											if (!Connected)
											{
												ImGui::OpenPopup(U8("连接失败"));
												//notify->AddNotification(U8("提示"), U8("连接失败"), 3000, gui->get_clr(ImColor(255, 0, 0)));
											}
										}
										else {
											Lurker::Close();
											Connected = false;
										}
										break;
									case 3:
										if (!GameData.Config.AimBot.Connected)
										{
											Connected = MoBox::Init(COM);
											if (!Connected)
											{
												ImGui::OpenPopup(U8("连接失败"));
												//notify->AddNotification(U8("提示"), U8("连接失败"), 3000, gui->get_clr(ImColor(255, 0, 0)));
											}
										}
										else {
											MoBox::Close();
											Connected = false;
										}
										break;
									}
								
									GameData.Config.AimBot.Connected = Connected;
								
									if (Connected)
										//notify->AddNotification(U8("提示"), U8("端口已连接"), 3000, gui->get_clr(ImColor(150, 255, 123)));
										ImGui::OpenPopup(U8("端口已连接"));
									else
										//notify->AddNotification(U8("提示"), U8("连接已断开"), 3000, gui->get_clr(ImColor(150, 255, 123)));
										ImGui::OpenPopup(U8("连接已断开"));
								
								}

								if (ImGui::Button(U8("测试移动"), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 25))) {
									switch (GameData.Config.AimBot.Controller)
									{
									case 0:
										KmBox::Move(0, 100);
										break;
									case 1:
										KmBoxNet::Move(0, 100);
										break;
									case 2:
										Lurker::Move(0, 100);
										break;
									case 3:
										MoBox::Move(0, 100);
										break;
									}
								}
								const char* configItems[]{ U8("配置1"), U8("配置2") };
								
								ImGui::Combo(U8("当前配置"), &GameData.Config.AimBot.ConfigIndex, configItems, IM_ARRAYSIZE(configItems));
								
								ImGui::Keybind(U8("配置热键"), &GameData.Config.AimBot.Configs[GameData.Config.AimBot.ConfigIndex].Key);

							

								if (ImGui::Button(U8("保存配置"), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 25))) {
									Config::Save();

								}

								if (ImGui::Button(U8("退出"), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 25))) {
								
									HWND Progman = FindWindowA("Progman", NULL);
									ShowWindow(Progman, SW_SHOW);
									exit(0);
								
								}


							}
							ImGui::EndChild(true);
							
						}
						ImGui::EndGroup();
						ImGui::SameLine();

						ImGui::BeginGroup();
						{
							ImGui::BeginChild(true, U8("武器设置"), "o", ImVec2(635, 30), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
							{
								ImGui::SetCursorPosY(3);
								
								ImGui::RadioButton(U8("步槍"), &自瞄, 0); ImGui::SameLine();
								ImGui::RadioButton(U8("栓狙"), &自瞄, 1); ImGui::SameLine();
								ImGui::RadioButton(U8("连狙"), &自瞄, 2); ImGui::SameLine();
								ImGui::RadioButton(U8("机枪"), &自瞄, 3); ImGui::SameLine();
								ImGui::RadioButton(U8("冲锋枪"), &自瞄, 4); ImGui::SameLine();
								ImGui::RadioButton(U8("散弹枪"), &自瞄, 5);ImGui::SameLine();
								ImGui::RadioButton(U8("手枪"), &自瞄, 6);ImGui::SameLine();
								ImGui::RadioButton(U8("独立压枪"), &自瞄, 7);ImGui::SameLine();
								//ImGui::RadioButton(U8("模型绘制"), &自瞄, 8);
							}
							ImGui::EndChild(true);
							
							if (自瞄 == 0) {
								Setting(WeapType[0]);
							}
							else if (自瞄 == 1) {
								Setting(WeapType[2]);
							}
							else if (自瞄 == 2) {
							
								Setting(WeapType[1]);
							}
							else if (自瞄 == 3) {
							
								Setting(WeapType[3]);
							}
							else if (自瞄 == 4) {
							
								Setting(WeapType[6]);
							}
							else if (自瞄 == 5) {
							
								Setting(WeapType[5]);
							}
							else if (自瞄 == 6) {
							
								Setting(WeapType[4]);
							}
							else if (自瞄 == 7) {
								
								 
								ImGui::BeginChild(true, U8("压枪幅度"), "o", ImVec2(306, 470));
								{
									ImGui::Checkbox(U8("启用"), &GameData.Config.AimBot.Recoilenanlek);
							
									 ImGui::SliderInt(U8("红点幅度"), &GameData.Config.AimBot.yRecoil[0], 1, 50,  "%.d\xC2\xB0");
							
									 ImGui::SliderInt(U8("二倍幅度"), &GameData.Config.AimBot.yRecoil[1], 1, 50, "%d\xC2\xB0");
							
									 ImGui::SliderInt(U8("三倍幅度"), &GameData.Config.AimBot.yRecoil[2], 1, 50,  "%d\xC2\xB0");
							
									 ImGui::SliderInt(U8("四倍幅度"), &GameData.Config.AimBot.yRecoil[3], 1, 50,  "%d\xC2\xB0");
							
									 ImGui::SliderInt(U8("六倍幅度"), &GameData.Config.AimBot.yRecoil[4], 1, 50,  "%d\xC2\xB0");
							
									 ImGui::SliderInt(U8("八倍幅度"), &GameData.Config.AimBot.yRecoil[5], 1, 50,  "%d\xC2\xB0");
								}
								ImGui::EndChild(true);
								ImGui::SameLine(); ImGui::SetCursorPosY(GetCursorPosY() - 25);
								ImGui::BeginChild(true, U8("压枪延迟"), "o", ImVec2(306, 470));
								{
									ImGui::SliderInt(U8("红点延迟"), &GameData.Config.AimBot.interval[0], 1, 10, "%d\xC2\xB0");

									ImGui::SliderInt(U8("二倍延迟"), &GameData.Config.AimBot.interval[1], 1, 10, "%d\xC2\xB0");

									ImGui::SliderInt(U8("三倍延迟"), &GameData.Config.AimBot.interval[2], 1, 10,  "%d\xC2\xB0");

									ImGui::SliderInt(U8("四倍延迟"), &GameData.Config.AimBot.interval[3], 1, 10,  "%d\xC2\xB0");

									ImGui::SliderInt(U8("六倍延迟"), &GameData.Config.AimBot.interval[4], 1, 10,  "%d\xC2\xB0");

									ImGui::SliderInt(U8("八倍延迟"), &GameData.Config.AimBot.interval[5], 1, 10,  "%d\xC2\xB0");
								}
								ImGui::EndChild(true);
								ImGui::SameLine(); ImGui::SetCursorPosY(GetCursorPosY() - 95);
								ImGui::BeginChild(true, U8("部位选择"), "o", ImVec2(295, 540));
								{

									ImGui::SetCursorPos(ImVec2(12, 65));
									ImGui::Image(texture::playermoder, ImVec2(258, 463), ImVec2(0, 0), ImVec2(1, 1), ImColor(c::image.x, c::image.y, c::image.z, c::image.w));

									static bool hitbox[20];

									ImGui::SetCursorPos(ImVec2(134, 99));
									ImGui::Checkbox_hitbox("BONE 1", &hitbox[0]);
									ImGui::SetCursorPos(ImVec2(134, 123));
									ImGui::Checkbox_hitbox("BONE 2", &hitbox[1]);
									ImGui::SetCursorPos(ImVec2(134, 147));
									ImGui::Checkbox_hitbox("BONE 3", &hitbox[2]);
									ImGui::SetCursorPos(ImVec2(134, 177));
									ImGui::Checkbox_hitbox("BONE 4", &hitbox[3]);
									ImGui::SetCursorPos(ImVec2(134, 227));
									ImGui::Checkbox_hitbox("BONE 5", &hitbox[4]);
									ImGui::SetCursorPos(ImVec2(134, 257));
									ImGui::Checkbox_hitbox("BONE 6", &hitbox[5]);
									ImGui::SetCursorPos(ImVec2(86, 152));
									ImGui::Checkbox_hitbox("BONE 7", &hitbox[6]);
									ImGui::SetCursorPos(ImVec2(65, 217));
									ImGui::Checkbox_hitbox("BONE 8", &hitbox[7]);
									ImGui::SetCursorPos(ImVec2(52, 272));
									ImGui::Checkbox_hitbox("BONE 9", &hitbox[8]);
									ImGui::SetCursorPos(ImVec2(186, 152));
									ImGui::Checkbox_hitbox("BONE 10", &hitbox[9]);
									ImGui::SetCursorPos(ImVec2(205, 217));
									ImGui::Checkbox_hitbox("BONE 11", &hitbox[10]);
									ImGui::SetCursorPos(ImVec2(218, 272));
									ImGui::Checkbox_hitbox("BONE 12", &hitbox[11]);
									ImGui::SetCursorPos(ImVec2(104, 282));
									ImGui::Checkbox_hitbox("BONE 13", &hitbox[12]);
									ImGui::SetCursorPos(ImVec2(101, 355));
									ImGui::Checkbox_hitbox("BONE 14", &hitbox[13]);
									ImGui::SetCursorPos(ImVec2(94, 447));
									ImGui::Checkbox_hitbox("BONE 15", &hitbox[14]);
									ImGui::SetCursorPos(ImVec2(165, 282));
									ImGui::Checkbox_hitbox("BONE 16", &hitbox[15]);
									ImGui::SetCursorPos(ImVec2(169, 355));
									ImGui::Checkbox_hitbox("BONE 17", &hitbox[16]);
									ImGui::SetCursorPos(ImVec2(174, 447));
									ImGui::Checkbox_hitbox("BONE 18", &hitbox[17]);


								}
								ImGui::EndChild(true);
							
							}
							else if (自瞄 == 8) {
								
								/*ImGui::BeginChild(true, U8("模型绘制"), "o", ImVec2(627, 470));
								{

									ImGui::Checkbox(U8("模型绘制"), &GameData.Config.ESP.PhysXDebug);

									 ImGui::SliderInt(U8("加载距离"), &GameData.Config.ESP.PhysxLoadRadius, 200, 1000,  "%d\xC2\xB0");

									 ImGui::SliderInt(U8("静态刷新"), &GameData.Config.ESP.PhysxStaticRefreshInterval, 300, 3000, "%d\xC2\xB0");

									 ImGui::SliderInt(U8("动态刷新"), &GameData.Config.ESP.PhysxDynamicRefreshInterval, 10, 3000, "%d\xC2\xB0");

									 ImGui::SliderInt(U8("刷新阈值"), &GameData.Config.ESP.PhysxRefreshLimit, 0, 3000,  "%d\xC2\xB0");

								}
								ImGui::EndChild(true);*/
								ImGui::SameLine(); ImGui::SetCursorPosY(GetCursorPosY() - 95);
								ImGui::BeginChild(true, U8("部位选择"), "o", ImVec2(295, 540));
								{

									ImGui::SetCursorPos(ImVec2(12, 65));
									ImGui::Image(texture::playermoder, ImVec2(258, 463), ImVec2(0, 0), ImVec2(1, 1), ImColor(c::image.x, c::image.y, c::image.z, c::image.w));

									static bool hitbox[20];

									ImGui::SetCursorPos(ImVec2(134, 99));
									ImGui::Checkbox_hitbox("BONE 1", &hitbox[0]);
									ImGui::SetCursorPos(ImVec2(134, 123));
									ImGui::Checkbox_hitbox("BONE 2", &hitbox[1]);
									ImGui::SetCursorPos(ImVec2(134, 147));
									ImGui::Checkbox_hitbox("BONE 3", &hitbox[2]);
									ImGui::SetCursorPos(ImVec2(134, 177));
									ImGui::Checkbox_hitbox("BONE 4", &hitbox[3]);
									ImGui::SetCursorPos(ImVec2(134, 227));
									ImGui::Checkbox_hitbox("BONE 5", &hitbox[4]);
									ImGui::SetCursorPos(ImVec2(134, 257));
									ImGui::Checkbox_hitbox("BONE 6", &hitbox[5]);
									ImGui::SetCursorPos(ImVec2(86, 152));
									ImGui::Checkbox_hitbox("BONE 7", &hitbox[6]);
									ImGui::SetCursorPos(ImVec2(65, 217));
									ImGui::Checkbox_hitbox("BONE 8", &hitbox[7]);
									ImGui::SetCursorPos(ImVec2(52, 272));
									ImGui::Checkbox_hitbox("BONE 9", &hitbox[8]);
									ImGui::SetCursorPos(ImVec2(186, 152));
									ImGui::Checkbox_hitbox("BONE 10", &hitbox[9]);
									ImGui::SetCursorPos(ImVec2(205, 217));
									ImGui::Checkbox_hitbox("BONE 11", &hitbox[10]);
									ImGui::SetCursorPos(ImVec2(218, 272));
									ImGui::Checkbox_hitbox("BONE 12", &hitbox[11]);
									ImGui::SetCursorPos(ImVec2(104, 282));
									ImGui::Checkbox_hitbox("BONE 13", &hitbox[12]);
									ImGui::SetCursorPos(ImVec2(101, 355));
									ImGui::Checkbox_hitbox("BONE 14", &hitbox[13]);
									ImGui::SetCursorPos(ImVec2(94, 447));
									ImGui::Checkbox_hitbox("BONE 15", &hitbox[14]);
									ImGui::SetCursorPos(ImVec2(165, 282));
									ImGui::Checkbox_hitbox("BONE 16", &hitbox[15]);
									ImGui::SetCursorPos(ImVec2(169, 355));
									ImGui::Checkbox_hitbox("BONE 17", &hitbox[16]);
									ImGui::SetCursorPos(ImVec2(174, 447));
									ImGui::Checkbox_hitbox("BONE 18", &hitbox[17]);

								
								}
								ImGui::EndChild(true);

							}
		
						}
						ImGui::EndGroup();

					}
					ImGui::EndChild();

				}
				else if (active_tab == 1)
				{

					 ImGui::SetCursorPos(ImVec2(region.x - (tab_alpha * region.x - 18), 190 - (anim * 120)));

					 ImGui::BeginChild(false, "Child", "o", ImVec2(1145, 565));
					 {
						 ImGui::BeginGroup();
						 {
							 ImGui::BeginChild(true, U8("基础设置"), "e", ImVec2(260, 250));//365
							 {
								 ImGui::Checkbox(U8("启动透视"), &GameData.Config.ESP.Enable);

								 ImGui::Checkbox(U8("危险预警"), &GameData.Config.ESP.DangerWarning);

								 ImGui::Checkbox(U8("文字阴影"), &GameData.Config.ESP.Stroke);

								 ImGui::Checkbox(U8("锁定不显"), &GameData.Config.ESP.LockedHiddenBones);
							 
								 ImGui::Checkbox(U8("可视检测"), &GameData.Config.ESP.VisibleCheck);

								 ImGui::Checkbox(U8("锁定变色"), &GameData.Config.ESP.soudingbianse);

								 ImGui::Checkbox(U8("官方走狗"), &GameData.Config.ESP.Partner);

							 }
							 ImGui::EndChild(true);

							 ImGui::BeginChild(true, U8("数值设置"), "a", ImVec2(260, 250));//365
							 {
								 ImGui::SliderInt(U8("透视距离"), &GameData.Config.ESP.DistanceMax, 0, 1000,  "%d\xC2\xB0");
							 
								 ImGui::SliderInt(U8("信息距离"), &GameData.Config.ESP.InfoDistanceMax, 0, 1000,  "%d\xC2\xB0");
							 
								 ImGui::SliderInt(U8("武器距离"), &GameData.Config.ESP.WeaponDistanceMax, 0, 1000,  "%d\xC2\xB0");
							 
								 ImGui::SliderInt(U8("骨骼粗细"), &GameData.Config.ESP.SkeletonWidth, 1, 5, "%d\xC2\xB0");
							 
								 ImGui::SliderInt(U8("信息大小"), &GameData.Config.ESP.FontSize, 8, 28,  "%d\xC2\xB0");
							 }
							 ImGui::EndChild(true);

						 }
						 ImGui::EndGroup();

						 ImGui::SameLine();

						 ImGui::BeginGroup();
						 {

							 ImGui::BeginChild(true, U8("颜色设置"), "e", ImVec2(240, 540));//365
							 {

								 ImGui::ColorEdit4(U8("可视颜色"), GameData.Config.ESP.Color.Visible.Skeleton, picker_flags);

								 ImGui::ColorEdit4(U8("可视信息"), GameData.Config.ESP.Color.Visible.Info, picker_flags);

								 ImGui::ColorEdit4(U8("掩体颜色"), GameData.Config.ESP.Color.Default.Skeleton, picker_flags);
							 
								 ImGui::ColorEdit4(U8("掩体信息"), GameData.Config.ESP.Color.Default.Info, picker_flags);
							 
								 ImGui::ColorEdit4(U8("人机颜色"), GameData.Config.ESP.Color.AI.Skeleton, picker_flags);

								 ImGui::ColorEdit4(U8("人机信息"), GameData.Config.ESP.Color.AI.Info, picker_flags);
							 
								 ImGui::ColorEdit4(U8("危险颜色"), GameData.Config.ESP.Color.Dangerous.Skeleton, picker_flags);//危险

								 ImGui::ColorEdit4(U8("危险信息"), GameData.Config.ESP.Color.Dangerous.Info, picker_flags);//危险
							 
								 ImGui::ColorEdit4(U8("倒地颜色"), GameData.Config.ESP.Color.Groggy.Skeleton, picker_flags);

								 ImGui::ColorEdit4(U8("倒地信息"), GameData.Config.ESP.Color.Groggy.Info, picker_flags);

								 ImGui::ColorEdit4(U8("被瞄颜色"), GameData.Config.ESP.Color.Ray.Line, picker_flags);

								 ImGui::ColorEdit4(U8("锁定颜色"), GameData.Config.ESP.Color.aim.Skeleton, picker_flags);
								 
								 ImGui::ColorEdit4(U8("黑名单骨骼"), GameData.Config.ESP.Color.Blacklist.Skeleton, picker_flags);

								 ImGui::ColorEdit4(U8("黑名单信息"), GameData.Config.ESP.Color.Blacklist.Info, picker_flags);
							 
								 ImGui::ColorEdit4(U8("白名单骨骼"), GameData.Config.ESP.Color.Whitelist.Skeleton, picker_flags);//没有
							
								 ImGui::ColorEdit4(U8("白名单信息"), GameData.Config.ESP.Color.Whitelist.Info, picker_flags);//没有


								 ImGui::ColorEdit4(U8("走狗颜色"), GameData.Config.ESP.Color.Partner.Skeleton, picker_flags);

								 ImGui::ColorEdit4(U8("走狗信息"), GameData.Config.ESP.Color.Partner.Info, picker_flags);

							 }
							 ImGui::EndChild(true);


						 }
						 ImGui::EndGroup();

						 ImGui::SameLine();

						 ImGui::BeginGroup();
						 {

							 ImGui::BeginChild(true, U8("透视开关"), "e", ImVec2(260, 540));//365
							 {

								 ImGui::Checkbox(U8("玩家血条"), &GameData.Config.ESP.health_bar);
								
								 ImGui::Checkbox(U8("玩家方框"), &GameData.Config.ESP.DisplayFrame);
								 
								 ImGui::Checkbox(U8("玩家骨骼"), &GameData.Config.ESP.Skeleton);

								 ImGui::Checkbox(U8("玩家名字"), &GameData.Config.ESP.Nickname);
								 
								 ImGui::Checkbox(U8("玩家队标"), &GameData.Config.ESP.TeamID);
								 
								 ImGui::Checkbox(U8("玩家战队"), &GameData.Config.ESP.ClanName);
								 
								 ImGui::Checkbox(U8("玩家等级"), &GameData.Config.ESP.等级);

								 ImGui::Checkbox(U8("玩家距离"), &GameData.Config.ESP.Dis);
								 
								 ImGui::Checkbox(U8("玩家血量"), &GameData.Config.ESP.Health);

								 ImGui::Checkbox(U8("掉线显示"), &GameData.Config.ESP.ShowInfos);
							
								 ImGui::Checkbox(U8("玩家手持"), &GameData.Config.ESP.Weapon);

								 ImGui::Checkbox(U8("玩家击杀"), &GameData.Config.ESP.击杀);

								 ImGui::Checkbox(U8("玩家伤害"), &GameData.Config.ESP.伤害);

								 ImGui::Checkbox(U8("被瞄射线"), &GameData.Config.ESP.TargetedRay);

								 ImGui::Checkbox(U8("头部骨骼"), &GameData.Config.ESP.HeadDrawing);

								 ImGui::Checkbox(U8("段位图标"), &GameData.Config.ESP.showIcon);

								 ImGui::Checkbox(U8("玩家观战"), &GameData.Config.ESP.观战);

								 ImGui::Checkbox(U8("玩家KDA"), &GameData.Config.ESP.KDA);								

								 const char* items[]{ U8("不查战绩"), U8("TPP单人"), U8("TPP小队"), U8("FPP单人"), U8("FPP小队") };
								 ImGui::Combo(U8("段位数据"), &GameData.Config.PlayerList.RankMode, items, IM_ARRAYSIZE(items));
								 
								 
								 const char* XueTiaoWEizhi[]{ U8("顶部显示"), U8("左侧显示") };
								 ImGui::Combo(U8("血条位置"), &GameData.Config.ESP.XueTiaoWEizhi, XueTiaoWEizhi, IM_ARRAYSIZE(XueTiaoWEizhi));

								 const char* HealthBarStyle[]{ U8("彩虹血条"), U8("单色血条"), U8("单色缩放"), U8("彩虹缩放") };
								 ImGui::Combo(U8("血条样式"), &GameData.Config.ESP.HealthBarStyle, HealthBarStyle, IM_ARRAYSIZE(HealthBarStyle));

							 }
							 ImGui::EndChild(true);


						 }
						 ImGui::EndGroup();

						 ImGui::SameLine();

						 ImGui::BeginChild(true, U8("ESP预览"), "o", ImVec2(330, 540));
						 {
							 
							 if (GameData.Config.Window.Players) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(149.0f / 255.0f, 1.0f / 255.0f, 247.f / 255.0f, 200.f / 255.f));
							 ImGui::Keybind(U8("段位列表"), &GameData.Config.Overlay.rankList);
							 if (ImGui::Button(GameData.Config.Window.Players ? U8("关闭列表") : U8("打开列表"), ImVec2(ImGui::GetContentRegionMax().x - 20, 20))) {
							 
							 	GameData.Config.Window.Players = !GameData.Config.Window.Players;
							 
							 };

							 ImVec2 childpos = ImGui::GetCursorScreenPos() + ImVec2(0, 20);
							 ImVec2 childsize = GetContentRegionMax();


							 /*if (skeleton)
								 GetWindowDrawList()->AddRect(childpos + ImVec2(90, 35), childpos + ImVec2(90, 35) + ImVec2(155, 310), ImColor(box_col[0], box_col[1], box_col[2]));
							 if (box)
								 GetWindowDrawList()->AddRect(childpos + ImVec2(90, 35), childpos + ImVec2(90, 35) + ImVec2(155, 310), ImColor(box_col[0], box_col[1], box_col[2]));

							 if (healthbar)
								 GetWindowDrawList()->AddRectFilled(childpos + ImVec2(88, 25), childpos + ImVec2(90, 25) + ImVec2(155, 4), ImColor(45, 255, 45));

							 if (teamid)
							 {
								 GetWindowDrawList()->AddCircleFilled(childpos + ImVec2(70, 27), 10, ImColor(175, 65, 255));
								 GetWindowDrawList()->AddText(childpos + ImVec2(67, 18), ImColor(255, 255, 255), "7");
							 }

							 if (name)
								 GetWindowDrawList()->AddText(childpos + ImVec2(148, 7), ImColor(255, 255, 255), "KAKA-PUBG");
							 if (Clan)
								 GetWindowDrawList()->AddText(childpos + ImVec2(110, 7), ImColor(255, 255, 255), "[DMA]");

							 if (rank)
							 {
								 GetWindowDrawList()->AddText(childpos + ImVec2(140, 28), ImColor(255, 255, 255), "");
								 GetWindowDrawList()->AddImage(texture::rank, childpos + ImVec2(117, -11), childpos + ImVec2(117, -11) + ImVec2(20, 20));
							 }

							 if (kda)
								 GetWindowDrawList()->AddText(childpos + ImVec2(145 + CalcTextSize("").x, -7), ImColor(255, 255, 255), "大师 6.00");

							 if (weapon_icon_b)
								 GetWindowDrawList()->AddImage(texture::weapon_image, childpos + ImVec2(120, -30), childpos + ImVec2(120, -30) + ImVec2(95, 26));

							 if (distance)
								 GetWindowDrawList()->AddText(childpos + ImVec2(150, 350), ImColor(255, 255, 255), "1337M");

							 if (health)
								 GetWindowDrawList()->AddText(childpos + ImVec2(150, 365), ImColor(255, 255, 255), "100HP");

							 if (level)
								 GetWindowDrawList()->AddText(childpos + ImVec2(148, 380), ImColor(255, 255, 255), "LV.595");
							 if (dmg)
								 GetWindowDrawList()->AddText(childpos + ImVec2(154, 395), ImColor(255, 255, 255), "S.99");
*/



						 }
						 ImGui::EndChild(true);

					 }
					 ImGui::EndChild();


				}
				 else if (active_tab == 2)
				 {

					 ImGui::SetCursorPos(ImVec2(region.x - (tab_alpha * region.x - 18), 190 - (anim * 120)));

					 ImGui::BeginChild(false, "Child", "o", ImVec2(1145, 565));
					 {
						 ImGui::BeginGroup();
						 {
							 ImGui::BeginChild(true, U8("物资开关"), "e", ImVec2(365, 215));
							 {
								 ImGui::Checkbox(U8("物品透视"), &GameData.Config.Item.Enable);
								 
								 ImGui::Checkbox(U8("物品叠加"), &GameData.Config.Item.Combination);

								 ImGui::Checkbox(U8("配件过滤"), &GameData.Config.Item.AccessoriesFilter);
								 
								 ImGui::Checkbox(U8("显示图标"), &GameData.Config.Item.ShowIcon);

								 ImGui::Keybind(U8("A组热键"), &GameData.Config.Item.GroupAKey);

								 ImGui::Keybind(U8("B组热键"), &GameData.Config.Item.GroupBKey);

								 ImGui::Keybind(U8("C组热键"), &GameData.Config.Item.GroupCKey);

								 ImGui::Keybind(U8("D组热键"), &GameData.Config.Item.GroupDKey);

								 ImGui::SliderInt(U8("物品距离"), &GameData.Config.Item.DistanceMax, 0, 500, "%d\xC2\xB0");

								 ImGui::SliderInt(U8("物品字体/图标比例"), &GameData.Config.Item.FontSize, 1, 40, "%d\xC2\xB0");

								 ImGui::Checkbox(U8("空投显示"), &GameData.Config.AirDrop.Enable);

								 ImGui::Keybind(U8("空投热键"), &GameData.Config.AirDrop.EnableKey);

								 ImGui::Checkbox(U8("显示空投物品"), &GameData.Config.AirDrop.ShowItems);

								 ImGui::SliderInt(U8("显示空投距离"), &GameData.Config.AirDrop.DistanceMax, 0, 1000, "%d\xC2\xB0");

								 ImGui::SliderInt(U8("空投字体"), &GameData.Config.AirDrop.FontSize, 1, 28, "%d\xC2\xB0");

								 ImGui::Checkbox(U8("载具显示"), &GameData.Config.Vehicle.Enable);

								 ImGui::Keybind(U8("快捷热键"), &GameData.Config.Vehicle.EnableKey);

								 ImGui::Checkbox(U8("载具血量"), &GameData.Config.Vehicle.Health);

								 ImGui::Checkbox(U8("载具油量"), &GameData.Config.Vehicle.Durability);

								 ImGui::SliderInt(U8("载具距离"), &GameData.Config.Vehicle.DistanceMax, 0, 1000, "%d\xC2\xB0");

								 ImGui::SliderInt(U8("载具字体"), &GameData.Config.Vehicle.FontSize, 1, 28, "%d\xC2\xB0");

								 ImGui::Checkbox(U8("盒子显示"), &GameData.Config.DeadBox.Enable);

								 ImGui::Keybind(U8("快捷热键"), &GameData.Config.DeadBox.EnableKey);

								 ImGui::Checkbox(U8("显示物品"), &GameData.Config.DeadBox.ShowItems);

								 ImGui::SliderInt(U8("盒子距离"), &GameData.Config.DeadBox.DistanceMax, 0, 200,"%d\xC2\xB0");

								 ImGui::SliderInt(U8("盒子字体"), &GameData.Config.DeadBox.FontSize, 1, 28, "%d\xC2\xB0");
							 }
							 ImGui::EndChild(true);

							 ImGui::BeginChild(true, U8("颜色设置"), "a", ImVec2(365, 285));
							 {
								
								ImGui::ColorEdit4(U8("分组A颜色"), GameData.Config.Item.GroupAColor, picker_flags);
																								
								ImGui::ColorEdit4(U8("分组B颜色"), GameData.Config.Item.GroupBColor, picker_flags);
																							
								ImGui::ColorEdit4(U8("分组C颜色"), GameData.Config.Item.GroupCColor, picker_flags);
																								
								ImGui::ColorEdit4(U8("分组D颜色"), GameData.Config.Item.GroupDColor, picker_flags);

								ImGui::ColorEdit4(U8("盒子颜色"), GameData.Config.DeadBox.Color, picker_flags);								

								ImGui::ColorEdit4(U8("空投颜色"), GameData.Config.AirDrop.Color, picker_flags);

								ImGui::ColorEdit4(U8("载具颜色"), GameData.Config.Vehicle.Color, picker_flags);

								ImGui::ColorEdit4(U8("载具血量"), GameData.Config.Vehicle.Fuelbarcolor, picker_flags);

								ImGui::ColorEdit4(U8("载具油量"), GameData.Config.Vehicle.Healthbarcolor, picker_flags);

							 }
							 ImGui::EndChild(true);

						 }
						 ImGui::EndGroup();

						 ImGui::SameLine();

						 ImGui::BeginGroup();
						 {

							 ImGui::BeginChild(true, U8("物资分组"), "e", ImVec2(745, 58), false, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
							 {
								 static int item_world;
								 ImGui::RadioButton(U8("智能物资"), &物品, 0); ImGui::SameLine();//30109433
								 ImGui::RadioButton(U8("步枪"), &物品, 1); ImGui::SameLine();
								 ImGui::RadioButton(U8("狙击枪"), &物品, 2); ImGui::SameLine();
								 ImGui::RadioButton(U8("机/手枪"), &物品,3); ImGui::SameLine();
								 ImGui::RadioButton(U8("冲/喷"), &物品, 4); ImGui::SameLine();
								 ImGui::RadioButton(U8("防/背"), &物品, 5); ImGui::SameLine();
								 ImGui::RadioButton(U8("配件"), &物品, 6); ImGui::SameLine();
								 ImGui::RadioButton(U8("药/投"), &物品, 7); ImGui::SameLine();
								 ImGui::RadioButton(U8("子弹"), &物品, 8); ImGui::SameLine();
								 ImGui::RadioButton(U8("其他"), &物品, 9); ImGui::SameLine();

							 }
							 ImGui::EndChild(true);

							 if (物品 == 0) {

								 ImGui::BeginChild(true, U8("智能物资过滤"), "e", ImVec2(745, 442));
								 {
									 ImGui::BeginGroup();
									 {
 
										ImGui::Checkbox(U8("开启"), &GameData.Config.Item.FilterEnable);
										ImGui::SliderInt(U8("医疗箱"), &GameData.Config.Item.Medicalkit, 1, 8);
										ImGui::SliderInt(U8("急救包"), &GameData.Config.Item.FirstAidKit, 1, 8);
										ImGui::SliderInt(U8("绷带"), &GameData.Config.Item.Bandage, 1, 8);
										ImGui::SliderInt(U8("肾上腺激素"), &GameData.Config.Item.Epinephrine, 1, 8);
										ImGui::SliderInt(U8("止疼药"), &GameData.Config.Item.PainKiller, 1, 8);
										ImGui::SliderInt(U8("能量饮料"), &GameData.Config.Item.EnergyDrink, 1, 8);
										ImGui::SliderInt(U8("手雷"), &GameData.Config.Item.Grenade, 1, 8);
										ImGui::SliderInt(U8("闪光弹"), &GameData.Config.Item.FlashGrenade, 1, 8);
										ImGui::SliderInt(U8("烟雾弹"), &GameData.Config.Item.SmokeGrenade, 1, 8);
										ImGui::SliderInt(U8("燃烧瓶"), &GameData.Config.Item.MolotovGrenade, 1, 8);
										ImGui::SliderInt(U8("蓝圈手雷"), &GameData.Config.Item.BluezoneGrenade, 1, 8);
										
									 }
									 ImGui::EndGroup();

								 }
								 ImGui::EndChild(true);

							 
							 }
							 else
							 {
								WeaponType Type;
								switch (物品)
								{
								case 1:
									Type = WeaponType::AR;
									break;
								case 2:
									Type = WeaponType::DMR;
									break;
								case 3:
									Type = WeaponType::LMG;
									break;
								case 4:
									Type = WeaponType::SMG;
									break;
								case 5:
									Type = WeaponType::Armor;
									break;
								case 6:
									Type = WeaponType::Sight;
									break;
								case 7:
									Type = WeaponType::Drug;
									break;
								case 8:
									Type = WeaponType::Bullet;
									break;
								case 9:
									Type = WeaponType::Other;
									break;
								}
								
								ImGui::BeginChild(true, U8("物资选择分组"), "e", ImVec2(745, 442));
								{
									static char search_buffer[256] = "";
									//ImGui::InputTextEx(U8("搜索..."), search_buffer, sizeof(search_buffer), ImVec2(330, 35), NULL, 0, 0);//func->c_page.sub_page
									size_t index =1;

									if (ImGui::BeginTable("table_title_null", 3, ImGuiTableFlags_Borders)) {

										ImGui::TableSetupColumn(U8("物资图标"));
										ImGui::TableSetupColumn(U8("物资名称"));
										ImGui::TableSetupColumn(U8("物资分组"));
										ImGui::TableHeadersRow();

										for (int row = 0; row < index; row++) {
											for (auto& pair : GameData.Config.Item.Lists) {

												const std::string& key = pair.first;
												ItemDetail& detail = pair.second;

												if (detail.Type != Type && 物品 != 1 && 物品 != 2 && 物品 != 3 && 物品 != 4 && 物品 != 5 && 物品 != 6 && 物品 != 7 && 物品 != 8 && 物品 != 9) {
													continue;
												}
												else if (detail.Type != Type && 物品 == 6)
												{
													if (detail.Type != WeaponType::Sight && detail.Type != WeaponType::Magazine && detail.Type != WeaponType::Muzzle && detail.Type != WeaponType::GunButt && detail.Type != WeaponType::Grip)
														continue;
												}
												else if (detail.Type != Type && 物品 == 3)//机/手
												{
													if (detail.Type != WeaponType::HG && detail.Type != WeaponType::LMG)
														continue;
												}
												else if (detail.Type != Type && 物品 == 1)//步枪
												{
													if (detail.Type != WeaponType::AR)
														continue;
												}
												else if (detail.Type != Type && 物品 == 2)//狙击枪
												{
													if (detail.Type != WeaponType::DMR && detail.Type != WeaponType::SR)
														continue;
												}
												else if (detail.Type != Type && 物品 == 4)//冲/喷
												{
													if (detail.Type != WeaponType::SMG && detail.Type != WeaponType::SG)
														continue;
												}
												else if (detail.Type != Type && 物品 == 5)//防具
												{
													if (detail.Type != WeaponType::Armor)
														continue;
												}
												else if (detail.Type != Type && 物品 == 7)//药/投
												{
													if (detail.Type != WeaponType::Drug && detail.Type != WeaponType::Grenade)
														continue;
												}

												else if (detail.Type != Type && 物品 == 8)//子弹
												{
													if (detail.Type != WeaponType::Bullet)
														continue;
												}
												else if (detail.Type != Type && 物品 == 9)//其他
												{
													if (detail.Type != WeaponType::Other)
														continue;
												}


												std::string displayName = Utils::StringToUTF8(detail.DisplayName);
												std::string searchKeyword = Utils::StringToUTF8(search_buffer);

												ImGui::TableNextRow();
												ImGui::TableSetColumnIndex(0);
												{
													std::string Space = "";
													ImGui::TextUnformatted(Space.c_str());

													std::string ItemName = pair.first;
													std::string IconUrl = "Assets/image/All/" + ItemName + ".png";
													//Utils::Log(1, "GameData.WorldTimeSeconds %f", ItemName.c_str());
													SetCursorPosY(GetCursorPosY() - 22);
													if (GImGuiTextureMap[IconUrl].Width > 0) {
														ImGui::Image((ImTextureID)GImGuiTextureMap[IconUrl].Texture, ImVec2(30, 30));
													}

												}
												ImGui::SameLine();
												SetCursorPosY(GetCursorPosY() + 8);

												std::string TempBuff = search_buffer;

												if (searchKeyword.length() > 2 && displayName.find(TempBuff) == std::string::npos)
													continue;
												std::string name = displayName;
												//std::string name = displayName + "##" + std::to_string(index);
												ImGui::TableSetColumnIndex(1);
												SetCursorPosY(GetCursorPosY() + 8);
												ImGui::Text(name.c_str(), row + 1);


												ImGui::TableSetColumnIndex(2);

												ImGui::PushID(key.c_str());  // 使用物资键作为唯一ID

												const char* Group_item_chinese[] = { U8("未选择"), U8("分组A"), U8("分组B"), U8("分组C") , U8("分组D") };
												//SetCursorPosY(GetCursorPosY() - 22);
												ImGui::SetNextItemWidth(200);
												if (ImGui::Combo_popup("##COMBO", &detail.Group, Group_item_chinese, IM_ARRAYSIZE(Group_item_chinese)));
												
												ImGui::PopID();


											}



										}
										
										ImGui::EndTable();
									}
								}
								ImGui::EndChild(true);
							 }

						 }
						 ImGui::EndGroup();

					 }
					 ImGui::EndChild();

				 }
				 else if (active_tab == 3)
				 {
					 ImGui::SetCursorPos(ImVec2(region.x - (tab_alpha * region.x - 18), 190 - (anim * 120)));
					 ImGui::BeginChild(false, "Child", "o", ImVec2(1145, 565));
					 {
						 ImGui::BeginGroup();
						 {
							 ImGui::BeginChild(true, U8("大地图"), "a", ImVec2(350, 500));
							 {
								 ImGui::Checkbox(U8("大地图玩家"), &GameData.Config.Radar.Main.ShowPlayer);
								 
								 ImGui::Checkbox(U8("大地图载具"), &GameData.Config.Radar.Main.ShowVehicle);
								 
								 ImGui::Checkbox(U8("大地图空投"), &GameData.Config.Radar.Main.ShowAirDrop);
								
								 ImGui::Checkbox(U8("大地图死亡"), &GameData.Config.Radar.Main.ShowAirDrop);
								 
								 ImGui::SliderFloat(U8("大地图比例"), &GameData.Config.Radar.Main.Map_size, 5.0f, 14.0f,"%.1f [F]");
								 
								 ImGui::SliderInt(U8("大地图图标比例"), &GameData.Config.Radar.Main.FontSize, 1, 20, "%d\xC2\xB0");
								 
							 }
							 ImGui::EndChild(true);
						 }
						 ImGui::EndGroup();

						 ImGui::SameLine();

						 ImGui::BeginGroup();
						 {
							 ImGui::BeginChild(true, U8("小地图"), "a", ImVec2(350, 500));
							 {
								 ImGui::Checkbox(U8("小地图玩家"), &GameData.Config.Radar.Mini.ShowPlayer);
								 
								 ImGui::Checkbox(U8("小地图载具"), &GameData.Config.Radar.Mini.ShowVehicle);
								 
								 ImGui::Checkbox(U8("小地图空投"), &GameData.Config.Radar.Mini.ShowAirDrop);
								 
								 ImGui::Checkbox(U8("小地图死亡"), &GameData.Config.Radar.Main.ShowAirDrop);
								 
								 ImGui::SliderFloat(U8("小地图比例"), &GameData.Config.Radar.Mini.Map_size, 5.0f, 14.0f, "%.1f [F]");
								 
								 ImGui::SliderInt(U8("小地图图标比例"), &GameData.Config.Radar.Mini.FontSize, 1, 20, "%d\xC2\xB0");
								 
							 }
							 ImGui::EndChild(true);

						 }
						 ImGui::EndGroup();

						 ImGui::SameLine();

						 ImGui::BeginGroup();
						 {
							 ImGui::BeginChild(true, U8("网页雷达"), "a", ImVec2(350, 500));
							 {
								
								 char address[512];
								 snprintf(address, sizeof(address), "http://%s:7891", GameData.Config.ESP.服务器IP);

								 ImGui::TextColored(c::text::text, U8("IP填服务器公网IP(如需网页雷达,可自行购买服务器)"));
								 ImGui::Separator();
								 ImGui::InputTextEx("##WIP/", U8("服务器IP:"), GameData.Config.ESP.服务器IP, IM_ARRAYSIZE(GameData.Config.ESP.服务器IP), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 25), ImGuiInputTextFlags_NoLabel);
								 ImGui::TextColored(c::text::text, U8("服务器运行雷达服务端后把生成网址分享给队友"));
								 ImGui::Separator();
								 ImGui::InputTextEx("##GIP/", U8("观看地址:"), address, IM_ARRAYSIZE(GameData.Config.ESP.服务器IP), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 25), ImGuiInputTextFlags_NoLabel);
								 // 添加链接按钮
								 if (ImGui::Button(U8("打开浏览器"), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 25))) {
									 OpenLink(address);
									 // 在这里添加打开链接的逻辑
								 }
								 ImGui::Separator();
								 ImGui::TextColored(c::text::text, U8("在打开的网页中点击 设置>服务器>"));
								 ImGui::Separator();
								 ImGui::TextColored(c::text::text, U8("输入公网IP+:7891/WS"));
							 }
							 ImGui::EndChild(true);
						 }
						 ImGui::EndGroup();

					 }
					 ImGui::EndChild();
					}
				 else if (active_tab == 4)
				 {

					 ImGui::SetCursorPos(ImVec2(region.x - (tab_alpha * region.x - 18), 190 - (anim * 120)));

					 ImGui::BeginChild(false, "Child", "o", ImVec2(1145, 565));
					 {
						 ImGui::BeginGroup();
						 {
							 ImGui::BeginChild(true, U8("主题色"), "e", ImVec2(220, 245), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
							 {
								 ImGui::ColorPicker4("Accent", color);
							 }
							 ImGui::EndChild(true);

							 ImGui::BeginChild(true, U8("机器人颜色"), "e", ImVec2(220, 245), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
							 {
								 ImGui::ColorPicker4("Menu Model", (float*)&c::image);
							 }
							 ImGui::EndChild(true);


						 }
						 ImGui::EndGroup();

						 ImGui::SameLine();

						 ImGui::BeginGroup();
						 {
							 ImGui::BeginChild(true, U8("手雷预警"), "e", ImVec2(445, 295));
							 {
								 ImGui::Checkbox(U8("爆炸提示"), &GameData.Config.Project.Enable);
								 
								 ImGui::Checkbox(U8("爆炸范围"), &GameData.Config.Project.FOV);
								 
								 ImGui::Checkbox(U8("高抛预判"), &GameData.Config.Project.GrenadePrediction);

								 ImGui::Checkbox(U8("文本计时"), &GameData.Config.Project.TextShowChareTime);

								 ImGui::Checkbox(U8("掐雷计时"), &GameData.Config.Project.ShowChareTime);

								 ImGui::Checkbox(U8("血条计时"), &GameData.Config.Project.BarShowChareTime);
								 
								 ImGui::ColorEdit4(U8("提示颜色"), GameData.Config.Project.ChareColor, picker_flags);
								 
								 ImGui::SliderInt(U8("字体大小"), &GameData.Config.Project.ChareFontSize, 5, 20,  "%d\xC2\xB0");

							 }
							 ImGui::EndChild(true);
							 ImGui::BeginChild(true, U8("雷达预警"), "e", ImVec2(445, 195));
							 {
								 ImGui::Checkbox(U8("启动预警"), &GameData.Config.Early.Enable);

								 ImGui::Checkbox(U8("显示距离"), &GameData.Config.Early.ShowDistance);

								 ImGui::SliderInt(U8("最大距离"), &GameData.Config.Early.DistanceMax, 10, 1000, "%d\xC2\xB0");

								 ImGui::SliderInt(U8("图标比例"), &GameData.Config.Early.FontSize, 5, 20, "%d\xC2\xB0");
							 }
							 ImGui::EndChild(true);
						 }
						 ImGui::EndGroup();

						 ImGui::SameLine();

						 ImGui::BeginGroup();
						 {
							 
							 ImGui::BeginChild(true, U8("软件其他设置"), "e", ImVec2(445, 530));
							 {
								 ImGui::Checkbox(U8("指向模式"), &GameData.Config.Overlay.zhixiangmoshi);
								 
								 ImGui::Checkbox(U8("垂直同步"), &GameData.Config.Overlay.VSync);

								 ImGui::Checkbox(U8("备用相机"), &GameData.Config.Overlay.UseLastFrameCameraCache);

								 ImGui::Checkbox(U8("独立线程"), &GameData.Config.Overlay.UseThread);

								 ImGui::Checkbox(U8("过滤迷雾"), &GameData.Config.ESP.miwu);	

								 if (ImGui::Checkbox(U8("融合模式"), &GameData.Config.Overlay.FusionMode)) {
									 HWND Progman = FindWindowA("Progman", NULL);
									 if (GameData.Config.Overlay.FusionMode)
									 {
										 if (Progman)
										 {
											 ShowWindow(Progman, SW_HIDE);

										 }
									 }
									 else if (Progman)
									 {
										 ShowWindow(Progman, SW_SHOW);

									 }
								 }
								 
								 ImGui::Keybind(U8("安全结束"), &GameData.Config.Overlay.Quit_key);
								 
								 ImGui::Keybind(U8("显隐菜单"), &GameData.Config.Menu.ShowKey);
								 
								
								 ImGui::Keybind(U8("融合快捷按键"), &GameData.Config.Overlay.FusionModeKey);
								 
								 ImGui::Keybind(U8("战斗模式"), &GameData.Config.ESP.FocusModeKey);

								 ImGui::Keybind(U8("信息开关"), &GameData.Config.ESP.DataSwitchkey);

								 ImGui::Keybind(U8("显示队友"), &GameData.Config.ESP.duiyouKey);
								 
								 ImGui::Keybind(U8("刷新缓存"), &GameData.Config.Function.ClearKey);



								 const char* style[] = { U8("黑色"),U8("白色") };
								 ImGui::Combo(U8("菜单主题背景色"), &menu_style, style, IM_ARRAYSIZE(style));
							 }
							 ImGui::EndChild(true);
						 }
						 ImGui::EndGroup();
					 }
					 ImGui::EndChild();
					 }
			};
			ImGui::PopStyleVar();


		}
		ImGui::End();
		ImGui::RenderNotifications();


	
	};

};




		
