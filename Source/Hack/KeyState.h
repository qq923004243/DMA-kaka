#pragma once
#include <Winsock2.h>
#include <DMALibrary/Memory/Memory.h>
#include <Common/Data.h>
#include <Utils/KmBox.h>
#include <Utils/KmBoxNet.h>
#include <Hack/Players.h>

namespace KeyState
{
	void Init()
	{
		GameData.Keyboard = mem.GetKeyboard();

		if (!GameData.Keyboard.InitKeyboard())
		{
			MessageBoxA(NULL, "热键初始化失败，请重启主机", "提示", MB_ICONERROR | MB_OK | MB_TOPMOST);
			//Utils::Log(2, "Failed to initialize keyboard hotkeys through kernel.");
			GameData.KeyState = false;
		}
		else {
			//Utils::Log(1, U8("初始化键盘热键成功（免责声明：该游戏仅供学习使用与本人无关，请及时在24小时内删除谢谢）"));
			GameData.KeyState = true;
			Utils::Log(1, U8("初始化键盘热键成功（免责声明：该游戏仅供学习使用与本人无关，请及时在24小时内删除谢谢）:%llx", GameData.Keyboard.GetAddrss()));
		}
	}
	void Update() {
		//Utils::Log(1, "Key Thread Running...");
		while (true)
		{
			GameData.Keyboard.UpdateKeys();

			std::unordered_map<int, std::vector<std::string>> Keys;
			Keys[GameData.Config.Menu.ShowKey].push_back("Menu");
			Keys[GameData.Config.Overlay.Quit_key].push_back("DEAD");
			Keys[GameData.Config.AimBot.Configs[0].Key].push_back("AimBotConfig0");
			Keys[GameData.Config.AimBot.Configs[1].Key].push_back("AimBotConfig1");
			Keys[VK_DELETE].push_back("RecoverOverlay");
			Keys[GameData.Config.Function.ClearKey].push_back("Clear");
			//Keys[GameData.Config.Item.GroupKey].push_back("GroupKey");
			Keys[GameData.Config.Item.GroupAKey].push_back("GroupAKey");
			Keys[GameData.Config.Item.GroupBKey].push_back("GroupBKey");
			Keys[GameData.Config.Item.GroupCKey].push_back("GroupCKey");
			Keys[GameData.Config.Item.GroupDKey].push_back("GroupDKey");
			Keys[GameData.Config.Vehicle.EnableKey].push_back("VehicleEnable");
			Keys[GameData.Config.PlayerList.MarkKey].push_back("PlayerListMarkType");
			Keys[GameData.Config.Overlay.rankList].push_back("rankList");
			Keys[GameData.Config.ESP.FocusModeKey].push_back("FocusModeKey");
			Keys[GameData.Config.AirDrop.EnableKey].push_back("AirDropEnableKey");
			Keys[GameData.Config.DeadBox.EnableKey].push_back("DeadBoxEnableKey");
			Keys[GameData.Config.Overlay.FusionModeKey].push_back("FusionModeKey");;
			Keys[GameData.Config.ESP.duiyouKey].push_back("duiyouKey");
			Keys[GameData.Config.ESP.fast_aimbot_switch].push_back("fast_aimbot_switch");
			Keys[GameData.Config.ESP.DataSwitchkey].push_back("DataSwitchkey");

			/*if (GameData.Keyboard.WasKeyPressed(VK_F2))
				GameData.Config.AimBot.Enable = !GameData.Config.AimBot.Enable;*/

			for (auto Key : Keys)
			{
				if (GameData.Keyboard.WasKeyPressed(Key.first))
				{
					for (auto KeyName : Key.second)
					{
						if (KeyName == "AirDropEnableKey")
						{
							GameData.Config.AirDrop.Enable = !GameData.Config.AirDrop.Enable;
						}

				

						if (KeyName == "duiyouKey")
						{
							GameData.Config.ESP.duiyou = !GameData.Config.ESP.duiyou;
						}

						if (KeyName == "fast_aimbot_switch") {
							GameData.Config.AimBot.Enable = !GameData.Config.AimBot.Enable;
						}

						if (KeyName == "DataSwitchkey")
						{
							GameData.Config.ESP.DataSwitch = !GameData.Config.ESP.DataSwitch;
						}

						if (KeyName == "DeadBoxEnableKey")
						{
							GameData.Config.DeadBox.Enable = !GameData.Config.DeadBox.Enable;
						}

						if (KeyName == "FusionModeKey")
						{
							GameData.Config.Overlay.FusionMode = !GameData.Config.Overlay.FusionMode;
							HWND Progman = FindWindowA("Progman", NULL);
							if (GameData.Config.Overlay.FusionMode)
							{
								if (Progman)
								{
									ShowWindow(Progman, SW_HIDE);
									//ShowWindow(TrayWnd, SW_HIDE);
								}
							}
							else if (Progman)
							{
								ShowWindow(Progman, SW_SHOW);
								//ShowWindow(TrayWnd, SW_SHOW);
							}
						}

						if (KeyName == "FocusModeKey")
						{
							GameData.Config.ESP.FocusMode = !GameData.Config.ESP.FocusMode;
						}
						if (KeyName == "rankList")
						{
							GameData.Config.Window.Players = !GameData.Config.Window.Players;
						}
						
						if (KeyName == "DEAD")//  这里是结束
						{

							HWND Progman = FindWindowA("Progman", NULL);
							//HWND TrayWnd = FindWindowA("Shell_TrayWnd", NULL);
							ShowWindow(Progman, SW_SHOW);
							//ShowWindow(TrayWnd, SW_SHOW);
							
							exit(0);
							//ExitProcess(0);
							
							
						}
						if (KeyName == "Clear")
						{
							if (GameData.Config.AimBot.Connected)
							{
								if (GameData.Config.AimBot.Controller == 0)
								{
									KmBox::Clear();
									Utils::Log(1, "KMBOX Clear Success");
								}

								if (GameData.Config.AimBot.Controller == 1)
								{
									KmBoxNet::Clear();
									Utils::Log(1, "KMBOXNET Clear Success");
								}
							}

							Data::SetCacheEntitys({});
							Data::SetCachePlayers({});
							Data::SetPlayers({});
							Data::SetPlayersData({});
							Data::SetCacheVehicles({});
							Data::SetVehicles({});
							Data::SetVehiclWheels({});
							Data::SetItems({});
							Data::SetCacheDroppedItems({});
							Data::SetCacheDroppedItemGroups({});
							GameData.AimBot.Target = 0;
							GameData.AimBot.Lock = false;
							mem.RefreshAll();
						}
						

						/*if (KeyName == "GroupKey")
						{
							if (GameData.Config.Item.ShowGroup != 5)
							{
								GameData.Config.Item.Enable = 1;
								GameData.Config.Item.ShowGroup++;
							}
							else {
								GameData.Config.Item.ShowGroup = 0;
								GameData.Config.Item.Enable = 0;
							}
						}*/

						if (KeyName == "GroupAKey")
						{
							GameData.Config.Item.Enable = 1;
							//GameData.Config.Item.ShowGroup = 2;
							if (GameData.Config.Item.ShowGroups.count(1))
								GameData.Config.Item.ShowGroups.erase(1);
							else
								GameData.Config.Item.ShowGroups.insert(1);
						}

						if (KeyName == "GroupBKey")
						{
							GameData.Config.Item.Enable = 1;
							//GameData.Config.Item.ShowGroup = 3;
							if (GameData.Config.Item.ShowGroups.count(2))
								GameData.Config.Item.ShowGroups.erase(2);
							else
								GameData.Config.Item.ShowGroups.insert(2);
						}

						if (KeyName == "GroupCKey")
						{
							GameData.Config.Item.Enable = 1;
							//GameData.Config.Item.ShowGroup = 4;
							if (GameData.Config.Item.ShowGroups.count(3))
								GameData.Config.Item.ShowGroups.erase(3);
							else
								GameData.Config.Item.ShowGroups.insert(3);
						}

						if (KeyName == "GroupDKey")
						{
							GameData.Config.Item.Enable = 1;
							//GameData.Config.Item.ShowGroup = 5;
							if (GameData.Config.Item.ShowGroups.count(4))
								GameData.Config.Item.ShowGroups.erase(4);
							else
								GameData.Config.Item.ShowGroups.insert(4);
						}

						if (KeyName == "VehicleEnable")
						{
							GameData.Config.Vehicle.Enable = !GameData.Config.Vehicle.Enable;
						}
						if (KeyName == "Menu")
						{
							//Utils::Log(1, "press END key!");
							GameData.Config.Menu.Show = !GameData.Config.Menu.Show;

							if (GameData.Config.Menu.Show)
							{
								SetForegroundWindow(GameData.Config.Overlay.hWnd);
							}
							else {
								SetForegroundWindow(GetDesktopWindow());
							}
						}
						if (KeyName == "AimBotConfig0")
						{
							GameData.Config.AimBot.ConfigIndex = 0;
						}
						if (KeyName == "AimBotConfig1")
						{
							GameData.Config.AimBot.ConfigIndex = 1;
						}
					}
				}
			}

			Sleep(3);
		}
	}
}
