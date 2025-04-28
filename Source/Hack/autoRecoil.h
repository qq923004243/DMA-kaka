#pragma once
#include <DMALibrary/Memory/Memory.h>
#include "common/Data.h"
#include "common/Entitys.h"
#include "utils/KmBox.h"
#include "utils/Lurker.h"
#include "utils/KmBoxNet.h"
#include "utils/MoBox.h"

class Recoil
{
public:


	static void Move(int X, int Y)
	{
		if (!GameData.Config.AimBot.Connected || !GameData.Config.AimBot.Enable)
		{
			return;
		}

		switch (GameData.Config.AimBot.Controller) {
		case 0:
			KmBox::Move(X, Y);
			break;
		case 1:
			KmBoxNet::Move(X, Y);
			break;
		case 2:
			Lurker::Move(X, Y);
			break;
		case 3:
			MoBox::Move(X, Y);
			break;
		default:
			return;
		}
	}

	static void autoRecoil()
	{
		auto Recolhs = mem.CreateScatterHandle();
		while (true)
		{
			Sleep(1);
			if (GameData.Radar.Visibility || GameData.Config.AimBot.aimboot || !GameData.Config.AimBot.Recoilenanlek)
				continue;

			if (GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType == WeaponType::AR)
			{
				SHORT BulletNumber = 1.f;
				bool IsScoping = false;
				bool IsReloading = false;
				FRotator Recoil;
				FRotator ControlRotation;
				mem.AddScatterRead(Recolhs, GameData.LocalPlayerInfo.AnimScriptInstance + GameData.Offset["bIsScoping_CP"], (bool*)&IsScoping);
				mem.AddScatterRead(Recolhs, GameData.LocalPlayerInfo.AnimScriptInstance + GameData.Offset["bIsReloading_CP"], (bool*)&IsReloading);
				mem.AddScatterRead(Recolhs, GameData.LocalPlayerInfo.AnimScriptInstance + GameData.Offset["RecoilADSRotation_CP"], (FRotator*)&Recoil);
				mem.AddScatterRead(Recolhs, GameData.LocalPlayerInfo.AnimScriptInstance + GameData.Offset["ControlRotation_CP"], (FRotator*)&ControlRotation);
				mem.AddScatterRead(Recolhs, GameData.LocalPlayerInfo.CurrentWeapon + GameData.Offset["CurrentAmmoData"], (SHORT*)&BulletNumber);
				mem.ExecuteReadScatter(Recolhs);
				if ((int)BulletNumber > 0 && IsScoping)
				{
					int yRecoil = 5;
					uint32_t temp = uint32_t(GameData.Camera.FOV);
					if (GameData.Keyboard.IsKeyDown(1))
					{
						if (temp <= 75 && temp >= 52)//紅點全息
						{
							Move(0, GameData.Config.AimBot.yRecoil[0]);
							Sleep(GameData.Config.AimBot.interval[0]);
							continue;
						}
						if (temp == 40)//二倍
						{
							Move(0, GameData.Config.AimBot.yRecoil[1]);
							Sleep(GameData.Config.AimBot.interval[1]);
							continue;
						}
						if (temp == 26)//三倍
						{
							Move(0, GameData.Config.AimBot.yRecoil[2]);
							Sleep(GameData.Config.AimBot.interval[2]);
							continue;
						}
						if (temp == 19)//四倍
						{
							Move(0, GameData.Config.AimBot.yRecoil[3]);
							Sleep(GameData.Config.AimBot.interval[3]);
							continue;
						}
						if (temp == 13)//六倍
						{
							Move(0, GameData.Config.AimBot.yRecoil[4]);
							Sleep(GameData.Config.AimBot.interval[4]);
							continue;
						}
						if (temp == 10)//八倍
						{
							Move(0, GameData.Config.AimBot.yRecoil[5]);
							Sleep(GameData.Config.AimBot.interval[5]);
							continue;
						}
					}
				}
			}
		}
		mem.CloseScatterHandle(Recolhs);
	}
};