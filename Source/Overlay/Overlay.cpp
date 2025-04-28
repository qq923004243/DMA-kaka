#include "Overlay.h"
#include "ESP.h"
#include "weapon.h"
#include "Texture.h"
#include <Common/Data.h>
#include <d3d11.h>
#include <D3DX11tex.h>
#include <imgui/imgui.h>

#include <imgui/imgui_internal.h>
#pragma comment(lib, "D3DX11.lib")
#include <Uxtheme.h>
#include <dwmapi.h>
#include <Utils/Throttler.h>
#include "Style.h"
#include "Map.h"
#include "RenderHelper.h"
#include "MThreadRenderer.h"

#include "Menu.h"
#include <tchar.h>
#include <Hack/LineTrace.h>

#include "imgui/font.h"

Wap deWap;

namespace image
{

	//子弹30109433
	ID3D11ShaderResourceView* Item_Ammo_12Guage = nullptr;
	ID3D11ShaderResourceView* Item_Ammo_300Magnum = nullptr;
	ID3D11ShaderResourceView* Item_Ammo_556mm = nullptr;
	ID3D11ShaderResourceView* Item_Ammo_9mm = nullptr;
	ID3D11ShaderResourceView* Item_Ammo_762mm = nullptr;
}
LONG nv_default = WS_POPUP | WS_CLIPSIBLINGS;
LONG nv_default_in_game = nv_default | WS_DISABLED;
LONG nv_edit = nv_default_in_game | WS_VISIBLE;

LONG nv_ex_default = WS_EX_TOOLWINDOW;
LONG nv_ex_edit = nv_ex_default | WS_EX_LAYERED | WS_EX_TRANSPARENT;
LONG nv_ex_edit_menu = nv_ex_default | WS_EX_TRANSPARENT;

// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };
// 在文件开头添加全局变量
static bool g_isDragging = false;
static POINT g_dragOffset = { 0, 0 };


// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void EnableMouseTransparency()
{
	HWND hWnd = (HWND)ImGui::GetCurrentWindow()->Viewport->PlatformHandleRaw;
	LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
	exStyle |= WS_EX_TRANSPARENT | WS_EX_LAYERED;
	SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);
}
void ShaderResource()
{
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Aimbot_, sizeof(Aimbot_), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Aimbot_id), nullptr);
	RtlZeroMemory(&Aimbot_, sizeof(Aimbot_));

	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, player, sizeof(player), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.player_id), nullptr);
	RtlZeroMemory(&player, sizeof(player));

	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, AK47, sizeof(AK47), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.AK47_id), nullptr);
	RtlZeroMemory(&AK47, sizeof(AK47));

	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, AUG, sizeof(AUG), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.AUG_id), nullptr);
	RtlZeroMemory(&AUG, sizeof(AUG));

	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, AWM, sizeof(AWM), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.AWM_id), nullptr);
	RtlZeroMemory(&AWM, sizeof(AWM));

	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Berreta686, sizeof(Berreta686), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Berreta686_id), nullptr);
	RtlZeroMemory(&Berreta686, sizeof(Berreta686));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, BerylM762, sizeof(BerylM762), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.BerylM762_id), nullptr);
	RtlZeroMemory(&BerylM762, sizeof(BerylM762));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, BizonPP19, sizeof(BizonPP19), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.BizonPP19_id), nullptr);
	RtlZeroMemory(&BizonPP19, sizeof(BizonPP19));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Crossbow, sizeof(Crossbow), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Crossbow_id), nullptr);
	RtlZeroMemory(&Crossbow, sizeof(Crossbow));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, DP12, sizeof(DP12), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.DP12_id), nullptr);
	RtlZeroMemory(&DP12, sizeof(DP12));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, DP28, sizeof(DP28), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.DP28_id), nullptr);
	RtlZeroMemory(&DP28, sizeof(DP28));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, FNFal, sizeof(FNFal), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.FNFal_id), nullptr);
	RtlZeroMemory(&FNFal, sizeof(FNFal));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, G36C, sizeof(G36C), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.G36C_id), nullptr);
	RtlZeroMemory(&G36C, sizeof(G36C));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Groza, sizeof(Groza), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Groza_id), nullptr);
	RtlZeroMemory(&Groza, sizeof(Groza));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, HK416, sizeof(HK416), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.HK416_id), nullptr);
	RtlZeroMemory(&HK416, sizeof(HK416));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, K2, sizeof(K2), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.K2_id), nullptr);
	RtlZeroMemory(&K2, sizeof(K2));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Kar98k, sizeof(Kar98k), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Kar98k_id), nullptr);
	RtlZeroMemory(&Kar98k, sizeof(Kar98k));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, L6, sizeof(L6), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.L6_id), nullptr);
	RtlZeroMemory(&L6, sizeof(L6));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, M16A4, sizeof(M16A4), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.M16A4_id), nullptr);
	RtlZeroMemory(&M16A4, sizeof(M16A4));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, M24, sizeof(M24), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.M24_id), nullptr);
	RtlZeroMemory(&M24, sizeof(M24));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, M249, sizeof(M249), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.M249_id), nullptr);
	RtlZeroMemory(&M249, sizeof(M249));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, MG3, sizeof(MG3), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.MG3_id), nullptr);
	RtlZeroMemory(&MG3, sizeof(MG3));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Mini14, sizeof(Mini14), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Mini14_id), nullptr);
	RtlZeroMemory(&Mini14, sizeof(Mini14));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, MK12, sizeof(MK12), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Mk12_id), nullptr);
	RtlZeroMemory(&MK12, sizeof(MK12));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, MK14, sizeof(MK14), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Mk14_id), nullptr);
	RtlZeroMemory(&MK14, sizeof(MK14));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Mk47Mutant, sizeof(Mk47Mutant), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Mk47Mutant_id), nullptr);
	RtlZeroMemory(&Mk47Mutant, sizeof(Mk47Mutant));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Mosin, sizeof(Mosin), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Mosin_id), nullptr);
	RtlZeroMemory(&Mosin, sizeof(Mosin));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, MP5K, sizeof(MP5K), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.MP5K_id), nullptr);
	RtlZeroMemory(&MP5K, sizeof(MP5K));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, P90, sizeof(P90), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.P90_id), nullptr);
	RtlZeroMemory(&P90, sizeof(P90));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, QBU88, sizeof(QBU88), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.QBU88_id), nullptr);
	RtlZeroMemory(&QBU88, sizeof(QBU88));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, QBZ95, sizeof(QBZ95), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.QBZ95_id), nullptr);
	RtlZeroMemory(&QBZ95, sizeof(QBZ95));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Saiga12, sizeof(Saiga12), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Saiga12_id), nullptr);
	RtlZeroMemory(&Saiga12, sizeof(Saiga12));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, SCAR_L, sizeof(SCAR_L), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.SCAR_L_id), nullptr);
	RtlZeroMemory(&SCAR_L, sizeof(SCAR_L));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, SKS, sizeof(SKS), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.SKS_id), nullptr);
	RtlZeroMemory(&SKS, sizeof(SKS));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Thompson, sizeof(Thompson), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Thompson_id), nullptr);
	RtlZeroMemory(&Thompson, sizeof(Thompson));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, UMP, sizeof(UMP), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.UMP_id), nullptr);
	RtlZeroMemory(&UMP, sizeof(UMP));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, UZI, sizeof(UZI), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.UZI_id), nullptr);
	RtlZeroMemory(&UZI, sizeof(UZI));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Vector, sizeof(Vector), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Vector_id), nullptr);
	RtlZeroMemory(&Vector, sizeof(Vector));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, VSS, sizeof(VSS), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.VSS_id), nullptr);
	RtlZeroMemory(&VSS, sizeof(VSS));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Win1894, sizeof(Win1894), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Win1894_id), nullptr);
	RtlZeroMemory(&Win1894, sizeof(Win1894));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Winchester, sizeof(Winchester), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Winchester_id), nullptr);
	RtlZeroMemory(&Winchester, sizeof(Winchester));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, DesertEagle, sizeof(DesertEagle), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.DesertEagle_id), nullptr);
	RtlZeroMemory(&DesertEagle, sizeof(DesertEagle));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, FlareGun, sizeof(FlareGun), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.FlareGun_id), nullptr);
	RtlZeroMemory(&FlareGun, sizeof(FlareGun));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, G18, sizeof(G18), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.G18_id), nullptr);
	RtlZeroMemory(&G18, sizeof(G18));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, M9, sizeof(M9), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.M9_id), nullptr);
	RtlZeroMemory(&M9, sizeof(M9));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, M1911, sizeof(M1911), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.M1911_id), nullptr);
	RtlZeroMemory(&M1911, sizeof(M1911));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, NagantM1895, sizeof(NagantM1895), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.NagantM1895_id), nullptr);
	RtlZeroMemory(&NagantM1895, sizeof(NagantM1895));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Rhino, sizeof(Rhino), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Rhino_id), nullptr);
	RtlZeroMemory(&Rhino, sizeof(Rhino));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, Sawnoff, sizeof(Sawnoff), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.Sawnoff_id), nullptr);
	RtlZeroMemory(&Sawnoff, sizeof(Sawnoff));
	D3DX11CreateShaderResourceViewFromMemory(
		g_pd3dDevice, vz61Skorpion, sizeof(vz61Skorpion), nullptr, nullptr,
		reinterpret_cast<ID3D11ShaderResourceView**>(&deWap.vz61Skorpion_id), nullptr);
	RtlZeroMemory(&vz61Skorpion, sizeof(vz61Skorpion));
}

#if 0
void Overlay::DrawOverlayHandler()
{
	if (GameData.Config.Overlay.Enable) {
		auto selectedMonitor = ImGui::GetPlatformIO().Monitors[GameData.Config.Overlay.MonitorCurrentIdx];
		ImGui::SetNextWindowSize(ImVec2(selectedMonitor.MainSize.x, selectedMonitor.MainSize.y));
		ImGui::SetNextWindowPos((ImVec2(selectedMonitor.MainPos.x, selectedMonitor.MainPos.y)));

		ImGuiWindowClass WindowClass;
		WindowClass.ClassId = ImHashStr("ESP Overlay");

		ImGuiViewportFlags WindowClassFlags = 0;
		WindowClassFlags |= ImGuiViewportFlags_NoDecoration;
		WindowClassFlags |= ImGuiViewportFlags_NoInputs;
		WindowClassFlags |= ImGuiViewportFlags_NoRendererClear;
		//WindowClassFlags |= ImGuiViewportFlags_IsPlatformWindow;
		WindowClassFlags |= ImGuiViewportFlags_NoAutoMerge;

		ImGuiWindowFlags WindowFlags = 0;
		WindowFlags |= ImGuiWindowFlags_NoTitleBar;
		WindowFlags |= ImGuiWindowFlags_NoResize;
		WindowFlags |= ImGuiWindowFlags_NoScrollbar;
		WindowFlags |= ImGuiWindowFlags_NoCollapse;
		WindowFlags |= ImGuiWindowFlags_NoInputs;
		WindowFlags |= ImGuiWindowFlags_NoDecoration;
		WindowFlags |= ImGuiWindowFlags_NoNav;

		if (!GameData.Config.Overlay.FusionMode)
		{
			WindowFlags |= ImGuiWindowFlags_NoBackground;
			WindowClassFlags |= ImGuiViewportFlags_TopMost;
		}

		ImGuiStyle& style = ImGui::GetStyle();
		float OriginalBorderSize = style.WindowBorderSize;
		ImVec4 OriginalWindowBgColor = style.Colors[ImGuiCol_WindowBg];
		ImVec2 OriginalWindowPadding = style.WindowPadding;

		style.WindowBorderSize = 0.0f;
		style.WindowPadding = { 0.f, 0.f };
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

		WindowClass.ViewportFlagsOverrideSet = WindowClassFlags;
		ImGui::SetNextWindowClass(&WindowClass);

		ImGui::Begin("ESP Overlay", nullptr, WindowFlags);
		{
			if (!GameData.Config.Overlay.FusionMode) EnableMouseTransparency();
			ImGuiIO& io = ImGui::GetIO(); (void)io;

			ESP::DrawFPS(io);
			ESP::DrawESP();
		}
		ImGui::End();

		style.WindowBorderSize = OriginalBorderSize;
		style.WindowPadding = OriginalWindowPadding;
		style.Colors[ImGuiCol_WindowBg] = OriginalWindowBgColor;
	}
}
#endif

void SetWindowMouseTransparent(bool transparent)
{

	LONG exStyle = GetWindowLong(GameData.Config.Overlay.hWnd, GWL_EXSTYLE);
	if (transparent)
	{
		exStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT; ;//隐藏窗口后鼠标穿透

	}
	else
	{
		exStyle &= ~WS_EX_TRANSPARENT;
	}
	SetWindowLong(GameData.Config.Overlay.hWnd, GWL_EXSTYLE, exStyle);
}

void ClickThrough(bool v)
{
	if (v) {
		nv_edit = nv_default_in_game | WS_VISIBLE;
		if (GetWindowLong(GameData.Config.Overlay.hWnd, GWL_EXSTYLE) != nv_ex_edit)
			SetWindowLong(GameData.Config.Overlay.hWnd, GWL_EXSTYLE, nv_ex_edit);
	}
	else {
		nv_edit = nv_default | WS_VISIBLE;
		if (GetWindowLong(GameData.Config.Overlay.hWnd, GWL_EXSTYLE) != nv_ex_edit_menu)
			SetWindowLong(GameData.Config.Overlay.hWnd, GWL_EXSTYLE, nv_ex_edit_menu);
	}
}
// Main code
void BeginDraw()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
void EndDraw(ImGuiIO& io)
{
	ImVec4 clear_color = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	ESP::DrawFPS(io);
	ImGui::Render();
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	g_pSwapChain->Present((GameData.Config.Overlay.FusionMode && !GameData.Config.Overlay.VSync) ? 0 : 1, 0);
}

void UpdateWindowTransparency()
{
	//static bool lastIsMenuRegion = false;
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(GameData.Config.Overlay.hWnd, &mousePos);

	//bool isMenuRegion = (mousePos.x >= menuRect.left && mousePos.x <= menuRect.right &&
	//	mousePos.y >= menuRect.top && mousePos.y <= menuRect.bottom);

	if (mousePos.x >= menuRect.left && mousePos.x <= menuRect.right &&
		mousePos.y >= menuRect.top && mousePos.y <= menuRect.bottom) {
		// 鼠标在菜单区域内，不穿透
		SetWindowLong(GameData.Config.Overlay.hWnd, GWL_EXSTYLE, nv_ex_edit_menu);
	}
	else {
		// 鼠标在菜单区域外，穿透
		if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && GameData.Config.Menu.Show)
			SetWindowLong(GameData.Config.Overlay.hWnd, GWL_EXSTYLE, nv_ex_edit_menu);
		else
			SetWindowLong(GameData.Config.Overlay.hWnd, GWL_EXSTYLE, nv_ex_edit);
	}

	// 重新设置窗口置顶
	//SetWindowPos(GameData.Config.Overlay.hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	//lastIsMenuRegion = isMenuRegion;
}

bool DrawMain(int Width, int Height, ImGuiIO& io) {

	UpdateWindowTransparency();

	D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };
	BeginDraw();
	MenuPlayerLists::Render11();
	ESP::DrawESP();
	
	if (GameData.Config.Menu.Show)
	{
		newMenu::newmenu(GameData.Config.Overlay.hWnd);
		
	}

	EndDraw(io);
	return true;
}

int Overlay::Init(HWND TargetWnd, DRAW_PROC DrawProc, int Width, int Height)
{
	ImGui_ImplWin32_EnableDpiAwareness();
	HWND DesktopWindow = GetDesktopWindow();

	if (!TargetWnd) {
		TargetWnd = DesktopWindow;
	}
	if (!Width || !Height)
	{
		RECT rect = { 0, 0, 0, 0 };
		::GetClientRect(DesktopWindow, &rect);
		Width = rect.right - rect.left;
		Height = rect.bottom - rect.top;
		std::cout << "Width: " << Width << "Height:  "<< Height << std::endl;

		GameData.Config.Overlay.ScreenWidth = (rect.right - rect.left);
		GameData.Config.Overlay.ScreenHeight = (rect.bottom - rect.top);
		GameData.Config.Overlay.ScreenX = GameData.Config.Overlay.ScreenWidth / 2;
		GameData.Config.Overlay.ScreenY = GameData.Config.Overlay.ScreenHeight / 2;
	}

	WNDCLASSEXW wc;
	memset(&wc, 0, sizeof(WNDCLASSEXW));
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = NULL;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = NULL;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)(RGB(0, 0, 0));
	wc.lpszClassName = L"CEF-OSC-WIDGET";
	wc.hIconSm = NULL;
	RegisterClassExW(&wc);
	HWND hwnd = CreateWindowEx((WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED),
		wc.lpszClassName, L"NVIDIA GeForce Overlay", WS_POPUP,
		0, 0, GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN), 0, 0, 0, 0);

	SetWindowLongA(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, 2);
	MARGINS margins = { -1 };
	::DwmExtendFrameIntoClientArea(hwnd, &margins);
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	if (!CreateDeviceD3D(hwnd)) {
		CleanupDeviceD3D();
		UnregisterClassW(wc.lpszClassName, wc.hInstance);
		MessageBox(nullptr, L"d3d fail!", L"", MB_OK);
		return 0;
	}

	Texture::LoadTextures(g_pd3dDevice, "Assets/image/RankImage");

	GameData.Config.Overlay.hWnd = hwnd;
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImFontConfig cfg;
	ImFontConfig cfg_regular;
	io.DisplaySize = ImVec2((float)Width, (float)Height);
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	cfg_regular.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;
	ImGui::StyleColorsDark();
	ShaderResource();

	//这里复制添加

	std::string IconUrl = "Assets/image/Map/car_land.png";

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Map/Carapackage_RedBox_C.png";   //地图空投

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Map/dead.png";  //地图死亡

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Map/indicator_onscreen_status_disconnect.png";  //地图掉线

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Map/indicator_onscreen_status_parachute.png";  //地图跳伞

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Map/indicator_onscreen_status_DBNO.png";  //地图倒地

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Map/indicator_onscreen_status_vehicle.png";  //地图开车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Map/Carapackage_SmallPackage_C.png";  //地图小空投

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Map/arrow.png";  //地图人物转向

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Armor_Repair_Kit_C.png";   //防具修理包

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Vehicle_Repair_Kit_C.png";   //载具修理包

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Helmet_Repair_Kit_C.png";   //头盔修理包

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/InstantRevivalKit_C.png";  //自救器

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Tiger_SelfRevive_C.png";  //自救器

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Rubberboat_C.png";  //皮划艇

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_9mm_C.png";    //9毫米子弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_12Guage_C.png";     //12口径子弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_12GuageSlug_C.png";    //O12子弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_40mm_C.png";  //40mm烟雾弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_45ACP_C.png";    //.45子弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_57mm_C.png";     //P90子弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_300Magnum_C.png";   //马格南子弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_303Ball_C.png";      //马格南子弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_556mm_C.png";   //556子弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_762mm_C.png";    //762子弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_Bolt_C.png";     //箭

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_Flare_C.png";   //信号枪子弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_Mortar_C.png";     //迫击炮弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Armor_C_01_Lv3_C.png";  //三级甲

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Armor_D_01_Lv2_C.png";   //二级甲

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Armor_E_01_Lv1_C.png";    //一级甲

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Lower_AngledForeGrip_C.png";   //三角握把

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Lower_Foregrip_C.png";    //垂直握把

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Lower_HalfGrip_C.png";     //红色握把

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Lower_LaserPointer_C.png";   //激光瞄准器

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Lower_LightweightForeGrip_C.png";  //轻型握把

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Lower_QuickDraw_Large_Crossbow_C.png";  //箭袋

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Lower_ThumbGrip_C.png";   //拇指握把

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Magazine_Extended_Large_C.png";   //步枪扩容

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Magazine_Extended_Medium_C.png";   //冲锋扩容

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Magazine_Extended_Small_C.png";    //手枪扩容

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Magazine_Extended_SniperRifle_C.png";  //狙击扩容

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Magazine_ExtendedQuickDraw_Large_C.png";  //步枪快扩

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Magazine_ExtendedQuickDraw_Medium_C.png";  //冲锋快扩

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Magazine_ExtendedQuickDraw_Small_C.png"; //手枪快扩

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Magazine_ExtendedQuickDraw_SniperRifle_C.png"; //狙击快扩

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Magazine_QuickDraw_Large_C.png";  //步枪快速

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Magazine_QuickDraw_Medium_C.png";   //冲锋快速

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Magazine_QuickDraw_Small_C.png";   //手枪快速

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Magazine_QuickDraw_SniperRifle_C.png";   //狙击快速

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Muzzle_Choke_C.png"; //扼流圈

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Muzzle_Compensator_Large_C.png";   //步枪补偿

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Muzzle_AR_MuzzleBrake_C.png";   //步枪制退器

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Muzzle_Compensator_Medium_C.png";   //冲锋枪补偿

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Muzzle_Compensator_SniperRifle_C.png";   //狙击补偿

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Muzzle_Duckbill_C.png";  //鸭嘴枪口

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Muzzle_FlashHider_Large_C.png";  //步枪消焰器

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Muzzle_FlashHider_Medium_C.png";   //冲锋枪消焰器 

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Muzzle_FlashHider_SniperRifle_C.png";//狙击枪消焰器 

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Muzzle_Suppressor_Large_C.png";   //步枪消音器

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Muzzle_Suppressor_Medium_C.png";//冲锋枪消音器

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Muzzle_Suppressor_Small_C.png";//手枪消音器

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Muzzle_Suppressor_SniperRifle_C.png";//狙击枪消音器

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_SideRail_DotSight_RMR_C.png";  //侧边瞄准器

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Stock_AR_Composite_C.png";  //战术枪托

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Stock_AR_HeavyStock_C.png";  //重型枪托

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Stock_SniperRifle_BulletLoops_C.png";   //子弹袋

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Stock_SniperRifle_CheekPad_C.png";    //托腮板

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Stock_UZI_C.png";   //折叠枪托

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Upper_DotSight_01_C.png";   //红点

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Upper_Holosight_C.png";    //全息

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Upper_Aimpoint_C.png";   //2倍镜

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Upper_Scope3x_C.png";   //3倍镜

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Upper_ACOG_01_C.png";    //4倍镜

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Upper_Scope6x_C.png";    //6倍镜

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Upper_CQBSS_C.png";    //8倍镜

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Upper_PM2_01_C.png";    //15倍镜

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Attach_Weapon_Upper_Thermal_C.png";    //热成像

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Back_B_08_Lv3_C.png";   //三级包

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Back_C_01_Lv3_C.png";   //三级包

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Back_C_02_Lv3_C.png";   //三级包

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Back_F_02_Lv2_C.png";    //二级包

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Back_F_01_Lv2_C.png";   //二级包

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Back_BlueBlocker.png";    //电磁包

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Back_E_02_Lv1_C.png";   //一级包

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Back_E_01_Lv1_C.png";   //一级包

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Bluechip_C.png";  //蓝色晶片

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Revival_Transmitter_C.png";  //蓝色晶片发射器

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Boost_AdrenalineSyringe_C.png";   //肾上腺素

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Boost_EnergyDrink_C.png";    //能量饮料  

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Boost_PainKiller_C.png";    //止疼药

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Heal_FirstAid_C.png";   //急救包

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Heal_MedKit_C.png";   //医疗箱

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Heal_Bandage_C.png";   //绷带

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_TacPack_C.png";   //战术套件

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Heal_BattleReadyKit_C.png";   //战斗准备套件

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_TraumaBag_C.png";    //医疗套件

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_IntegratedRepair_C.png";    //三合一套件

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ghillie_01_C.png";   //吉利服

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ghillie_02_C.png";      //吉利服

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ghillie_03_C.png";     //吉利服

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ghillie_04_C.png";      //吉利服

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ghillie_05_C.png";     //吉利服

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ghillie_06_C.png";     //吉利服

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Head_E_01_Lv1_C.png";  //一级头

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Head_E_02_Lv1_C.png";   //一级头

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Head_F_01_Lv2_C.png";  //二级头

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Head_F_02_Lv2_C.png";  //二级头

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Head_G_01_Lv3_C.png";  //三级头

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_BTSecretRoom_Key_C.png";  //海岛钥匙

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Chimera_Key_C.png";  //火山钥匙

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Heaven_Key_C.png";  //钥匙

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_DihorOtok_Key_C.png";  //门禁卡

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Secuity_KeyCard_C.png";  //蒂斯顿门禁卡

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Tiger_Key_C.png";  //泰戈钥匙

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Neon_Gold_C.png";  //金条

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Neon_Coin_C.png";  //交易币

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_BulletproofShield_C.png";   //折叠盾牌

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Mountainbike_C.png";   //折叠自行车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_EmergencyPickup_C.png";  //紧急呼救器

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_JerryCan_C.png";   //汽油桶

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_ACE32_C.png";   //ACE

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_AK47_C.png";   //AK

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_AUG_C.png";   //AUG

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_AWM_C.png";   //M24

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Berreta686_C.png";   //S686

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_BerylM762_C.png";   //M762

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_BizonPP19_C.png";   //野牛冲锋枪

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Crossbow_C.png";   //十字弩

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_BluezoneGrenade_C.png";   //篮圈手雷

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_C4_C.png";   //C4

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Cowbar_C.png";   //撬棍

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Pickaxe_C.png";   //镐

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Ziplinegun_C.png";   //绳索发射器

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Ammo_ZiplinegunHook_C.png";   //绳索

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_DecoyGrenade_C.png";   //诱饵弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_DesertEagle_C.png";    //沙漠之鹰

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_DP12_C.png";   //DBS

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_DP28_C.png";   //DP28

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Dragunov_C.png";   //德拉贡诺夫

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Drone_C.png";   //无人机

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Duncans_M416_C.png";   //M416

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_FAMASG2_C.png";   //FAMASI

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_FlareGun_C.png";    //信号枪

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_FlashBang_C.png";    //闪光弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_FNFal_C.png";   //SLR

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_G18_C.png";   //P18C

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_G36C_C.png";   //G36C

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Grenade_C.png";   //手雷

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Groza_C.png";   //狗杂

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_HK416_C.png";   //M416

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_JS9_C.png";   //JS9

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/WeapJS9_C.png";   //JS9

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Julies_Kar98k_C.png";   //98K

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_M9_C.png";   //P92

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_L6_C.png";    //巴雷特

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Kar98k_C.png";   //98K

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_K2_C.png";   //K2

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_M16A4_C.png";   //M16A4

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_M24_C.png";   //M24·

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_M79_C.png";   //M79

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_M249_C.png";    //M249

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_M1911_C.png";  //P1911

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Machete_C.png";   //砍刀

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Mads_QBU88_C.png";   //QBU

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_MG3_C.png";   //MG3

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Mini14_C.png";    //Mini14

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Mk12_C.png";   //Mk12

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Mk14_C.png";   //Mk14

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Mk47Mutant_C.png";   //Mk47

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Molotov_C.png";     //燃烧瓶

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Mortar_C.png";   //迫击炮

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Mosin_C.png";   //莫辛甘纳

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_MP5K_C.png";    //MP5K

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_MP9_C.png";    //MP9

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_NagantM1895_C.png";   //R1895

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_OriginS12_C.png";   //O12

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_P90_C.png";    //P90

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Pan_C.png";    //平底锅

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_PanzerFaust100M_C.png";    //火箭筒

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_QBU88_C.png";    //QBU

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_QBZ95_C.png";   //QBZ95

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Rhino_C.png";   //R45

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Saiga12_C.png";   //S12K

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Sawnoff_C.png";   //锯齿短喷

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_SCAR-L_C.png";     //SCAR - L

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Sickle_C.png";   //镰刀

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/WeapSickle_C.png";   //镰刀

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/WeapSickleProjectile_C.png";    //镰刀

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_SKS_C.png";   //SKS

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_SmokeBomb_C.png";  //烟雾弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/WeapSmokeBomb_C.png";  //烟雾弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_SpikeTrap_C.png";    //尖刺陷阱

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Spotter_Scope_C.png";   //望远镜

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_StickyGrenade_C.png";  //粘性炸弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_StunGun_C.png";   //电击枪

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Thompson_C.png";   //汤姆逊

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_UMP_C.png";   //UMP

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_UZI_C.png";   //UZI

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Vector_C.png";   //Vector

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_VSS_C.png";   //VSS

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_vz61Skorpion_C.png";   //蝎式手枪

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Win1894_C.png";   //Win94

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/All/Item_Weapon_Winchester_C.png";  //S1897

	//手持武器贴图

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/ACE.png";   //ACE

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/AKM.png";  //AKM

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/AUG.png";  //AUG

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/AWM.png";  //AWM

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/BerylM762.png";  //BerylM762

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/C4.png";  //C4

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/DBS.png";  //DBS

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/DP28.png";  //DP28

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/FAMASI.png";  //FAMASI

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/G36C.png";  //G36C

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/Groza.png";  //Groza

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/JS9.png";  //JS9

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/K2.png";  //K2

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/Kar98k.png";  //Kar98k

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/Lynx.png";  //Lynx

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/M16A4.png";  //M16A4

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/M24.png";  //M24

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/M249.png";  //M249

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/M416.png";  //M416

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/MG3.png";  //MG3

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/Mini14.png";  //Mini14

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/Mk12.png";  //Mk12

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/Mk14.png";  //Mk14

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/Mk47.png";  //Mk47

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/MP5K.png";  //MP5K

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/MP9.png";  //MP9

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/O12.png";  //O12

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/P18C.png";  //P18C

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/P90.png";  //P90

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/P92.png";  //P92

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/P1911.png";  //P1911

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/QBU.png";  //QBU

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/QBZ95.png";  //QBZ95

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/R45.png";  //R45

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/R1895.png";  //R1895

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/S12K.png";  //S12K

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/S686.png";  //S686

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/S1897.png";  //S1897

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/SCAR-L.png";  //SCAR-L

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/SKS.png";  //SKS

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/SLR.png";  //SLR

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/UMP.png";  //UMP

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/UZI.png";  //UZI

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/Vector.png";  //Vector

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/VSS.png";  //VSS

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/Win94.png";  //Win94

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/德拉贡诺夫.png";  //德拉贡诺夫

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/电击枪.png";  //电击枪

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/火箭筒.png";  //火箭筒

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/尖刺陷阱.png";  //尖刺陷阱

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/锯齿短喷.png";  //锯齿短喷

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/砍刀.png";  //砍刀

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/篮圈手雷.png";  //篮圈手雷

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/镰刀.png";  //镰刀

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/莫辛甘纳.png";  //莫辛甘纳

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/弩.png";  //弩

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/平底锅.png";  //平底锅

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/撬棍.png";  //撬棍

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/燃烧瓶.png";  //燃烧瓶

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/沙漠之鹰.png";  //沙漠之鹰

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/闪光弹.png";  //闪光弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/手雷.png";  //手雷

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/汤姆逊.png";  //汤姆逊

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/蝎式手枪.png";  //蝎式手枪

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/信号枪.png";  //信号枪

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/烟雾弹.png";  //烟雾弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/野牛PP19.png";  //野牛PP19

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/诱饵手雷.png";  //诱饵手雷

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Weapon/粘性炸弹.png";  //粘性炸弹

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);


	//段位相关

	IconUrl = "Assets/image/RankImage/Bronze-1.png";  //青铜1

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Bronze-2.png"; // 青铜2

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Bronze-3.png";  // 青铜3

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Bronze-4.png";  // 青铜4

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Bronze-5.png";  // 青铜5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Unranked.png";  //未定级

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Master.png";  //大师

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Gold-1.png";  //黄金1

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Gold-2.png";  //黄金2

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Gold-3.png";  //黄金3

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Gold-4.png";  //黄金4

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Gold-5.png";  //黄金5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Silver-1.png";  //白银1

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Silver-2.png";  //白银2

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Silver-3.png";  //白银3

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Silver-4.png";  //白银4

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Silver-5.png";  //白银5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Platinum-1.png";  //铂金1

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Platinum-2.png";  //铂金2

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Platinum-3.png";  //铂金3

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Platinum-4.png";  //铂金4

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Platinum-5.png";  //铂金5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Diamond-1.png";  //钻石1

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Diamond-2.png";  //钻石2

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Diamond-3.png";  //钻石3

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Diamond-4.png";  //钻石4

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/RankImage/Diamond-5.png";  //钻石5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	//载具图片
	IconUrl = "Assets/image/Vehicle/AquaRail_A_00_C.png";  //钻石5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);
	
	IconUrl = "Assets/image/Vehicle/Boat_PG117_C.png";  //钻石5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);
	
	IconUrl = "Assets/image/Vehicle/BP_BRDM_C.png";  //装甲车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_DO_Circle_Train_Merged_C.png";  //火车5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_DO_Line_Train_Dino_Merged_C.png";  //钻石5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_DO_Line_Train_Merged_C.png";  //钻石5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);


	IconUrl = "Assets/image/Vehicle/BP_LootTruck_C.png";  //物资车5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_M_Rony_A_00_C.png";  //钻石5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_Mirado_A_00_C.png";  //钻石5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_Mirado_Open_00_C.png";  //钻石5

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_Motorbike_00_C.png";  //摩托车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_Motorbike_00_SideCar_C.png";  //3轮摩托车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_Motorglider_C.png";  //滑翔机

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_Niva_00_C.png";  //雪地车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_PickupTruck_A_00_C.png";  //摩托车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_PickupTruck_B_00_C.png";  //摩托车========

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_Scooter_00_A_C.png";  //摩托车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_Snowbike_00_C.png";  //摩托车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);
	
	IconUrl = "Assets/image/Vehicle/BP_Snowmobile_00_C.png";  //摩托车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_TukTukTuk_A_00_C.png";  //3轮车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/BP_Van_A_00_C.png";  //公交车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/Buggy_A_01_C.png";  //笨笨

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/Dacia_A_00_v2_C.png";  //摩托车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/DummyTransportAircraft_C.png";  //飞机

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/Uaz_A_00_C.png";  //摩托车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/Uaz_B_00_C.png";  //摩托车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	IconUrl = "Assets/image/Vehicle/Uaz_C_00_C.png";  //摩托车

	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	//logo
	IconUrl = "Assets/image/Map/PaoX.png";  //logo
	GImGuiTextureMap[IconUrl] = Texture::LoadTexture(g_pd3dDevice, IconUrl);

	// Setup Platform/Renderer backends



	cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;
	cfg.FontDataOwnedByAtlas = true;

	font::calibri_bold = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyhb.ttc", 18.f, &cfg, io.Fonts->GetGlyphRangesChineseFull());
	font::calibri_bold_hint = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyhbd.ttc", 18.f, &cfg, io.Fonts->GetGlyphRangesChineseFull());
	font::myth_bold = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyhbd.ttc", 18.f, &cfg, io.Fonts->GetGlyphRangesChineseFull());
	font::myth_bold_b = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyhbd.ttc", 25.f, &cfg, io.Fonts->GetGlyphRangesChineseFull());
	font::calibri_regular = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyh.ttc", 25.f, &cfg_regular, io.Fonts->GetGlyphRangesChineseFull());
	

	{ // font


		font::icomoon_default = io.Fonts->AddFontFromMemoryTTF(icomoon_sizeof, sizeof(icomoon_sizeof), 35.f, &cfg, io.Fonts->GetGlyphRangesChineseFull());
		font::icomoon_menu = io.Fonts->AddFontFromMemoryTTF(icomoon_sizeof, sizeof(icomoon_sizeof), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::pixel_7_small = io.Fonts->AddFontFromMemoryTTF(pixel_7_small, sizeof(pixel_7_small), 10.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::icomoon = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 20, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::weapon_val = io.Fonts->AddFontFromMemoryTTF(weapon_icon, sizeof(weapon_icon), 55, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		io.Fonts->AddFontFromMemoryTTF(icomoon_sizeof, sizeof icomoon_sizeof, 17);
	}
	
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	ClickThrough(false);

	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };

	if (texture::logo == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, logo, sizeof(logo), &info, pump, &texture::logo, 0);
	if (texture::playermoder == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, playermoder_size, sizeof(playermoder_size), &info, pump, &texture::playermoder, 0);
	if (texture::weapon_image == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, weapon_image, sizeof(weapon_image), &info, pump, &texture::weapon_image, 0);
	if (texture::rank == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, ranking, sizeof(ranking), &info, pump, &texture::rank, 0);


	Throttler Throttlered;
	CameraData Camera;
	float TimeSeconds = 0.f;
	// Main loop
	bool is = true;
	while (is)
	{
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				is = false;
		}
		if (GameData.Config.Overlay.hWnd == hwnd)
		{
			SetWindowPos(hwnd, (HWND)-1, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		if (!is)
			break;



		if (TargetWnd != DesktopWindow)
		{
			RECT rect = { 0, 0, 0, 0 };
			::GetClientRect(TargetWnd, &rect);
			is = DrawProc((int)(rect.right - rect.left), (int)(rect.bottom - rect.top), io);
		}
		else
		{
			is = DrawProc((int)io.DisplaySize.x, (int)io.DisplaySize.y, io);
		}
		if (GameData.Config.ESP.PhysXDebug && GameData.Scene == Scene::Gaming) {
			GameData.NextHintMeshData = LineTrace::getNextHint();
		}
		if (GameData.Config.Window.IsLogin) {
			if (!GameData.Config.Overlay.UseThread) {
				auto hScatter = mem.CreateScatterHandle();
				if (!GameData.Config.Overlay.UseLastFrameCameraCache)
				{
					mem.AddScatterRead(hScatter, GameData.PlayerCameraManager + GameData.Offset["CameraCacheLocation"], &Camera.Location);
					mem.AddScatterRead(hScatter, GameData.PlayerCameraManager + GameData.Offset["CameraCacheRotation"], &Camera.Rotation);
					mem.AddScatterRead(hScatter, GameData.PlayerCameraManager + GameData.Offset["CameraCacheFOV"], &Camera.FOV);
				}
				mem.AddScatterRead(hScatter, GameData.UWorld + GameData.Offset["TimeSeconds"], &TimeSeconds);
				mem.ExecuteReadScatter(hScatter);

				GameData.Camera = Camera;
				GameData.WorldTimeSeconds = TimeSeconds;

				mem.CloseScatterHandle(hScatter);
			}
		}
		
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	GameData.Config.Window.Main = false;

	return 0;
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & 16384)
		{
			//const int dpi = HIWORD(wParam);
			//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
			const RECT* suggested_rect = (RECT*)lParam;
			::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}