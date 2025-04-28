#pragma once
#include <winsock2.h>
#include <windows.h>
#include <imgui/imgui.h>
typedef bool(__stdcall* DRAW_PROC)(int Width, int Height, ImGuiIO& io);

bool DrawMain(int Width, int Height, ImGuiIO& io);
bool DebugDrawMain(int Width, int Height, ImGuiIO& io);
void ClickThrough(bool v);
namespace Overlay
{
	int Init(HWND TargetWnd, DRAW_PROC DrawProc, int Width = 0, int Height = 0);
	void DrawOverlayHandler();
	void Run();
	//int login();
};