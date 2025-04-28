#pragma once
#include <DMALibrary/Memory/Memory.h>
#include <Common/Data.h>
#include <Common/Entitys.h>
#include <Utils/Utils.h>
#include <Utils/Throttler.h>
#include <Hack/Decrypt.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <Hack/GNames.h>
#include <Utils/Timer.h>
#include <Hack/Process.h>
#include <Hack/LineTrace.h>
inline std::queue<std::pair<std::string, PlayerRankList>> RankWorkQueue;              // 排行队列
inline std::mutex RankQueueMutex;                                                     // 队列互斥锁
inline std::condition_variable RankQueueCondition;                                    // 队列条件变量
inline int PlayerRankWorkCount = 0;                                                   // 排行工作计数
static bool lastZhixiangmoshi = GameData.Config.Overlay.zhixiangmoshi;
static bool settingsApplied = false;
inline uint8_t* xe_decrypt_container = (uint8_t*)VirtualAlloc(nullptr, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); // 解密容器

class Players
{
public:

	static void DecryptedFloat(float* a1, unsigned int a2, char a3) {
		uint32_t v7[16] = { GameData.Offset["DecryptedHealthOffsets0"], GameData.Offset["DecryptedHealthOffsets1"], GameData.Offset["DecryptedHealthOffsets2"], GameData.Offset["DecryptedHealthOffsets3"],
							GameData.Offset["DecryptedHealthOffsets4"], GameData.Offset["DecryptedHealthOffsets5"], GameData.Offset["DecryptedHealthOffsets6"], GameData.Offset["DecryptedHealthOffsets7"],
							GameData.Offset["DecryptedHealthOffsets8"], GameData.Offset["DecryptedHealthOffsets9"], GameData.Offset["DecryptedHealthOffsets10"], GameData.Offset["DecryptedHealthOffsets11"],
							GameData.Offset["DecryptedHealthOffsets12"], GameData.Offset["DecryptedHealthOffsets13"], GameData.Offset["DecryptedHealthOffsets14"], GameData.Offset["DecryptedHealthOffsets15"] };

		auto* ptr = reinterpret_cast<unsigned char*>(a1); // 将float指针转换为字节指针
		for (unsigned int i = 0; i < a2; ++i) {
			char v5 = i + a3;
			ptr[i] ^= reinterpret_cast<char*>(v7)[v5 & 0x3F];
		}
	}

	static void LoadLists(int type)
	{
		std::unordered_map <std::string, int> PlayerWhiteLists;
		std::unordered_map <std::string, int> PlayerBlackLists;
		std::string filename = "Config/BlackLists.txt";
		if (type == 2) {
			filename = "Config/WhiteLists.txt";
		}

		std::ifstream file(filename);
		if (!file.is_open()) {
			return;
		}

		std::string line;
		while (std::getline(file, line)) {
			if (type == 2) {
				PlayerWhiteLists[line] = 1;
			}
			else {
				PlayerBlackLists[line] = 1;
			}
		}

		if (type == 2) {
			Data::SetPlayerWhiteLists(PlayerWhiteLists);
		}
		else {
			Data::SetPlayerBlackLists(PlayerBlackLists);
		}

		file.close();
	}

	static void SaveLists(int type) {
		std::unordered_map <std::string, int> PlayerWhiteLists = Data::GetPlayerWhiteLists();
		std::unordered_map <std::string, int> PlayerBlackLists = Data::GetPlayerBlackLists();
		std::string filename;
		std::unordered_map<std::string, int> currentList;

		if (type == 2) {
			filename = "Config/WhiteLists.txt";
			currentList = PlayerWhiteLists;
		}
		else {
			filename = "Config/BlackLists.txt";
			currentList = PlayerBlackLists;
		}

		std::ofstream file(filename);
		if (!file.is_open()) {
			return;
		}

		for (const auto& entry : currentList) {
			file << entry.first << std::endl;
		}

		file.close();
	}

	static void AddToBlackList(const std::string& name) {
		Data::DeletePlayerWhiteListsItem(name);
		SaveLists(2);
		Data::SetPlayerBlackListsItem(name, 1);
		SaveLists(1);
	}

	static void AddToWhiteList(const std::string& name) {
		Data::DeletePlayerBlackListsItem(name);
		SaveLists(1);
		Data::SetPlayerWhiteListsItem(name, 1);
		SaveLists(2);
	}

	static void RemoveList(const std::string& name) {
		Data::DeletePlayerBlackListsItem(name);
		Data::DeletePlayerWhiteListsItem(name);
		SaveLists(1);
		SaveLists(2);
	}

	static void ReadPlayerLists()
	{
		LoadLists(1);
		LoadLists(2);
	}

	static void UpdateFogPlayers()
	{
		while (true)
		{
			if (GameData.Scene != Scene::Gaming)
			{
				Sleep(GameData.ThreadSleep);
				continue;
			}

			std::unordered_map<uint64_t, FogPlayerInfo> FogPlayerInfos;
			TArray<uint64_t> DormantCharacterClientList = mem.Read<TArray<uint64_t>>(GameData.AntiCheatCharacterSyncManager + GameData.Offset["DormantCharacterClientList"]);
			for (auto FogPlayerEntity : DormantCharacterClientList.GetVector())
			{
				FogPlayerInfo FogPlayer;
				FogPlayerInfos[FogPlayerEntity] = FogPlayer;
			}

			Data::SetFogPlayers(FogPlayerInfos);

			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	
	static void UpdatePlayerLists()
	{
		if (GameData.Scene == Scene::Gaming)
		{
			auto PlayerBlackLists = Data::GetPlayerBlackLists();
			auto PlayerWhiteLists = Data::GetPlayerWhiteLists();
			auto LocalPlayerInfo = GameData.LocalPlayerInfo;
			auto ScatterHandle = mem.CreateScatterHandle();

			std::unordered_map<std::string, GamePlayerInfo> GPlayerLists;
			std::unordered_map<std::string, PlayerRankList> PlayerRankLists = Data::GetPlayerRankLists();

			std::vector<GamePlayerInfo> playerLists;
			GameData.PlayerCount = mem.Read<int>(GameData.GameState + GameData.Offset["PlayerArray"] + 0x8);  // 读取玩家数量
			GameData.NumAliveTeams = mem.Read<int>(GameData.GameState + GameData.Offset["NumAliveTeams"]);
			if (GameData.PlayerCount <= 0)
			{
				return;
			}
			// 根据玩家数量分配缓冲区
			uint64_t* PlayerArrayBuffer = new uint64_t[GameData.PlayerCount];
			// 读取玩家数组并将其存储在缓冲区中
			mem.Read(mem.Read<uint64_t>(GameData.GameState + GameData.Offset["PlayerArray"]), PlayerArrayBuffer, sizeof(uint64_t) * GameData.PlayerCount);
			// 将缓冲区内容复制到一个 std::vector 中
			std::vector<uint64_t> PlayerArray(PlayerArrayBuffer, PlayerArrayBuffer + GameData.PlayerCount);

			// 遍历玩家数组中的每个玩家信息指针
			for (auto& pPlayerInfo : PlayerArray)
			{
				GamePlayerInfo player;             // 创建一个 GamePlayerInfo 结构体实例
				player.pPlayerInfo = pPlayerInfo;  // 将当前玩家信息指针赋值给 player 结构体中的 pPlayerInfo 成员
				if (player.TeamID == 0) {
					continue;
				}
				else {
					playerLists.push_back(player);     // 将 player 结构体添加到 playerLists 向量中
				}
				
			}

			for (GamePlayerInfo& player : playerLists)
			{
				mem.AddScatterReadRequest(ScatterHandle, player.pPlayerInfo + GameData.Offset["PlayerName"], (uint64_t*)&player.pPlayerName);
				mem.AddScatterReadRequest(ScatterHandle, player.pPlayerInfo + GameData.Offset["PlayerStatusType"], (BYTE*)&player.StatusType);
				mem.AddScatterReadRequest(ScatterHandle, player.pPlayerInfo + GameData.Offset["AccountId"], (uint64_t*)&player.pAccountId);
				mem.AddScatterReadRequest(ScatterHandle, player.pPlayerInfo + GameData.Offset["TeamNumber"], (int*)&player.TeamID);
				mem.AddScatterReadRequest(ScatterHandle, player.pPlayerInfo + GameData.Offset["SquadMemberIndex"], (int*)&player.SquadMemberIndex);
				mem.AddScatterReadRequest(ScatterHandle, player.pPlayerInfo + GameData.Offset["PartnerLevel"], (EPartnerLevel*)&player.PartnerLevel);
				//mem.AddScatterReadRequest(ScatterHandle, player.pPlayerInfo + GameData.Offset["PubgIdData"], (FWuPubgIdData*)&player.PubgIdData);
				mem.AddScatterReadRequest(ScatterHandle, player.pPlayerInfo + GameData.Offset["PlayerStatistics"], (int*)&player.KillCount);
				mem.AddScatterReadRequest(ScatterHandle, player.pPlayerInfo + GameData.Offset["DamageDealtOnEnemy"], (float*)&player.DamageDealtOnEnemy);
				mem.AddScatterReadRequest(ScatterHandle, player.pPlayerInfo + GameData.Offset["CharacterClanInfo"] + 0x20, (uint64_t*)&player.pClanName);
				mem.AddScatterReadRequest(ScatterHandle, player.pPlayerInfo + GameData.Offset["SurvivalTier"], (int*)&player.PubgIdData.SurvivalTier);
				mem.AddScatterReadRequest(ScatterHandle, player.pPlayerInfo + GameData.Offset["SurvivalLevel"], (int*)&player.PubgIdData.SurvivalLevel);
				//mem.AddScatterReadRequest(ScatterHandle, player.pPlayerInfo + GameData.Offset["SpectatedCount"], (int*)&player.SpectatedCount);
			}

			mem.ExecuteReadScatter(ScatterHandle);

			for (GamePlayerInfo& player : playerLists)
			{
				mem.AddScatterReadRequest(ScatterHandle, player.pClanName, (FText*)&player.FClanName);
				mem.AddScatterReadRequest(ScatterHandle, player.pPlayerName, (FText*)&player.FPlayerName);
				mem.AddScatterReadRequest(ScatterHandle, player.pAccountId, (FText*)&player.FAccountId);
			}

			mem.ExecuteReadScatter(ScatterHandle);

			for (GamePlayerInfo& player : playerLists)
			{
				player.ClanName = Utils::UnicodeToAnsi(player.FClanName.buffer);
				player.AccountId = Utils::UnicodeToAnsi(player.FAccountId.buffer);
				player.PlayerName = Utils::UnicodeToAnsi(player.FPlayerName.buffer);
				//player.PubgIdData.SurvivalLevel = (player.PubgIdData.SurvivalTier - 1) * 500 + player.PubgIdData.SurvivalLevel;
				player.TeamID = (player.TeamID >= 100000) ? (player.TeamID - 100000) : player.TeamID;
				player.IsMyTeam = GameData.LocalPlayerInfo.TeamID == player.TeamID;

				if (PlayerBlackLists.find(player.AccountId) != PlayerBlackLists.end()) {
					player.ListType = 1;
				}
				else if (PlayerWhiteLists.find(player.AccountId) != PlayerWhiteLists.end()) {
					player.ListType = 2;
				}

				if (player.PubgIdData.SurvivalTier > 0) player.PubgIdData.SurvivalLevel = (player.PubgIdData.SurvivalTier - 1) * 500 + player.PubgIdData.SurvivalLevel;
			}

			// 遍历 playerLists 向量中的每个 GamePlayerInfo 结构体
			for (GamePlayerInfo& player : playerLists)
			{
				if (player.PlayerName == "") {
					continue;
				}

				// 将当前玩家信息存储到 GPlayerLists 映射中，以玩家名字作为键
				GPlayerLists[player.PlayerName] = player;

				// 如果 PlayerRankLists 中没有当前玩家的名字，并且玩家状态为存活 (8) 或人机 (12)
				if (PlayerRankLists.count(player.PlayerName) == 0)
				{
					PlayerRankList playerRankList;
					playerRankList.AccountId = player.AccountId;
					playerRankList.PlayerName = player.PlayerName;
					playerRankList.Tem = player.TeamID;
					playerRankList.DamageAmount = player.DamageDealtOnEnemy;
					playerRankList.Survivallevel = player.PubgIdData.SurvivalLevel;
					Data::SetPlayerRankListsItem(playerRankList.AccountId, playerRankList);
					//PlayerRankList playerRankList;  // 创建一个 PlayerRankList 结构体实例
					//PlayerRankLists[player.PlayerName] = playerRankList;  // 将 playerRankList 添加到 PlayerRankLists 映射中
					//Data::SetPlayerRankListsItem(player.PlayerName, {});  // 调用函数设置玩家排名列表项
				}
			}


			Data::SetPlayerLists(GPlayerLists);

			mem.CloseScatterHandle(ScatterHandle);
		}
		else {
			Data::SetPlayerLists({});
		}
	}

	static void Update()
	{
		Throttler Throttlered;
		Throttler ThrottleredSleep;
		auto hScatter = mem.CreateScatterHandle();
		std::unordered_map<uint64_t, tMapInfo> EnemyInfoMap;
		float TimeSeconds = 0.f;
		while (true)
		{
			if (GameData.Scene != Scene::Gaming)
			{
				TimeSeconds = 0.f;
				GameData.FogPlayerCount = 0;
				EnemyInfoMap.clear();
				GameData.PlayerSegmentLists.clear();
				GameData.PlayerRankLists.clear();
				Data::SetPlayerLists({});
				GameData.LocalPlayerInfo = Player();
				Sleep(GameData.ThreadSleep);
				continue;
			}

			int FogPlayerCount = 0;

			std::unordered_map<uint64_t, Player> CachePlayers = Data::GetCachePlayers();
			std::unordered_map<uint64_t, Player> PlayersData = Data::GetPlayersData();
			std::unordered_map<uint64_t, FogPlayerInfo> FogPlayers = Data::GetFogPlayers();

			Throttlered.executeTask("UpdatePlayersData", std::chrono::milliseconds(88), [&hScatter, &CachePlayers, &PlayersData] {
				for (auto& Item : CachePlayers)
				{
					Player& Player = Item.second;

					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["RootComponent"], (uint64_t*)&Player.RootComponent);
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["CharacterMovement"], (uint64_t*)&Player.CharacterMovement);
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["Mesh"], (uint64_t*)&Player.MeshComponent);
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["PlayerState"], (uint64_t*)&Player.PlayerState);
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["GroggyHealth"], (float*)&Player.GroggyHealth);
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["SpectatedCount"], (int*)&Player.SpectatedCount);
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["LastTeamNum"], (int*)&Player.TeamID);
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["CharacterName"], (uint64_t*)&Player.pCharacterName);
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["CharacterState"], (ECharacterState*)&Player.CharacterState);
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["AimOffsets"], (FRotator*)&Player.AimOffsets);
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["WeaponProcessor"], (uint64_t*)&Player.WeaponProcessor);
					//mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["Gender"], (EGender*)&Player.Gender);
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["bEncryptedHealth"], (BYTE*)&Player.bEncryptedHealth);
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["EncryptedHealthOffset"], (unsigned char*)&Player.EncryptedHealthOffset);
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["DecryptedHealthOffset"], (unsigned char*)&Player.DecryptedHealthOffset);
				}
				mem.ExecuteReadScatter(hScatter);

				for (auto& Item : CachePlayers)
				{
					Player& Player = Item.second;


					Player.RootComponent = Decrypt::Xe(Player.RootComponent);
					Player.CharacterMovement = Decrypt::Xe(Player.CharacterMovement);

					Player.PlayerState = Decrypt::Xe(Player.PlayerState);


					mem.AddScatterRead(hScatter, Player.pCharacterName, (FText*)&Player.CharacterName);
					mem.AddScatterRead(hScatter, Player.MeshComponent + GameData.Offset["AnimScriptInstance"], (uint64_t*)&Player.AnimScriptInstance);
					mem.AddScatterRead(hScatter, Player.RootComponent + GameData.Offset["ComponentLocation"], (FVector*)&Player.Location);
				}
				mem.ExecuteReadScatter(hScatter);

				auto GamePlayerLists = Data::GetPlayerLists();
				
				auto PlayerRankLists = Data::GetPlayerRankLists();
				// 循环更新缓存中的玩家信息
				for (auto& Item : CachePlayers)
				{
					// 获取玩家对象的引用
					Player& Player = Item.second;

					// 判断玩家是否为自己
					if (Player.Entity == GameData.CameraViewTarget)
						Player.IsMe = true;

					// 调整团队ID
					Player.TeamID = (Player.TeamID >= 100000) ? (Player.TeamID - 100000) : Player.TeamID;

					// 检查玩家是否为本队成员
					if (GameData.Config.ESP.duiyou)
						Player.IsMyTeam = false;
					else
						Player.IsMyTeam = Player.TeamID == GameData.LocalPlayerTeamID;

					// 将玩家名称转换为ANSI格式，并去除多余字符
					Player.Name = Utils::RemoveBracketsAndTrim(Utils::UnicodeToAnsi(Player.CharacterName.buffer));


					// 从GamePlayerLists中获取玩家详细信息
					GamePlayerInfo PlayerInfo = GamePlayerLists[Player.Name];

					// 填充玩家对象的详细信息
					Player.ClanName = PlayerInfo.ClanName;
					//Player.SpectatedCount = PlayerInfo.SpectatedCount;
					Player.SurvivalLevel = PlayerInfo.PubgIdData.SurvivalLevel;
					Player.PartnerLevel = PlayerInfo.PartnerLevel;
					Player.DamageDealtOnEnemy = PlayerInfo.DamageDealtOnEnemy;
					Player.Alignment = PlayerInfo.Alignment;
					Player.KillCount = PlayerInfo.KillCount;
					Player.ListType = PlayerInfo.ListType;
					Player.SquadMemberIndex = PlayerInfo.SquadMemberIndex;
					Player.AccountId = PlayerInfo.AccountId;
					// 将玩家位置转换为屏幕坐标
					FVector2D WorldToScreen = VectorHelper::WorldToScreen(Player.Location);

					// 如果玩家不是自己且不在屏幕内，则标记为不在屏幕内并跳过后续处理
					if (!Player.IsMe && (WorldToScreen.X < -100 || WorldToScreen.X > GameData.Config.Overlay.ScreenWidth + 100 || WorldToScreen.Y < -100 || WorldToScreen.Y > GameData.Config.Overlay.ScreenHeight + 100))
					{
						Player.InScreen = false;
						continue;
					}

					// 添加对动画状态和武器信息的读取
					mem.AddScatterRead(hScatter, Player.AnimScriptInstance + GameData.Offset["PreEvalPawnState"], (EAnimPawnState*)&Player.PreEvalPawnState);
					mem.AddScatterRead(hScatter, Player.WeaponProcessor + GameData.Offset["EquippedWeapons"], (uint64_t*)&Player.EquippedWeapons);
					mem.AddScatterRead(hScatter, Player.WeaponProcessor + GameData.Offset["CurrentWeaponIndex"], (BYTE*)&Player.CurrentWeaponIndex);
				}
				mem.ExecuteReadScatter(hScatter);

				// 读取玩家当前武器的信息
				for (auto& Item : CachePlayers)
				{
					Player& Player = Item.second;
					if (!Player.InScreen)
					{
						continue;
					}
					// 检查当前武器索引是否在有效范围内
					if (Player.CurrentWeaponIndex >= 0 && Player.CurrentWeaponIndex < 8)
					{
						mem.AddScatterRead(hScatter, Player.EquippedWeapons + Player.CurrentWeaponIndex * 8, (uint64_t*)&Player.CurrentWeapon);
					}
				}
				mem.ExecuteReadScatter(hScatter);
			
				// 读取玩家是否在瞄准状态
				for (auto& Item : CachePlayers)
				{
					Player& Player = Item.second;
					if (!Player.InScreen)
					{
						continue;
					}
					// 检查当前武器是否有效
					if (Player.CurrentWeapon > 0)
					{
						// 读取玩家当前武器的ID
						mem.AddScatterRead(hScatter, Player.CurrentWeapon + GameData.Offset["ObjID"], (int*)&Player.WeaponID);

						// 读取玩家当前武器的类别
						//mem.AddScatterRead(hScatter, Player.CurrentWeapon + GameData.Offset["WeaponConfig_WeaponClass"], (EWeaponClass*)&Player.WeaponClass);

						mem.AddScatterRead(hScatter, Player.CurrentWeapon + GameData.Offset["WeaponConfig_WeaponClass"], (BYTE*)&Player.WeaponClassByte);

						// 如果玩家是自己
						if (Player.IsMe)
						{
							// 读取武器的时间（例如，手雷的引爆时间）
							mem.AddScatterRead(hScatter, Player.CurrentWeapon + GameData.Offset["ElapsedCookingTime"], (float*)&Player.ElapsedCookingTime);

							// 读取玩家是否在瞄准状态
							mem.AddScatterRead(hScatter, Player.AnimScriptInstance + GameData.Offset["bIsScoping_CP"], (bool*)&Player.IsScoping);
						}
					}
				}


				mem.ExecuteReadScatter(hScatter);

				// 获取武器的显示名称
				for (auto& Item : CachePlayers)
				{
					// 获取玩家对象的引用
					Player& Player = Item.second;

					// 解密玩家武器的ID
					Player.WeaponID = Decrypt::CIndex(Player.WeaponID);

					// 根据解密后的武器ID获取武器实体信息
					Player.WeaponEntityInfo = Data::GetGNameListsByIDItem(Player.WeaponID);

					// 获取武器的显示名称
					Player.WeaponName = Player.WeaponEntityInfo.DisplayName;

					// 将更新后的玩家信息保存到 PlayersData 容器中
					PlayersData[Player.Entity] = Player;
				}

				});

			for (auto& Item : CachePlayers)
			{
				Player& Player = Item.second;

				// 检查 PlayersData 容器中是否包含当前玩家的实体
				if (PlayersData.count(Player.Entity) > 0)
				{
					// 如果包含，则从 PlayersData 容器中获取该玩家的信息
					Player = PlayersData[Player.Entity];

					// 标记玩家在屏幕内
					Player.InScreen = true;
				}
			}

			for (auto& Item : CachePlayers)
			{
				Player& Player = Item.second;


				if (Player.bEncryptedHealth == 0)
				{
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["Health"], (float*)&Player.Health);
				}
				else {
					mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["Health"] + Player.EncryptedHealthOffset, (float*)&Player.Health);
				}

				// 读取玩家的其他属性
				mem.AddScatterRead(hScatter, Player.MeshComponent + GameData.Offset["Eyes"], (int*)&Player.Eyes);
				mem.AddScatterRead(hScatter, Player.MeshComponent + GameData.Offset["StaticMesh"], (uint64_t*)&Player.StaticMesh);
				mem.AddScatterRead(hScatter, Player.MeshComponent + GameData.Offset["bAlwaysCreatePhysicsState"], (UCHAR*)&Player.bAlwaysCreatePhysicsState);
				mem.AddScatterRead(hScatter, Player.MeshComponent + GameData.Offset["ComponentToWorld"], (FTransform*)&Player.ComponentToWorld);
				mem.AddScatterRead(hScatter, Player.RootComponent + GameData.Offset["ComponentLocation"], (FVector*)&Player.Location);

			}
			mem.ExecuteReadScatter(hScatter);

			for (auto& Item : CachePlayers)
			{
				Player& Player = Item.second;

				if (Player.bEncryptedHealth != 0) {
					DecryptedFloat(&Player.Health, sizeof(Player.Health), Player.DecryptedHealthOffset);
				}

				// 将玩家的世界坐标转换为屏幕坐标
				FVector2D WorldToScreen = VectorHelper::WorldToScreen(Player.Location);
				//过滤迷雾写法
				if (GameData.Config.ESP.miwu)
				{
					if (Player.bAlwaysCreatePhysicsState < 6) ///6=没在迷雾  4=在迷雾
					{
						continue;
					}
					else {

						if (!Player.IsMe && (WorldToScreen.X < -100 || WorldToScreen.X > GameData.Config.Overlay.ScreenWidth + 100 || WorldToScreen.Y < -100 || WorldToScreen.Y > GameData.Config.Overlay.ScreenHeight + 100))
						{
							Player.InScreen = false;
							continue;
						}

					}
				}
				else
				{
					if (Player.bAlwaysCreatePhysicsState == 4) ///6=没在迷雾  4=在迷雾
					{
						//Utils::Log(1, "%s %f %f %f", Player.Name, Player.ComponentToWorld.Translation.X, Player.ComponentToWorld.Translation.Y , Player.ComponentToWorld.Translation.Z);
						Player.InFog = true;
						Player.InScreen = false;
						FogPlayerCount++;
					}
					else {

						if (!Player.IsMe && (WorldToScreen.X < -100 || WorldToScreen.X > GameData.Config.Overlay.ScreenWidth + 100 || WorldToScreen.Y < -100 || WorldToScreen.Y > GameData.Config.Overlay.ScreenHeight + 100))
						{
							Player.InScreen = false;
							continue;
						}

					}

				}

				// 检查玩家是否在雾中
				if (Player.bAlwaysCreatePhysicsState == 4)
				{
					Player.InFog = true;
					Player.InScreen = false;
					FogPlayerCount++;
				}
				else {
					// 检查玩家是否在屏幕范围内
					if (!Player.IsMe && (WorldToScreen.X < -100 || WorldToScreen.X > GameData.Config.Overlay.ScreenWidth + 100 || WorldToScreen.Y < -100 || WorldToScreen.Y > GameData.Config.Overlay.ScreenHeight + 100))
					{
						Player.InScreen = false;
						continue;
					}
				}

				// 如果玩家不在屏幕上或者是自己或者是自己的队友，跳过该玩家
				if (!Player.InScreen || Player.IsMe || Player.IsMyTeam)
				{
					continue;
				}
				if (GameData.Config.Overlay.zhixiangmoshi != lastZhixiangmoshi) {
					// 如果模式发生切换，重置设置应用标志
					settingsApplied = false;
				}

				// 如果模式切换或未应用设置，执行设置
				if (GameData.Config.Overlay.zhixiangmoshi && !settingsApplied) {
					// 模式1: 设置为 0
					GameData.Config.ESP.InfoDistanceMax = 0;
					GameData.Config.ESP.WeaponDistanceMax = 0;
					settingsApplied = true;  // 标记设置已应用
				}
				else if (!GameData.Config.Overlay.zhixiangmoshi && !settingsApplied) {
					// 模式2: 设置为 300 和 200
					GameData.Config.ESP.InfoDistanceMax = 300;
					GameData.Config.ESP.WeaponDistanceMax = 200;
					settingsApplied = true;  // 标记设置已应用
				}
				lastZhixiangmoshi = GameData.Config.Overlay.zhixiangmoshi;
				// 遍历每个骨骼
				for (EBoneIndex Bone : SkeletonLists::Bones)
				{
					int BoneIndex = Bone;

					mem.AddScatterRead(hScatter, Player.StaticMesh + (static_cast<unsigned long long>(BoneIndex) * 0x30), (FTransform*)&Player.Skeleton.Bones[Bone]);
				}
			}
			mem.ExecuteReadScatter(hScatter);

			if (!GameData.AimBot.Lock)
			{
				GameData.AimBot.Type = EntityType::Player;
				GameData.AimBot.Target = 0;
				GameData.AimBot.Bone = 0;
				GameData.AimBot.ScreenDistance = 1000.0f;
			}

			AimBotConfig Config = GameData.Config.AimBot.Configs[GameData.Config.AimBot.ConfigIndex].Weapon[WeaponTypeToString[GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType]];
			if (Config.DynamicFov) {
				Config.FOV = Config.FOV * (90.0f / GameData.Camera.FOV);
				Config.WheelFOV = Config.FOV * (90.0f / GameData.Camera.FOV);
			}

			bool IsWheelKeyDown = GameData.Keyboard.IsKeyDown(Config.Wheel.Key);
			// 用于存储所有符合条件的目标
			std::vector<FGameData::TargetInfo> validTargets;
			// 遍历缓存的玩家数据
			for (auto& Item : CachePlayers)
			{
				Player& Player = Item.second; // 获取玩家对象

				// 计算玩家是否可见
				Player.IsVisible = Player.Eyes + 0.05 >= GameData.LocalPlayerInfo.Eyes;

				// 计算玩家与本地玩家的距离
				Player.Distance = GameData.Camera.Location.Distance(Player.Location) / 100.0f;

				// 初始化玩家的瞄准状态和生存状态
				Player.IsAimMe = false;
				Player.State =
					Player.Health > 0.0f ? CharacterState::Alive :
					Player.GroggyHealth > 0.0f ? CharacterState::Groggy :
					CharacterState::Dead;

				// 根据不同的玩家状态设置雷达图标
				if (Player.CharacterState == ECharacterState::Offline)
				{
					Player.RadarState = ECharacterIconType::Quitter; // 掉线
				}
				else if (Player.State == CharacterState::Groggy)
				{
					Player.RadarState = ECharacterIconType::Groggy; // 倒地
				}

				// 更新玩家位置
				Player.Location = Player.ComponentToWorld.Translation;

				// 计算时间差和玩家位置信息
				float TimeStampDelta = GameData.WorldTimeSeconds - EnemyInfoMap[Player.Entity].TimeStamp;
				EnemyInfoMap[Player.Entity].TimeStamp = GameData.WorldTimeSeconds;

				// 更新玩家位置信息
				[&] {
					auto& PosInfo = EnemyInfoMap[Player.Entity].PosInfo.Info;

					if (Player.State == CharacterState::Dead) {
						PosInfo.clear(); // 玩家死亡时清空位置信息
					}
					else {
						// 更新位置信息队列
						if (TimeStampDelta)
							PosInfo.push_front({ GameData.WorldTimeSeconds, Player.Location });

						if (PosInfo.size() > 200)
							PosInfo.pop_back(); // 控制位置信息队列的最大长度

						// 计算玩家的速度
						float SumTimeDelta = 0.0f;
						FVector SumPosDif;

						for (size_t i = 1; i < PosInfo.size(); i++) {
							const float DeltaTime = PosInfo[i - 1].Time - PosInfo[i].Time;
							const FVector DeltaPos = PosInfo[i - 1].Pos - PosInfo[i].Pos;
							const FVector DeltaVelocity = DeltaPos * (1.0f / DeltaTime);
							const float DeltaSpeedPerHour = DeltaVelocity.Length() / 100.0f * 3.6f;

							if (DeltaTime > 0.05f || DeltaSpeedPerHour > 500.0f) {
								PosInfo.clear(); // 过大的时间差或速度，清空位置信息队列
							}
							else {
								SumTimeDelta = SumTimeDelta + DeltaTime;
								SumPosDif = SumPosDif + DeltaPos;

								if (SumTimeDelta > 0.15f)
									break;
							}
						}
						if (SumTimeDelta > 0.1f) {
							Player.Velocity = SumPosDif * (1.0f / SumTimeDelta); // 计算平均速度
						}
					}
					}();

				PlayersData[Player.Entity] = Player; // 更新玩家数据

				if (Player.IsMe)
				{
					GameData.LocalPlayerInfo = Player; // 更新本地玩家信息
					GameData.LocalPlayerTeamID = Player.TeamID; // 更新本地玩家队伍ID
				}

				if (!Player.InScreen || Player.IsMe || Player.IsMyTeam)
				{
					continue; // 如果玩家不在屏幕内或是本地玩家或是本队玩家，继续下一次循环
				}

				// 计算玩家与本地玩家的角度差
				FVector AimFov = VectorHelper::CalculateAngles(Player.Location, GameData.LocalPlayerInfo.Location);
				FRotator AmiMz = Player.AimOffsets;
				AmiMz.Clamp();
				int32_t AimX = int32_t(abs(AimFov.X - AmiMz.Yaw));
				int32_t AimY = int32_t(abs(AimFov.Y - AmiMz.Pitch));

				// 判断玩家是否在瞄准范围内
				if (AimX <= 2 && AimY <= 2)
				{
					Player.IsAimMe = true;
				}

				for (EBoneIndex Bone : SkeletonLists::Bones)
				{
					Player.Skeleton.LocationBones[Bone] = VectorHelper::GetBoneWithRotation(Player.Skeleton.Bones[Bone], Player.ComponentToWorld);
					Player.Skeleton.ScreenBones[Bone] = VectorHelper::WorldToScreen(Player.Skeleton.LocationBones[Bone]);
					Player.Skeleton.VisibleBones[Bone] = LineTrace::LineTraceSingle(GameData.Camera.Location, Player.Skeleton.LocationBones[Bone]);
				}
				if (GameData.Config.ESP.AdjustableDistance)
				{

					Player.IsVisible = LineTrace::LineTraceSingle(GameData.Camera.Location, Player.Skeleton.LocationBones[EBoneIndex::Head]);
				}
				// 自瞄筛选
				if ((!GameData.AimBot.Lock || Utils::ValidPtr(GameData.AimBot.Target)) && !IsWheelKeyDown)
				{
					bool AllowAim = Player.State != CharacterState::Dead;
					auto Bones = Config.First.Bones;

					bool firstKeyCondition = GameData.Keyboard.IsKeyDown(Config.First.Key);
					bool secondKeyCondition = GameData.Keyboard.IsKeyDown(Config.Second.Key);

					// 根据瞄准快捷键和合并规则设置瞄准骨骼
					if (firstKeyCondition && Config.HotkeyMerge && secondKeyCondition)
					{
						Bones = Config.Second.Bones;
					}
					else if (!Config.HotkeyMerge && secondKeyCondition)
					{
						Bones = Config.Second.Bones;
					}

					if (GameData.Keyboard.IsKeyDown(Config.Groggy.Key))
					{
						Bones = Config.Groggy.Bones;
						AllowAim = Player.State == CharacterState::Groggy;
					}

					if (AllowAim && !Player.IsMyTeam && !Player.IsMe)
					{
						for (int i = 0; i < 17; i++)
						{
							if (Bones[i])
							{
								FVector2D ScreenLocation = Player.Skeleton.ScreenBones[BoneIndex[i]];
								float Distance = Utils::CalculateDistance(GameData.Config.Overlay.ScreenWidth / 2, GameData.Config.Overlay.ScreenHeight / 2, ScreenLocation.X, ScreenLocation.Y);

								if (Config.LockMode == 0) {
									
									Distance = Player.Distance;
									if (Distance < GameData.AimBot.ScreenDistance) {
										GameData.AimBot.ScreenDistance = Distance;
										GameData.AimBot.Target = Player.Entity;
										GameData.AimBot.Bone = BoneIndex[i];
										GameData.AimBot.Type = EntityType::Player;
									}

								}
								//else if (Config.LockMode == 1) {

									//if (Distance <= Config.FOV) {
										//GameData.AimBot.ScreenDistance = Distance;
										//GameData.AimBot.Target = Player.Entity;
										//GameData.AimBot.Bone = BoneIndex[i];
										//GameData.AimBot.Type = EntityType::Player;
								else if (Config.LockMode == 1) {

									if (Distance <= Config.FOV) {
										if (Distance < GameData.AimBot.ScreenDistance) {
											GameData.AimBot.ScreenDistance = Distance;
											GameData.AimBot.Target = Player.Entity;
											GameData.AimBot.Bone = BoneIndex[i];
											GameData.AimBot.isV = Player.Skeleton.VisibleBones[BoneIndex[i]];
											GameData.AimBot.Type = EntityType::Player;
											//Utils::Log(1, "%f", GameData.AimBot.ScreenDistance);	
										}																
									}

								}
	
							}
						}
					}
				}

				if (GameData.AimBot.Target == Player.Entity) {
					GameData.AimBot.TargetPlayerInfo = Player;
				}
			}

			
			if ((!GameData.AimBot.Lock || Utils::ValidPtr(GameData.AimBot.Target)) && IsWheelKeyDown && Config.AimWheel)
			{
				std::unordered_map<uint64_t, VehicleWheelInfo> VehicleWheels = Data::GetVehicleWheels();

				for (auto& Item : VehicleWheels)
				{
					auto& Wheel = Item.second;

					bool AllowAim = Wheel.State == WheelState::Normal;

					Wheel.ScreenLocation = VectorHelper::WorldToScreen(Wheel.Location);
					float Distance = Utils::CalculateDistance(GameData.Config.Overlay.ScreenWidth / 2, GameData.Config.Overlay.ScreenHeight / 2, Wheel.ScreenLocation.X, Wheel.ScreenLocation.Y);
					if (Distance <= Config.WheelFOV) {
						if (Distance < GameData.AimBot.ScreenDistance) {
							GameData.AimBot.ScreenDistance = Distance;
							GameData.AimBot.Target = Wheel.Wheel;
							GameData.AimBot.Bone = 0;
							GameData.AimBot.Type = EntityType::Wheel;
						}
					}
				}
			}

			Data::SetPlayers(CachePlayers);
			Data::SetPlayersData(PlayersData);
			GameData.FogPlayerCount = FogPlayerCount;

		}
		mem.CloseScatterHandle(hScatter);
	}
};