#pragma once
#include <cstdint>
#include "common/Data.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>

#include <wininet.h>
#include <sstream>

#pragma comment(lib, "wininet.lib")

namespace Offset
{

	inline uint64_t extractOffset(const std::string& offset, const std::string& key) {
		size_t startPos = offset.find(key);
		while (startPos != std::string::npos) {
			size_t endPos = startPos + key.length();
			if (endPos >= offset.length() || std::isspace(offset[endPos])) {
				//std::cout << "找到键: " << key << " 起始位置: " << startPos << std::endl;
				startPos = endPos;
				// 查找 '=' 号的位置
				size_t eqPos = offset.find('=', startPos);
				if (eqPos != std::string::npos) {
					//std::cout << "找到 '=' 号 起始位置: " << eqPos << std::endl;
					startPos = eqPos + 1; // 移动到 '=' 之后
					size_t lineEndPos = offset.find("\n", startPos);
					if (lineEndPos == std::string::npos) {
						lineEndPos = offset.length();
					}
					std::string line = offset.substr(startPos, lineEndPos - startPos);
					//std::cout << "提取的行: " << line << std::endl;
					std::stringstream ss(line);
					uint64_t value;
					ss >> std::ws; // 跳过前导空白字符
					ss >> std::hex >> value;
					if (ss.fail()) {
						std::cerr << "Change 0x16 Failed !" << std::endl;
						return 0;
					}
					return value;
				}
				else {
					std::cerr << "Not Found '=' " << std::endl;
				}
			}
			// 继续搜索下一个匹配的键
			startPos = offset.find(key, startPos + 1);
		}
		std::cerr << "Not Found Key: " << key << std::endl;
		return 30109433;
	}

	static std::string FindOffsets(std::string name, std::string data)
	{
		name += ',';
		for (uintptr_t i = 0; i < data.size(); i++)
		{
			size_t Position = data.find(name);
			if (Position != data.npos)
			{
				Position += strlen(name.c_str());
				Position += 1;
				size_t nSize = 0;
				for (nSize; nSize < 100; nSize++)
				{
					if (data[Position + nSize] == ',')
					{
						break;
					}
				}
				std::string address;
				address.resize(nSize, 0);
				memcpy((void*)address.c_str(), (const void*)&data[Position], nSize);
				return address;
			}
		}
		return "0";
	}
#define BUF_SIZE 8192*2
	static LPSTR GetInterNetURLText(LPCSTR  lpcInterNetURL, char* buff)
	{
		HINTERNET hSession;
		LPSTR lpResult = NULL;
		hSession = InternetOpen(L"WinInet", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		__try
		{
			if (hSession != NULL)
			{
				HINTERNET hRequest;
				hRequest = InternetOpenUrlA(hSession, lpcInterNetURL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
				__try
				{
					if (hRequest != NULL)
					{
						DWORD dwBytesRead;
						char szBuffer[BUF_SIZE] = { 0 };

						if (InternetReadFile(hRequest, szBuffer, BUF_SIZE, &dwBytesRead))
						{
							RtlMoveMemory(buff, szBuffer, BUF_SIZE);
							return 0;
						}
					}
				}
				__finally
				{
					InternetCloseHandle(hRequest);
				}
			}
		}
		__finally
		{
			InternetCloseHandle(hSession);
		}
		return lpResult;
	}

	

	static bool ReadLocalFile(const char* filename, char* buff, size_t bufSize) {
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (!file.is_open()) {
			return false;
		}

		std::streamsize size = file.tellg();
		if (size < 0 || size >= bufSize) {
			return false;
		}

		file.seekg(0, std::ios::beg);
		if (!file.read(buff, size)) {
			return false;
		}

		buff[size] = '\0';  // 确保字符串以null结尾
		return true;
	}

	inline void Sever_Init() {
		//这里是post获取
		std::string url = "http://localhost/offset.txt";

		char buf[BUF_SIZE] = { 0 };
		GetInterNetURLText(url.c_str(), buf);
		std::string buffer = std::string(buf);

		//这里是读取文本
		//std::string file_path = "offset.txt";  // 修改为本地文件名
		//std::string buffer = "";
		//char buf[BUF_SIZE] = { 0 };
		//if (ReadLocalFile(file_path.c_str(), buf, BUF_SIZE)) {
		//	buffer = std::string(buf);

		//	Utils::Log(0, U8("read success"));
		//}
		//else {
		//	buffer = "1";
		//}
		//

		GameData.Offset["Health"] = extractOffset(buffer, "Health4");
		GameData.Offset["bEncryptedHealth"] = GameData.Offset["Health"] + 0x15;
		GameData.Offset["EncryptedHealthOffset"] = GameData.Offset["Health"] + 0x14;
		GameData.Offset["DecryptedHealthOffset"] = GameData.Offset["Health"] + 0x10;

		//GameData.Offset["PhysxSDK"] = extractOffset(buffer, "Physx_SDK");
		//GameData.Offset["PhysxSDK"] = 0x10518988;

		GameData.Offset["VehicleFuel"] = extractOffset(buffer, "VehicleFuel");
		GameData.Offset["VehicleCommonComponent"] = extractOffset(buffer, "VehicleCommonComponent");
		GameData.Offset["FloatingComponent"] = extractOffset(buffer, "FloatingComponent");
		GameData.Offset["VehicleFuelMax"] = extractOffset(buffer, "VehicleFuelMax");
		GameData.Offset["VehicleHealth"] = extractOffset(buffer, "VehicleHealth");
		GameData.Offset["VehicleHealthMax"] = extractOffset(buffer, "VehicleHealthMax");

		GameData.Offset["InventoryFacade"] = extractOffset(buffer, "InventoryFacade");
		GameData.Offset["Inventory"] = extractOffset(buffer, "Inventory");
		GameData.Offset["InventoryItems"] = extractOffset(buffer, "InventoryItems");
		GameData.Offset["InventoryItemCount"] = extractOffset(buffer, "InventoryItemCount");
		GameData.Offset["InventoryItemTagItemCount"] = extractOffset(buffer, "InventoryItemTagItemCount");

		GameData.Offset["HeaFlag"] = extractOffset(buffer, "HeaFlag");
		GameData.Offset["Health1"] = extractOffset(buffer, "Health1");
		GameData.Offset["Health2"] = extractOffset(buffer, "Health2");
		GameData.Offset["Health3"] = extractOffset(buffer, "Health3");
		GameData.Offset["Health4"] = extractOffset(buffer, "Health4");
		GameData.Offset["Health5"] = extractOffset(buffer, "Health5");
		GameData.Offset["Health6"] = extractOffset(buffer, "Health6");
		GameData.Offset["DecryptedHealthOffsets0"] = extractOffset(buffer, "Health_keys0");
		GameData.Offset["DecryptedHealthOffsets1"] = extractOffset(buffer, "Health_keys1");
		GameData.Offset["DecryptedHealthOffsets2"] = extractOffset(buffer, "Health_keys2");
		GameData.Offset["DecryptedHealthOffsets3"] = extractOffset(buffer, "Health_keys3");
		GameData.Offset["DecryptedHealthOffsets4"] = extractOffset(buffer, "Health_keys4");
		GameData.Offset["DecryptedHealthOffsets5"] = extractOffset(buffer, "Health_keys5");
		GameData.Offset["DecryptedHealthOffsets6"] = extractOffset(buffer, "Health_keys6");
		GameData.Offset["DecryptedHealthOffsets7"] = extractOffset(buffer, "Health_keys7");
		GameData.Offset["DecryptedHealthOffsets8"] = extractOffset(buffer, "Health_keys8");
		GameData.Offset["DecryptedHealthOffsets9"] = extractOffset(buffer, "Health_keys9");
		GameData.Offset["DecryptedHealthOffsets10"] = extractOffset(buffer, "Health_keys10");
		GameData.Offset["DecryptedHealthOffsets11"] = extractOffset(buffer, "Health_keys11");
		GameData.Offset["DecryptedHealthOffsets12"] = extractOffset(buffer, "Health_keys12");
		GameData.Offset["DecryptedHealthOffsets13"] = extractOffset(buffer, "Health_keys13");
		GameData.Offset["DecryptedHealthOffsets14"] = extractOffset(buffer, "Health_keys14");
		GameData.Offset["DecryptedHealthOffsets15"] = extractOffset(buffer, "Health_keys15");
		GameData.Offset["MouseX"] = extractOffset(buffer, "MouseX");
		GameData.Offset["MouseY"] = extractOffset(buffer, "MouseY");
		GameData.Offset["bAlwaysCreatePhysicsState"] = extractOffset(buffer, "bAlwaysCreatePhysicsState");
		GameData.Offset["XenuineDecrypt"] = extractOffset(buffer, "XenuineDecrypt");
		GameData.Offset["UWorld"] = extractOffset(buffer, "UWorld");
		GameData.Offset["GNames"] = extractOffset(buffer, "GNames");
		GameData.Offset["GNamesPtr"] = extractOffset(buffer, "GNamesPtr");
		GameData.Offset["ChunkSize"] = extractOffset(buffer, "ChunkSize");
		GameData.Offset["DecryptNameIndexRor"] = extractOffset(buffer, "DecryptNameIndexRor");
		GameData.Offset["DecryptNameIndexXorKey1"] = extractOffset(buffer, "DecryptNameIndexXorKey1");
		GameData.Offset["DecryptNameIndexXorKey2"] = extractOffset(buffer, "DecryptNameIndexXorKey2");
		GameData.Offset["DecryptNameIndexRval"] = extractOffset(buffer, "DecryptNameIndexRval");
		GameData.Offset["DecryptNameIndexSval"] = extractOffset(buffer, "DecryptNameIndexSval");
		GameData.Offset["ObjID"] = extractOffset(buffer, "ObjID");
		GameData.Offset["GameInstance"] = extractOffset(buffer, "GameInstance");
		GameData.Offset["LocalPlayer"] = extractOffset(buffer, "LocalPlayer");
		GameData.Offset["Actors"] = extractOffset(buffer, "Actors");
		GameData.Offset["NumAliveTeams"] = extractOffset(buffer, "NumAliveTeams");
		GameData.Offset["ComponentToWorld"] = extractOffset(buffer, "ComponentToWorld");
		GameData.Offset["ComponentLocation"] = extractOffset(buffer, "ComponentLocation");
		GameData.Offset["TimeSeconds"] = extractOffset(buffer, "TimeSeconds");
		GameData.Offset["WorldToMap"] = extractOffset(buffer, "MapGrid_Map");
		GameData.Offset["CurrentLevel"] = extractOffset(buffer, "CurrentLevel");
		GameData.Offset["Mesh"] = extractOffset(buffer, "Mesh");
		GameData.Offset["CharacterMovement"] = extractOffset(buffer, "CharacterMovement");
		GameData.Offset["LastUpdateVelocity"] = extractOffset(buffer, "LastUpdateVelocity");
		GameData.Offset["RootComponent"] = extractOffset(buffer, "RootComponent");
		GameData.Offset["PlayerController"] = extractOffset(buffer, "PlayerController");
		GameData.Offset["PlayerInput"] = extractOffset(buffer, "PlayerInput");
		GameData.Offset["InputYawScale"] = extractOffset(buffer, "InputYawScale");
		GameData.Offset["bShowMouseCursor"] = extractOffset(buffer, "bShowMouseCursor");
		GameData.Offset["AcknowledgedPawn"] = extractOffset(buffer, "AcknowledgedPawn");
		GameData.Offset["MyHUD"] = extractOffset(buffer, "MyHUD");
		GameData.Offset["InputAxisProperties"] = extractOffset(buffer, "InputAxisProperties");
		GameData.Offset["GameState"] = extractOffset(buffer, "GameState");
		GameData.Offset["SafetyZoneRadius"] = extractOffset(buffer, "SafetyZoneRadius");
		GameData.Offset["SafetyZonePosition"] = extractOffset(buffer, "SafetyZonePosition");
		GameData.Offset["BlueZoneRadius"] = extractOffset(buffer, "BlueZoneRadius");
		GameData.Offset["BlueZonePosition"] = extractOffset(buffer, "BlueZonePosition");
		GameData.Offset["PlayerArray"] = extractOffset(buffer, "PlayerArray");
		GameData.Offset["PlayerCameraManager"] = extractOffset(buffer, "PlayerCameraManager");
		GameData.Offset["ViewTarget"] = extractOffset(buffer, "ViewTarget");
		GameData.Offset["CameraCacheFOV"] = extractOffset(buffer, "CameraCacheFOV");
		GameData.Offset["CameraCacheRotation"] = extractOffset(buffer, "CameraCacheRotation");
		GameData.Offset["CameraCacheLocation"] = extractOffset(buffer, "CameraCacheLocation");
		GameData.Offset["GroggyHealth"] = extractOffset(buffer, "GroggyHealth");
		GameData.Offset["CharacterName"] = extractOffset(buffer, "CharacterName");
		GameData.Offset["LastTeamNum"] = extractOffset(buffer, "LastTeamNum");
		GameData.Offset["SpectatedCount"] = extractOffset(buffer, "SpectatedCount");
		GameData.Offset["PlayerState"] = extractOffset(buffer, "PlayerState");
		GameData.Offset["CharacterState"] = extractOffset(buffer, "CharacterState");
		GameData.Offset["AnimScriptInstance"] = extractOffset(buffer, "AnimScriptInstance");
		GameData.Offset["PreEvalPawnState"] = extractOffset(buffer, "PreEvalPawnState");
		GameData.Offset["StaticMesh"] = extractOffset(buffer, "StaticMesh");
		GameData.Offset["Eyes"] = extractOffset(buffer, "Eyes");
		GameData.Offset["AimOffsets"] = extractOffset(buffer, "AimOffsets");
		GameData.Offset["PlayerStatistics"] = extractOffset(buffer, "PlayerStatistics");
		GameData.Offset["DamageDealtOnEnemy"] = extractOffset(buffer, "DamageDealtOnEnemy");
		GameData.Offset["PartnerLevel"] = extractOffset(buffer, "PartnerLevel");
		GameData.Offset["TeamNumber"] = extractOffset(buffer, "TeamNumber");
		GameData.Offset["PlayerName"] = extractOffset(buffer, "PlayerName");
		GameData.Offset["PlayerStatusType"] = extractOffset(buffer, "PlayerStatusType");
		GameData.Offset["SquadMemberIndex"] = extractOffset(buffer, "SquadMemberIndex");
		GameData.Offset["AccountId"] = extractOffset(buffer, "AccountId");
		GameData.Offset["SurvivalTier"] = extractOffset(buffer, "SurvivalTier");
		GameData.Offset["SurvivalLevel"] = extractOffset(buffer, "SurvivalLevel");
		GameData.Offset["CharacterClanInfo"] = extractOffset(buffer, "CharacterClanInfo");
		GameData.Offset["VehicleRiderComponent"] = extractOffset(buffer, "VehicleRiderComponent");
		GameData.Offset["ReplicatedMovement"] = extractOffset(buffer, "ReplicatedMovement");
		GameData.Offset["SeatIndex"] = extractOffset(buffer, "SeatIndex");
		GameData.Offset["LastVehiclePawn"] = extractOffset(buffer, "LastVehiclePawn");
		GameData.Offset["VehicleMovement"] = extractOffset(buffer, "VehicleMovement");
		GameData.Offset["Wheels"] = extractOffset(buffer, "Wheels");
		GameData.Offset["WheelLocation"] = extractOffset(buffer, "WheelLocation");
		GameData.Offset["DampingRate"] = extractOffset(buffer, "DampingRate");
		GameData.Offset["ShapeRadius"] = extractOffset(buffer, "ShapeRadius");
		GameData.Offset["WidgetStateMap"] = extractOffset(buffer, "WidgetStateMap");
		GameData.Offset["TrainingMapGrid"] = extractOffset(buffer, "TrainingMapGrid");
		GameData.Offset["MapGrid_Map"] = extractOffset(buffer, "MapGrid_Map");
		GameData.Offset["LayoutData"] = extractOffset(buffer, "LayoutData");
		GameData.Offset["Visibility"] = extractOffset(buffer, "Visibility");
		GameData.Offset["Slot"] = extractOffset(buffer, "Slot");
		GameData.Offset["Offsets"] = extractOffset(buffer, "Offsets");
		GameData.Offset["Alignment"] = extractOffset(buffer, "Alignment");
		GameData.Offset["BlockInputWidgetList"] = extractOffset(buffer, "BlockInputWidgetList");
		GameData.Offset["SelectMinimapSizeIndex"] = extractOffset(buffer, "SelectMinimapSizeIndex");
		GameData.Offset["ItemID"] = extractOffset(buffer, "ItemID");
		GameData.Offset["ItemTable"] = extractOffset(buffer, "ItemTable");
		GameData.Offset["ItemPackageItems"] = extractOffset(buffer, "ItemPackageItems");
		GameData.Offset["DroppedItemGroup"] = extractOffset(buffer, "DroppedItemGroup");
		GameData.Offset["DroppedItem"] = extractOffset(buffer, "DroppedItem");
		GameData.Offset["DroppedItemGroupUItem"] = extractOffset(buffer, "DroppedItemGroupUItem");
		GameData.Offset["TimeTillExplosion"] = extractOffset(buffer, "TimeTillExplosion");
		GameData.Offset["ExplodeState"] = extractOffset(buffer, "ExplodeState");
		GameData.Offset["WeaponProcessor"] = extractOffset(buffer, "WeaponProcessor");
		GameData.Offset["CurrentAmmoData"] = extractOffset(buffer, "CurrentAmmoData");
		GameData.Offset["CurrentWeaponIndex"] = extractOffset(buffer, "CurrentWeaponIndex");
		GameData.Offset["EquippedWeapons"] = extractOffset(buffer, "EquippedWeapons");
		GameData.Offset["WeaponTrajectoryData"] = extractOffset(buffer, "WeaponTrajectoryData");
		GameData.Offset["TrajectoryGravityZ"] = extractOffset(buffer, "TrajectoryGravityZ");
		GameData.Offset["TrajectoryConfig"] = extractOffset(buffer, "TrajectoryConfig");
		GameData.Offset["BallisticCurve"] = extractOffset(buffer, "BallisticCurve");
		GameData.Offset["FloatCurves"] = extractOffset(buffer, "FloatCurves");
		GameData.Offset["Keys"] = extractOffset(buffer, "Keys");
		GameData.Offset["WeaponConfig_WeaponClass"] = extractOffset(buffer, "WeaponConfig_WeaponClass");
		GameData.Offset["Mesh3P"] = extractOffset(buffer, "Mesh3P");
		GameData.Offset["FiringAttachPoint"] = extractOffset(buffer, "FiringAttachPoint");
		GameData.Offset["AttachedStaticComponentMap"] = extractOffset(buffer, "AttachedStaticComponentMap");
		GameData.Offset["AttachedItems"] = extractOffset(buffer, "AttachedItems");
		GameData.Offset["WeaponAttachmentData"] = extractOffset(buffer, "WeaponAttachmentData");
		GameData.Offset["ScopingAttachPoint"] = extractOffset(buffer, "ScopingAttachPoint");
		GameData.Offset["ElapsedCookingTime"] = extractOffset(buffer, "ElapsedCookingTime");
		GameData.Offset["ControlRotation_CP"] = extractOffset(buffer, "ControlRotation_CP");
		GameData.Offset["RecoilADSRotation_CP"] = extractOffset(buffer, "RecoilADSRotation_CP");
		GameData.Offset["LeanLeftAlpha_CP"] = extractOffset(buffer, "LeanLeftAlpha_CP");
		GameData.Offset["LeanRightAlpha_CP"] = extractOffset(buffer, "LeanRightAlpha_CP");
		GameData.Offset["bIsScoping_CP"] = extractOffset(buffer, "bIsScoping_CP");
		GameData.Offset["bIsReloading_CP"] = extractOffset(buffer, "bIsReloading_CP");
		GameData.Offset["StaticSockets"] = extractOffset(buffer, "StaticSockets");
		GameData.Offset["StaticSocketName"] = extractOffset(buffer, "StaticSocketName");
		GameData.Offset["StaticRelativeLocation"] = extractOffset(buffer, "StaticRelativeLocation");
		GameData.Offset["StaticRelativeRotation"] = extractOffset(buffer, "StaticRelativeRotation");
		GameData.Offset["StaticRelativeScale"] = extractOffset(buffer, "StaticRelativeScale");
		GameData.Offset["AntiCheatCharacterSyncManager"] = extractOffset(buffer, "AntiCheatCharacterSyncManager");

		return;
	}
}





