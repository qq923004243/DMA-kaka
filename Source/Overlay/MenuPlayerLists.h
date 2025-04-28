#pragma once
#include "imgui/imgui.h"
#include "common/Data.h"
#include <Hack/Players.h>
RECT menuRect1;
namespace MenuPlayerLists
{

	inline char filterPlayerName[256] = "";
	inline void SortItemsByGamePlayerInfo(std::vector<GamePlayerInfo>& items, int column, bool ascending, std::unordered_map<std::string, PlayerRankList> PlayerRankLists = {}) {
		auto compare = [&column, &ascending, &PlayerRankLists](const GamePlayerInfo& a, const GamePlayerInfo& b) {
			PlayerRankList temp, BPlayerRank;
			PlayerRankInfo APlayerRankData, BPlayerRankData;

			if (PlayerRankLists.count(a.PlayerName) > 0)
			{
				temp = PlayerRankLists[a.PlayerName];
			}

			if (PlayerRankLists.count(b.PlayerName) > 0)
			{
				BPlayerRank = PlayerRankLists[b.PlayerName];
			}

			if (GameData.Config.PlayerList.RankMode == 1)
			{
				APlayerRankData = temp.TPP;
				BPlayerRankData = temp.TPP;
			}

			if (GameData.Config.PlayerList.RankMode == 2)
			{
				APlayerRankData = temp.SquadTPP;
				BPlayerRankData = temp.SquadTPP;
			}

			if (GameData.Config.PlayerList.RankMode == 3)
			{
				APlayerRankData = temp.FPP;
				BPlayerRankData = temp.FPP;
			}

			if (GameData.Config.PlayerList.RankMode == 4)
			{
				APlayerRankData = temp.SquadFPP;
				BPlayerRankData = temp.SquadFPP;
			}

			switch (column) {
			case 0: return ascending ? a.TeamID < b.TeamID : a.TeamID > b.TeamID;
			case 1: return ascending ? a.PlayerName < b.PlayerName : a.PlayerName > b.PlayerName;
			case 2: return ascending ? APlayerRankData.Tier  < BPlayerRankData.Tier : APlayerRankData.Tier > BPlayerRankData.Tier;
			case 3: return ascending ? APlayerRankData.RankPoint < BPlayerRankData.RankPoint : APlayerRankData.RankPoint > BPlayerRankData.RankPoint;
			case 4: return ascending ? APlayerRankData.KDA < BPlayerRankData.KDA : APlayerRankData.KDA > BPlayerRankData.KDA;
			case 5: return ascending ? APlayerRankData.WinRatio < BPlayerRankData.WinRatio : APlayerRankData.WinRatio > BPlayerRankData.WinRatio;
			case 6: return ascending ? a.KillCount < b.KillCount : a.KillCount > b.KillCount;
			case 7: return ascending ? a.DamageDealtOnEnemy < b.DamageDealtOnEnemy : a.DamageDealtOnEnemy > b.DamageDealtOnEnemy;
			case 8: return ascending ? a.PubgIdData.SurvivalLevel < b.PubgIdData.SurvivalLevel : a.PubgIdData.SurvivalLevel > b.PubgIdData.SurvivalLevel;
			case 9: return ascending ? a.PartnerLevel < b.PartnerLevel : a.PartnerLevel > b.PartnerLevel;
			case 10: return ascending ? a.ListType < b.ListType : a.ListType > b.ListType;
			default: return false;
			}
			};
		std::sort(items.begin(), items.end(), compare);
	}

	inline void Render11()
	{

		ImGuiStyle* style = &ImGui::GetStyle();

		static float color[4] = { 217 / 255.f, 164 / 255.f, 88 / 255.f, 1.f };
		c::accent = { color[0], color[1], color[2], 1.f };
		style->WindowPadding = ImVec2(0, 0);
		style->ItemSpacing = ImVec2(15, 15);
		style->WindowBorderSize = 0;
		style->ScrollbarSize = 10.f;//滚动条宽度

		ImGui::SetNextWindowSize(ImVec2(c::bg::size2));
		using namespace ImGui;

		if (GameData.Config.Window.Players && ImGui::Begin("Menu111", &GameData.Config.Window.Players, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			const ImVec2& pos = ImGui::GetWindowPos();
			const ImVec2& region = ImGui::GetContentRegionMax();

			const ImVec2 size = ImGui::GetWindowSize();

			const ImVec2& spacing = style->ItemSpacing;
			menuRect1.left = pos.x;
			menuRect1.right = size.x + pos.x;
			menuRect1.top = pos.y;
			menuRect1.bottom = size.y + pos.y;
			{ // decoration
				
				GetBackgroundDrawList()->AddShadowRect(pos, pos + ImVec2(region), ImGui::GetColorU32(c::shadow), 80, ImVec2(0, 0), NULL, c::bg::rounding);
				GetBackgroundDrawList()->AddRectFilled(pos, pos + ImVec2(region), ImGui::GetColorU32(c::bg::background), c::bg::rounding);
		
				
			}
			ImGui::BeginChild(false, "Child", "o", ImVec2(1145, 565));
			{
				ImGui::BeginGroup();
				{
					ImGui::BeginChild(true, U8("玩家列表    (可设置热键快捷开关)"), "o", ImVec2(870, 547));
					{
						
						
						static const char* filterPlayerList[] = { U8("全部名单"), U8("黑名单"), U8("白名单") };
						static int filterPlayerListIndex = 0;


						if (ImGui::BeginCombo(U8("##名单类型"), filterPlayerList[filterPlayerListIndex], 0, true))
						{
							for (int i = 0; i < IM_ARRAYSIZE(filterPlayerList); i++) {
								bool isSelected = (filterPlayerListIndex == i);
								if (ImGui::Selectable(filterPlayerList[i], isSelected))
									filterPlayerListIndex = i;

								if (isSelected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
						//ImGui::SameLine(); 
						//ImGui::SetNextItemWidth(10);
						static const char* filterPlayerType[] = { U8("全部类型"), U8("玩家"), U8("合作者"), U8("队友"), U8("本人"), U8("人机") };
						static int filterPlayerTypeIndex = 0;
						if (ImGui::BeginCombo(U8("##玩家类型"), filterPlayerType[filterPlayerTypeIndex], 0))
						{
							for (int i = 0; i < IM_ARRAYSIZE(filterPlayerType); i++) {
								bool isSelected = (filterPlayerTypeIndex == i);
								if (ImGui::Selectable(filterPlayerType[i], isSelected))
									filterPlayerTypeIndex = i;

								if (isSelected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
						//ImGui::SameLine(); 
						static const char* items[] = { U8("不查战绩"), U8("TPP单人"), U8("TPP小队"), U8("FPP单人"), U8("FPP小队") };
						if (ImGui::BeginCombo(U8("##战绩数据"), items[GameData.Config.PlayerList.RankMode], 0)) {
							for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
								bool isSelected = (GameData.Config.PlayerList.RankMode == i);
								if (ImGui::Selectable(items[i], isSelected))
									GameData.Config.PlayerList.RankMode = i;

								if (isSelected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
						//ImGui::SameLine();
						//ImGui::SetNextItemWidth(100);
						//ImGui::InputText( U8("搜索玩家"), filterPlayerName, sizeof(filterPlayerName));
						static ImGuiTableFlags flags = ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
						std::unordered_map<std::string, GamePlayerInfo> PlayerLists;
						PlayerLists = Data::GetPlayerLists();
						bool NeedUpdatePlayerLists = false;
						std::vector<GamePlayerInfo> players = {};
						for (auto player : PlayerLists)
						{
							players.push_back(player.second);
						}
						if (ImGui::BeginTable(U8("玩家列表"), 11, flags, ImVec2(880, 440)))
						{
							ImGui::TableSetupScrollFreeze(0, 1);
							ImGui::TableSetupColumn(U8("  队伍"), ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultSort, 50.0f);
							ImGui::TableSetupColumn(U8("玩家昵称"), ImGuiTableColumnFlags_WidthFixed, 180.0f);
							ImGui::TableSetupColumn(U8("段位"), ImGuiTableColumnFlags_WidthFixed, 70.0f);
							ImGui::TableSetupColumn(U8("分数"), ImGuiTableColumnFlags_WidthFixed, 60.f);
							ImGui::TableSetupColumn(U8("KD"), ImGuiTableColumnFlags_WidthFixed, 60.0f);
							ImGui::TableSetupColumn(U8("吃鸡率"), ImGuiTableColumnFlags_WidthFixed, 60.0f);
							ImGui::TableSetupColumn(U8("击杀"), ImGuiTableColumnFlags_WidthFixed, 60.0f);
							ImGui::TableSetupColumn(U8("伤害"), ImGuiTableColumnFlags_WidthFixed, 60.0f);
							ImGui::TableSetupColumn(U8("等级"), ImGuiTableColumnFlags_WidthFixed, 60.0f);
							ImGui::TableSetupColumn(U8("类型"), ImGuiTableColumnFlags_WidthFixed, 60.0f);
							ImGui::TableSetupColumn(U8("名单"), ImGuiTableColumnFlags_WidthFixed, 80.0f);
							ImGui::TableHeadersRow();

							std::unordered_map<std::string, PlayerRankList> PlayerRankLists = Data::GetPlayerRankLists();

							if (ImGuiTableSortSpecs* SortsSpecs = ImGui::TableGetSortSpecs())
							{
								if (SortsSpecs->SpecsDirty)
								{
									SortItemsByGamePlayerInfo(players, SortsSpecs->Specs->ColumnIndex, SortsSpecs->Specs->SortDirection == ImGuiSortDirection_Ascending, PlayerRankLists);
								}
							}

							for (auto& player : players)
							{
								if (player.PlayerName == "" || player.PlayerName == "Bear" || player.PlayerName == "Bird") {
									continue;
								}
								if (player.StatusType == 8 || player.StatusType == 12) {
									auto PlayerName = player.PlayerName;
									if (player.ClanName != "") {
										PlayerName = "[" + player.ClanName + "] " + player.PlayerName;

									}
									PlayerRankInfo PlayerRankData;

									//PlayerRankList temp = Data::GetPlayerRankListsItem(player.PlayerName.c_str());
									PlayerRankList temp = Data::GetPlayerSegmentListsItem(player.PlayerName);
									// 根据配置的排名模式，选择相应的排名数据
									switch (GameData.Config.PlayerList.RankMode) {
									case 1:
										PlayerRankData = temp.TPP; // 第三人称排名
										break;
									case 2:
										//Utils::Log(1, "2222%s %d %f %s", player.PlayerName.c_str(), temp.SquadTPP.RankPoint, temp.SquadTPP.KDA, temp.SquadTPP.Tier);
										PlayerRankData = temp.SquadTPP; // 第三人称组队排名
										break;
									case 3:
										PlayerRankData = temp.FPP; // 第一人称排名
										break;
									case 4:
										PlayerRankData = temp.SquadFPP; // 第一人称组队排名
										break;
									default:
										break; // 如果没有匹配的模式，保持 PlayerRankData 为初始状态
									}

									std::string RankTier = PlayerRankData.TierToString;

									std::string RankKDA;
									//[+] 2222 NOMU_528 0 0.000000
									//Utils::Log(1,"%s %d ", player.PlayerName.c_str(), temp.SquadTPP.RankPoint);
									if (PlayerRankData.KDAToString.empty()) {

										if (PlayerRankData.KDA == 0.0)
										{
											RankKDA = "-";
										}
										else
										{
											std::ostringstream oss;
											oss << std::fixed << std::setprecision(2) << PlayerRankData.KDA;
											RankKDA = oss.str();
										}
									}
									else
									{
										RankKDA = PlayerRankData.KDAToString;
									}

									std::string RankPoint = std::to_string(static_cast<int>(PlayerRankData.RankPoint));
									if (PlayerRankData.RankPoint == 0)
									{
										RankPoint = "-";
									}
									else
									{
										RankPoint = std::to_string(static_cast<int>(PlayerRankData.RankPoint));
									}
									std::ostringstream oss;
									oss << std::fixed << std::setprecision(2) << PlayerRankData.WinRatio << "%";
									std::string WinRatio = oss.str();

									std::string PlayerType = U8("玩家"), PlayerListType = U8("默认");
									if (player.PartnerLevel > 0) {
										PlayerType = U8("合作者Lvl.") + std::to_string(player.PartnerLevel);
									}
									else if (player.IsSelf) {
										PlayerType = U8("本人");
									}
									else if (player.IsMyTeam) {
										PlayerType = U8("队友");
									}
									if (player.StatusType == 12) {
										PlayerType = U8("人机");
									}
									if (player.ListType == 1) {
										PlayerListType = U8("黑名单");
									}
									else if (player.ListType == 2) {
										PlayerListType = U8("白名单");
									}

									if ((!filterPlayerListIndex && !filterPlayerTypeIndex && !filterPlayerName[0]) || (filterPlayerListIndex > 0 && strstr(filterPlayerList[filterPlayerListIndex], PlayerListType.c_str())) || (filterPlayerTypeIndex > 0 && strstr(PlayerType.c_str(), filterPlayerType[filterPlayerTypeIndex])) || (filterPlayerName[0] && strstr(player.PlayerName.c_str(), filterPlayerName) != nullptr)) {

										ImGui::TableNextRow();
										std::string Space = "  ";
										ImGui::TableSetColumnIndex(0);
										ImGui::TextUnformatted((Space + std::to_string(player.TeamID)).c_str());
										ImGui::TableSetColumnIndex(1);
										ImGui::TextUnformatted(PlayerName.c_str());
										ImGui::TableSetColumnIndex(2);
										ImGui::TextUnformatted(RankTier.c_str());
										ImGui::TableSetColumnIndex(3);
										ImGui::TextUnformatted(RankPoint.c_str());
										ImGui::TableSetColumnIndex(4);
										ImGui::TextUnformatted(RankKDA.c_str());
										ImGui::TableSetColumnIndex(5);
										ImGui::TextUnformatted(WinRatio.c_str());
										ImGui::TableSetColumnIndex(6);
										ImGui::TextUnformatted(std::to_string(player.KillCount).c_str());
										ImGui::TableSetColumnIndex(7);
										ImGui::TextUnformatted(std::to_string((int)player.DamageDealtOnEnemy).c_str());
										ImGui::TableSetColumnIndex(8);
										ImGui::TextUnformatted(std::to_string(player.PubgIdData.SurvivalLevel).c_str());
										ImGui::TableSetColumnIndex(9);
										ImGui::TextUnformatted(PlayerType.c_str());
										if (ImGui::TableSetColumnIndex(10))
										{
											auto NeedPopStyleColor = false;
											if (player.ListType == 1)
											{
												ImGui::PushStyleColor(ImGuiCol_Button, Utils::FloatToImColor(GameData.Config.ESP.Color.Blacklist.Info));
												ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Utils::FloatToImColor(GameData.Config.ESP.Color.Blacklist.Info));
												ImGui::PushStyleColor(ImGuiCol_ButtonActive, Utils::FloatToImColor(GameData.Config.ESP.Color.Blacklist.Info));
												NeedPopStyleColor = true;
											}
											else if (player.ListType == 2)
											{
												ImGui::PushStyleColor(ImGuiCol_Button, Utils::FloatToImColor(GameData.Config.ESP.Color.Whitelist.Info));
												ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Utils::FloatToImColor(GameData.Config.ESP.Color.Whitelist.Info));
												ImGui::PushStyleColor(ImGuiCol_ButtonActive, Utils::FloatToImColor(GameData.Config.ESP.Color.Whitelist.Info));
												NeedPopStyleColor = true;
											}

											ImGui::PushID((int)player.pPlayerInfo);
											if (ImGui::SmallButton(PlayerListType.c_str())) {
												if (player.ListType >= 2) {
													player.ListType = 0;
												}
												else {
													player.ListType++;
												}
												if (player.ListType == 1) {

													Players::AddToBlackList(player.AccountId);
												}
												else if (player.ListType == 2) {
													Players::AddToWhiteList(player.AccountId);
												}
												else {
													Players::RemoveList(player.AccountId);
												}

												if (player.ListType != PlayerLists[player.PlayerName].ListType)
												{
													PlayerLists[player.PlayerName].ListType = player.ListType;
													Data::SetPlayerListsItem(player.PlayerName, PlayerLists[player.PlayerName]);
												}

											}

											if (NeedPopStyleColor)
											{
												ImGui::PopStyleColor(3);
											}

											ImGui::PopID();
										}
									}

								}

							}

							ImGui::EndTable();
						}

						ImGui::End();



						
						

					}
					ImGui::EndChild();
				}
				ImGui::EndGroup();
			}
			ImGui::EndChild();
				

		
				

		}

	}
}