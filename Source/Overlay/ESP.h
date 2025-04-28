#pragma once

#include <windows.h>
#include "common/Data.h"
#include "common/VectorHelper.h"
#include "RenderHelper.h"
#include <imgui/imgui.h>
#include <DMALibrary/Memory/Memory.h>
#include "Texture.h"
#include "HealthBar.h"
#include <Hack/Radar.h>
#include <Hack/LineTrace.h>
//#include <Hack/AimBot.h>

class ESP {
public:
	static void DrawFPS(ImGuiIO& io)
	{
		if (!GameData.Config.Overlay.ShowFPS)
		{
			return;
		}
		auto GameScene = GameData.Scene;
		std::string Scene = "Unknown";
		std::string Items = "隐藏";

		if (GameScene == Scene::FindProcess) {
			Scene = "寻找游戏进程";
		}
		else if (GameScene == Scene::Lobby) {
			Scene = "大厅中";
		}
		else if (GameScene == Scene::Gaming) {
			Scene = "游戏中";
		}

		if (GameData.Config.ESP.FocusMode)
		{
			Items = "战斗模式";
		}
		
		else {

			std::unordered_map<int, std::string> groupMap = {
				{1, "A"},
				{2, "B"},
				{3, "C"},
				{4, "D"}
			};
			Items = "";

			for (const auto& group : GameData.Config.Item.ShowGroups) {
				if (groupMap.find(group) != groupMap.end()) {
					if (!Items.empty()) {
						Items += " ";
					}
					Items += groupMap[group];
				}
			}

		}
		if (GameData.Config.ESP.DataSwitch)
		{
			auto TextPosition = ImVec2(5, 20);
			int spectatedCount = GameData.LocalPlayerInfo.SpectatedCount;
			int damageDealt = GameData.LocalPlayerInfo.DamageDealtOnEnemy;

			// 渲染其他信息
			RenderHelper::StrokeText(Utils::StringToUTF8(std::format("{} {}\nF2: 自瞄开关:{}\n战斗模式:{} \n分组显示: {}  \n自瞄配置: 配置{}  \n当前伤害: {} \n当前排名: {} \n盲区数量: {}",
				Scene,
				(int)io.Framerate,
				GameData.Config.AimBot.Enable ? "开" : "关",
				GameData.Config.ESP.FocusMode ? "开" : "关",
				Items,
				GameData.Config.AimBot.ConfigIndex + 1,
				damageDealt,
				GameData.NumAliveTeams,
				GameData.FogPlayerCount

			)).c_str(), TextPosition, IM_COL32(0, 255, 0, 255), 18, false, false);

			// 根据需要调整位置
			TextPosition.y += 145;

			// 根据观战数值决定颜色和字体大小
			ImU32 spectatedColor = (spectatedCount > 0) ? IM_COL32(255, 0, 0, 255) : IM_COL32(0, 255, 0, 255);
			int fontSize = 1 + (spectatedCount > 0 ? 21 : 0); // 大于0时字体大小增加21

			// 渲染“观战”文本，颜色和字体大小根据条件变化
			RenderHelper::StrokeText(Utils::StringToUTF8(std::format("观战数量: {}",
				spectatedCount
			)).c_str(), TextPosition, spectatedColor, fontSize, false, false);
		}
		
		if (GameData.Config.ESP.SpectatedWarning)
		{
			float posX = GameData.Config.Overlay.ScreenWidth / 2;
			//float posY = GameData.Config.Overlay.ScreenHeight - 150;
			float posY = 150;

			ImU32 textColor = (GameData.LocalPlayerInfo.SpectatedCount > 0) ? true : false;

			if (textColor) {
				RenderHelper::StrokeText(Utils::StringToUTF8(std::format(
					"请注意演技   当前观战数量 {}  ",
					GameData.LocalPlayerInfo.SpectatedCount
					//)).c_str(), ImVec2(posX, posY), IM_COL32(255, 0, 0, 255), 18, true, true);
				)).c_str(), ImVec2(posX, posY), IM_COL32(255, 0, 0, 255), 24, true, true);
			}
		}
		
		
	};

	/*static void DrawSkeleton(Player Player, const ImU32 SkeletonUseColor, const float Thickness)
	{
		FVector2D neckpos = Player.Skeleton.ScreenBones[EBoneIndex::Neck_01];
		FVector2D pelvispos = Player.Skeleton.ScreenBones[EBoneIndex::Pelvis];
		std::vector<std::pair<ImVec2, ImVec2>> Bones;
		bool failed = false;

		for (const auto& a : SkeletonLists::Skeleton)
		{
			FVector2D previous = FVector2D(0, 0);
			FVector2D current, p1, c1;

			for (EBoneIndex bone : a)
			{
				current = (bone == EBoneIndex::Neck_01) ? neckpos : (bone == EBoneIndex::Pelvis) ? pelvispos : Player.Skeleton.ScreenBones[bone];

				if (previous.X != 0.f && previous.Y != 0.f)
				{
					p1 = previous;
					c1 = current;

					Bones.emplace_back(std::make_pair(ImVec2(p1.X, p1.Y), ImVec2(c1.X, c1.Y)));

					if (p1.X == INFINITY || p1.Y == INFINITY || c1.X == INFINITY || c1.Y == INFINITY) {
						failed = true;
						Bones.clear();
						break;
					}
				}
				previous = current;
			}

			if (failed)
				break;
		}
		for (auto& line : Bones)
		{
			RenderHelper::Line(line.first, line.second, SkeletonUseColor, Thickness);
		}
	}*/

	static void DrawSkeleton(Player Player, const ImU32 SkeletonUseColor, const float Thickness)
	{
		FVector2D neckpos = Player.Skeleton.ScreenBones[EBoneIndex::Neck_01];
		FVector2D pelvispos = Player.Skeleton.ScreenBones[EBoneIndex::Pelvis];
		std::vector<std::tuple<ImVec2, ImVec2, ImU32>> Bones;
		bool failed = false;

		for (const auto& a : SkeletonLists::Skeleton)
		{
			FVector2D previous = FVector2D(0, 0);
			FVector2D current, p1, c1;


			for (EBoneIndex bone : a)
			{
				current = (bone == EBoneIndex::Neck_01) ? neckpos : (bone == EBoneIndex::Pelvis) ? pelvispos : Player.Skeleton.ScreenBones[bone];

				if (previous.X != 0.f && previous.Y != 0.f)
				{
					p1 = previous;
					c1 = current;

					auto visible = Player.Skeleton.VisibleBones[bone];
					ImU32 color = ImGui::ColorConvertFloat4ToU32(Utils::FloatToImColor(GameData.Config.ESP.Color.Visible.Skeleton));
					Bones.emplace_back(std::make_tuple(ImVec2(p1.X, p1.Y), ImVec2(c1.X, c1.Y), visible ? color : SkeletonUseColor));

					if (p1.X == INFINITY || p1.Y == INFINITY || c1.X == INFINITY || c1.Y == INFINITY) {
						failed = true;
						Bones.clear();
						break;
					}

				}
				previous = current;
			}

			if (failed)
				break;
		}

		for (auto& line : Bones)
		{
			RenderHelper::Line(std::get<0>(line), std::get<1>(line), std::get<2>(line), Thickness);
		}
	}
	
	static void DrawSkeleton_Physx(Player Player, const ImU32 SkeletonUseColor, const float Thickness)
	{
		FVector2D neckpos = Player.Skeleton.ScreenBones[EBoneIndex::Neck_01];
		FVector2D pelvispos = Player.Skeleton.ScreenBones[EBoneIndex::Pelvis];
		std::vector<std::tuple<ImVec2, ImVec2, ImU32>> Bones;
		bool failed = false;

		for (const auto& a : SkeletonLists::Skeleton)
		{
			FVector2D previous = FVector2D(0, 0);
			FVector2D current, p1, c1;


			for (EBoneIndex bone : a)
			{
				current = (bone == EBoneIndex::Neck_01) ? neckpos : (bone == EBoneIndex::Pelvis) ? pelvispos : Player.Skeleton.ScreenBones[bone];

				if (previous.X != 0.f && previous.Y != 0.f)
				{
					p1 = previous;
					c1 = current;

					auto visible = Player.Skeleton.VisibleBones[bone];
					ImU32 color = ImGui::ColorConvertFloat4ToU32(Utils::FloatToImColor(GameData.Config.ESP.Color.Visible.Skeleton));
					Bones.emplace_back(std::make_tuple(ImVec2(p1.X, p1.Y), ImVec2(c1.X, c1.Y), visible ? color : SkeletonUseColor));

					if (p1.X == INFINITY || p1.Y == INFINITY || c1.X == INFINITY || c1.Y == INFINITY) {
						failed = true;
						Bones.clear();
						break;
					}

				}
				previous = current;
			}

			if (failed)
				break;
		}

		for (auto& line : Bones)
		{
			RenderHelper::Line(std::get<0>(line), std::get<1>(line), std::get<2>(line), Thickness);
		}
	}
	static void DrawFOV()
	{
		AimBotConfig Config = GameData.Config.AimBot.Configs[GameData.Config.AimBot.ConfigIndex].Weapon[WeaponTypeToString[GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType]];

		if (Config.DynamicFov) {
			Config.FOV = Config.FOV * (90.0f / GameData.Camera.FOV);
			Config.WheelFOV = Config.WheelFOV * (90.0f / GameData.Camera.FOV);
		}

		ImVec2 CenterPoint = { (float)GameData.Config.Overlay.ScreenWidth / 2, (float)GameData.Config.Overlay.ScreenHeight / 2 };

		if (GameData.Config.AimBot.ShowFOV && !GameData.AimBot.Lock && GameData.AimBot.Type == EntityType::Player)
		{
			if (Config.FOV <= 0) return;
			RenderHelper::Circle(CenterPoint, Config.FOV, Utils::FloatToImColor(GameData.Config.AimBot.FOVColor), 2, 360);
		}

		if (GameData.Config.AimBot.ShowWheelFOV && !GameData.AimBot.Lock)
		{
			if (Config.WheelFOV <= 0) return;
			RenderHelper::Circle(CenterPoint, Config.WheelFOV, Utils::FloatToImColor(GameData.Config.AimBot.WheelFOVColor), 2, 360);
		}
	}
	static void DrawAimBotPoint()
	{
		if (GameData.Config.AimBot.ShowPoint && GameData.LocalPlayerInfo.IsScoping && !Utils::ValidPtr(GameData.AimBot.TargetPlayerInfo.Entity))
		{
			ImVec2 CenterPoint = { (float)GameData.Config.Overlay.ScreenWidth / 2, (float)GameData.Config.Overlay.ScreenHeight / 2 };
			if (GameData.AimBot.PredictedPos.X == 0 && GameData.AimBot.PredictedPos.Y == 0 && GameData.AimBot.PredictedPos.Z == 0) return;

			FVector2D PointPos = VectorHelper::WorldToScreen(GameData.AimBot.PredictedPos);

			ImVec2 ImPointPos = { PointPos.X, PointPos.Y };

			RenderHelper::CircleFilled(ImPointPos, GameData.Config.AimBot.PointSize, Utils::FloatToImColor(GameData.Config.AimBot.PointColor), GameData.Config.AimBot.PointSize * 2);
		}
	}
	static void DrawHealthBar(DWORD Sign, float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size, ImColor Color, bool Horizontal, bool ShowBackupHealth = true)
	{
		static std::map<DWORD, HealthBar> HealthBarMap;
		if (!HealthBarMap.count(Sign))
		{
			HealthBarMap.insert({ Sign,HealthBar() });
		}
		if (HealthBarMap.count(Sign))
		{
			if (Horizontal)
				HealthBarMap[Sign].DrawHealthBar_Horizontal(MaxHealth, CurrentHealth, Pos, Size, Color, ShowBackupHealth);
			else
				HealthBarMap[Sign].DrawHealthBar_Vertical(MaxHealth, CurrentHealth, Pos, Size, Color, ShowBackupHealth);
		}
	}

	static ImVec4 Get2DBox(const Player& Player)
	{
		FVector2D Head = Player.Skeleton.ScreenBones.at(EBoneIndex::Head);
		FVector2D Root = Player.Skeleton.ScreenBones.at(EBoneIndex::Root);

		ImVec2 Size, Pos;
		Size.y = (Root.Y - Head.Y) * 1.09;
		Size.x = Size.y * 0.6;

		Pos = ImVec2(Root.X - Size.x / 2, Head.Y - Size.y * 0.08);

		return ImVec4{ Pos.x,Pos.y,Size.x,Size.y };
	}
	static std::unordered_map<int, std::list<ItemInfo>> GroupItems(std::vector<std::pair<uint64_t, ItemInfo>> Items, float ThresholdX, float ThresholdY) {
		static const auto IsNear = [&](const FVector2D& A, const FVector2D& B) {
			return std::abs(A.X - B.X) < ThresholdX && std::abs(A.Y - B.Y) < ThresholdY;
			};

		std::unordered_map<int, std::list<ItemInfo>> Groups;
		int GroupId = 0;

		for (auto& Key : Items) {
			auto& Item = Key.second;

			bool FoundGroup = false;
			Item.ScreenLocation = VectorHelper::WorldToScreen(Item.Location);

			for (auto& Group : Groups) {
				for (const auto& GroupItem : Group.second) {
					if (IsNear(Item.ScreenLocation, GroupItem.ScreenLocation)) {
						Group.second.push_back(Item);
						FoundGroup = true;
						break;
					}
				}
				if (FoundGroup) break;
			}

			if (!FoundGroup) {
				Groups[GroupId++] = std::list<ItemInfo>{ Item };
			}
		}
		return Groups;
	}
	static void DrawItems(std::unordered_map<uint64_t, ItemInfo> Items)
	{
		// 检查物品绘制功能是否启用
		if (!GameData.Config.Item.Enable) return;

		// 将物品从 unordered_map 转换为 vector，以便排序
		std::vector<std::pair<uint64_t, ItemInfo>> Vectors(Items.begin(), Items.end());

		// 根据物品距离进行降序排序
		std::sort(Vectors.begin(), Vectors.end(), [](const std::pair<uint64_t, ItemInfo>& a, const std::pair<uint64_t, ItemInfo>& b) {
			return a.second.Distance > b.second.Distance;
			});

		ImColor FontSize = GameData.Config.Item.FontSize; // 获取字体大小
		ImColor TextColor = ImColor(255, 255, 255); // 默认文本颜色为白色

		// 如果组合显示启用
		if (GameData.Config.Item.Combination)
		{
			// 根据距离分组物品
			std::unordered_map<int, std::list<ItemInfo>> CombinationItems = GroupItems(Vectors, GameData.Config.Item.ThresholdX, GameData.Config.Item.ThresholdY);
			for (auto& Group : CombinationItems)
			{
				int Index = 0; // 当前组内索引
				int FistIndex = 0; // 第一个物品的索引
				FVector2D FistPos; // 第一个物品的位置

				// 按物品类型排序
				Group.second.sort([](const ItemInfo& A, const ItemInfo& B) {
					return A.ItemType > B.ItemType;
					});

				for (ItemInfo& ItemInfo : Group.second)
				{
					// 检查物品是否隐藏或超出最大距离
					if (ItemInfo.bHidden || ItemInfo.Distance > GameData.Config.Item.DistanceMax) continue;

					std::string Text = Utils::StringToUTF8(std::format("{}", ItemInfo.DisplayName));
					int GroupIndex = GameData.Config.Item.Lists[ItemInfo.Name].Group;

					// 检查是否显示该组
					if (GameData.Config.Item.ShowGroups.find(GroupIndex) == GameData.Config.Item.ShowGroups.end()) continue;

					// 根据组 ID 设置文本颜色
					switch (GroupIndex)
					{
					case 1:
						TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupAColor);
						break;
					case 2:
						TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupBColor);
						break;
					case 3:
						TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupCColor);
						break;
					case 4:
						TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupDColor);
						break;
					default:
						continue;
					}

					// 将物品位置转换为屏幕坐标
					ItemInfo.ScreenLocation = VectorHelper::WorldToScreen(ItemInfo.Location);
					// 检查物品是否在屏幕内
					if (!VectorHelper::IsInScreen(ItemInfo.ScreenLocation)) continue;

					ItemInfo.Distance = GameData.Camera.Location.Distance(ItemInfo.Location) / 100.0f; // 更新距离

					// 记录第一个物品的位置
					if (FistPos.X == 0 && FistPos.Y == 0)
					{
						FistIndex = Index;
						FistPos = ItemInfo.ScreenLocation;
					}

					// 如果显示距离，并且是第一个物品
					if (GameData.Config.Item.ShowDistance && Index == FistIndex)
					{
						if (Group.second.size() > 1)
						{
							Text += "\n";
						}

						if (Group.second.size() == 1) {
							Text += " ";
						}

						Text += "[" + std::to_string((int)ItemInfo.Distance) + "M]"; // 添加距离信息
					}

					ImVec2 ScreenLocation = { FistPos.X, FistPos.Y - ((FontSize + 1) * Index) }; // 计算绘制位置

					// 绘制文本或图标
					if (!GameData.Config.Item.ShowIcon)
					{
						RenderHelper::StrokeText(Text.c_str(), { ScreenLocation.x, ScreenLocation.y }, TextColor, FontSize, false, false);
					}
					else
					{
						float Scale = (FontSize / 14.f);
						std::string IconUrl = "Assets/image/All/" + ItemInfo.Name + ".png"; // 图标路径
						if (GImGuiTextureMap[IconUrl].Width > 0) {
							float TargetHeight = 14.f * Scale;
							float HeightZoom = TargetHeight / GImGuiTextureMap[IconUrl].Height;
							float IconWidth = GImGuiTextureMap[IconUrl].Width * HeightZoom + 5.0f;
							float IconHeight = TargetHeight + 5.0f;

							ScreenLocation.x -= (FontSize + 4); // 调整位置

							RenderHelper::Image(GImGuiTextureMap[IconUrl].Texture, ScreenLocation, ImVec2(IconWidth, IconHeight), false); // 绘制图标
						}
					}

					Index++;
				}
			}
		}
		else {
			// 如果不启用组合显示
			for (auto& Item : Vectors)
			{
				ItemInfo& ItemInfo = Item.second;

				if (ItemInfo.bHidden || ItemInfo.Distance > GameData.Config.Item.DistanceMax) continue;

				int GroupIndex = GameData.Config.Item.Lists[ItemInfo.Name].Group;
				if (GameData.Config.Item.ShowGroups.find(GroupIndex) == GameData.Config.Item.ShowGroups.end()) continue;

				// 根据组 ID 设置文本颜色
				switch (GroupIndex)
				{
				case 1:
					TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupAColor);
					break;
				case 2:
					TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupBColor);
					break;
				case 3:
					TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupCColor);
					break;
				case 4:
					TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupDColor);
					break;
				default:
					continue;
				}

				// 将物品位置转换为屏幕坐标
				ItemInfo.ScreenLocation = VectorHelper::WorldToScreen(ItemInfo.Location);
				if (!VectorHelper::IsInScreen(ItemInfo.ScreenLocation)) continue;

				ItemInfo.Distance = GameData.Camera.Location.Distance(ItemInfo.Location) / 100.0f; // 更新距离

				std::string Text = Utils::StringToUTF8(std::format("{}", ItemInfo.DisplayName));
				ImColor InfoColor = ImColor(255, 255, 255); // 默认信息颜色

				// 绘制文本或距离
				ImVec2 InfoSize = RenderHelper::StrokeText(Text.c_str(), { ItemInfo.ScreenLocation.X, ItemInfo.ScreenLocation.Y }, TextColor, FontSize, true, true);

				if (GameData.Config.Item.ShowDistance) RenderHelper::StrokeText(Utils::StringToUTF8(std::format("{}M", (int)ItemInfo.Distance)).c_str(), { ItemInfo.ScreenLocation.X, ItemInfo.ScreenLocation.Y }, ImColor(255, 255, 255, 255), FontSize - 1, true, false);

				if (GameData.Config.Item.ShowIcon) {
					float Scale = 1.f;
					if (ItemInfo.ItemType == WeaponType::AR || ItemInfo.ItemType == WeaponType::DMR || ItemInfo.ItemType == WeaponType::SG || ItemInfo.ItemType == WeaponType::SR || ItemInfo.ItemType == WeaponType::LMG)
					{
						Scale = 1.4f;
					}

					std::string IconUrl = "Assets/image/All/" + ItemInfo.Name + ".png";
					if (GImGuiTextureMap[IconUrl].Width > 0) {
						{
							float TargetHeight = 24.f * (FontSize / 14.f) * Scale;
							float HeightZoom = TargetHeight / GImGuiTextureMap[IconUrl].Height;
							float IconWidth = GImGuiTextureMap[IconUrl].Width * HeightZoom + 15.0f;//图标大小
							float IconHeight = TargetHeight + 15.0f;//图标大小
							ItemInfo.ScreenLocation.X -= IconWidth / 2;
							ItemInfo.ScreenLocation.Y -= (IconHeight + FontSize + 2);

							RenderHelper::Image(GImGuiTextureMap[IconUrl].Texture, { ItemInfo.ScreenLocation.X, ItemInfo.ScreenLocation.Y }, ImVec2(IconWidth, IconHeight), false);
						}
					}
				}
			}
		}
	}
	static void DrawProjects(std::unordered_map<uint64_t, ProjectInfo> Items)
	{
		// 检查项目绘制功能是否启用
		if (!GameData.Config.Project.Enable) return;

		// 将项目从 unordered_map 转换为 vector，以便排序
		std::vector<std::pair<uint64_t, ProjectInfo>> Vectors(Items.begin(), Items.end());

		// 根据项目距离进行降序排序
		std::sort(Vectors.begin(), Vectors.end(), [](const std::pair<uint64_t, ProjectInfo>& a, const std::pair<uint64_t, ProjectInfo>& b) {
			return a.second.Distance > b.second.Distance;
			});

		// 遍历每个项目
		for (auto& Item : Vectors)
		{
			ProjectInfo& Project = Item.second;

			// 检查项目是否可见、爆炸时间是否小于等于0，以及是否超出最大距离
			if (Project.bVisible == 1 || Project.TimeTillExplosion <= 0 || Project.Distance > GameData.Config.Project.DistanceMax) continue;

			// 将项目位置转换为屏幕坐标
			Project.ScreenLocation = VectorHelper::WorldToScreen(Project.Location);
			// 更新项目距离
			Project.Distance = GameData.Camera.Location.Distance(Project.Location) / 100.0f;

			const int FontSize = GameData.Config.Project.FontSize; // 获取字体大小
			const float Scale = (float)FontSize / 14.f; // 根据基准大小计算缩放比例

			// 格式化文本，包括项目名称和距离信息
			std::string Text = Utils::StringToUTF8(std::format("{} [{}M]", Project.Name, (int)Project.Distance));
			ImColor InfoColor = Utils::FloatToImColor(GameData.Config.Project.Color); // 获取信息颜色
			Project.ScreenLocation.Y += 5 * Scale; // 调整屏幕位置

			// 绘制项目文本
			ImVec2 InfoSize = RenderHelper::StrokeText(Text.c_str(), { Project.ScreenLocation.X, Project.ScreenLocation.Y }, InfoColor, FontSize, true, false);
			
			// 如果项目是手榴弹，绘制健康条
			if (Project.EntityName == "ProjGrenade_C")
			{
				ImVec2 HealthBarPos, HealthBarSize;
				HealthBarSize = { 26 * Scale, 5 * Scale }; // 健康条的大小
				HealthBarPos = { Project.ScreenLocation.X - (HealthBarSize.x / 2), Project.ScreenLocation.Y + (InfoSize.y + 3 * Scale) }; // 计算健康条位置
				// 绘制健康条，当前健康为剩余爆炸时间的百分比
				DrawHealthBar(Project.Entity, 100.f, Project.TimeTillExplosion / 4.0f * 100.0f, HealthBarPos, HealthBarSize, InfoColor, true, false);
				if (GameData.Config.Project.FOV) {
					//爆炸范围
					Project.ExplosionRadius = 1000.f;
					RenderHelper::DrawExplosiveRange(Project.Location, Project.ExplosionRadius, InfoColor, 3);
				}
			}
			else if (Project.EntityName == "ProjC4_C") {
				Project.ExplosionRadius = 2200.f;
				if ((int)Project.Distance > 25) {
					InfoColor = Utils::FloatToImColor(GameData.Config.Project.Colorgreen);
					RenderHelper::DrawExplosiveRange(Project.Location, Project.ExplosionRadius, InfoColor, 3);
				}
			}

			
		}
	}

	static void DrawLocalPlayerProject()
	{
		if (GameData.Config.Project.ShowChareTime)
		{
			if (!GameData.Config.Project.BarShowChareTime && !GameData.Config.Project.TextShowChareTime) return;

			if (GameData.LocalPlayerInfo.ElapsedCookingTime > 0 && (int)GameData.LocalPlayerInfo.WeaponClassByte == 10)
			{
				static bool flashTriggered = false;
				float totalTime = 5.05f;
				if (GameData.LocalPlayerInfo.WeaponName == "闪光弹")
				{
					totalTime /= 2.f;
				}
				float progressValue = GameData.LocalPlayerInfo.ElapsedCookingTime / totalTime;

				if (progressValue >= 0.75f && !flashTriggered) {
					flashTriggered = true;
					progressValue = 0.f;
				}
				else if (flashTriggered) {
					flashTriggered = false;
				}

				ImColor Color = Utils::FloatToImColor(GameData.Config.Project.ChareColor);

				RenderHelper::DrawDashboardProgress(
					ImVec2(GameData.Config.Overlay.ScreenWidth / 2, GameData.Config.Overlay.ScreenHeight / 2),
					120.f * (GameData.Config.Project.ChareFontSize / 14.f),
					2,
					20,
					8,
					progressValue,
					Color
				);
			}
		}
	}
	static void DrawExplosiveRange(FVector center, float radius, ImColor color, float thickness) {

		FVector Vertices[46];
		bool VerticesValid[46]{};
		float angle = 0;
		const float angleStep = (M_PI * 2) / 45;

		for (int i = 0; i <= 45; ++i, angle += angleStep)
		{
			FVector pos = FVector(radius * cosf(angle) + center.X, radius * sinf(angle) + center.Y, center.Z);
			FVector2D w2sPos = VectorHelper::WorldToScreen(pos);
			VerticesValid[i] = w2sPos.X > 0 && w2sPos.X < GameData.Config.Overlay.ScreenWidth && w2sPos.Y > 0 && w2sPos.Y < GameData.Config.Overlay.ScreenHeight;
			Vertices[i] = FVector(w2sPos.X, w2sPos.Y, 0);
		}

		for (int i = 0; i < 45; ++i) {
			if (Vertices[i].Distance(Vertices[i + 1]) < radius)
				if ((VerticesValid[i] && VerticesValid[i + 1]))
					ImGui::GetWindowDrawList()->AddLine({ Vertices[i].X, Vertices[i].Y }, { Vertices[i + 1].X,  Vertices[i + 1].Y }, color, thickness);
		}
	}
	static void DrawVehicles(std::unordered_map<uint64_t, VehicleInfo> Vehicles)
	{
		
		FVector2D FistPos; //
		// 检查车辆绘制功能是否启用
		if (!GameData.Config.Vehicle.Enable) return;

		// 将车辆信息从 unordered_map 转换为 vector 以便排序
		std::vector<std::pair<uint64_t, VehicleInfo>> Vectors(Vehicles.begin(), Vehicles.end());

		// 根据车辆距离进行降序排序
		std::sort(Vectors.begin(), Vectors.end(), [](const std::pair<uint64_t, VehicleInfo>& a, const std::pair<uint64_t, VehicleInfo>& b) {
			return a.second.Distance > b.second.Distance;
			});

		// 遍历每辆车辆
		for (auto& Item : Vectors)
		{
			VehicleInfo& Vehicle = Item.second;

			// 检查车辆距离是否超过最大距离限制
			if (Vehicle.Distance > GameData.Config.Vehicle.DistanceMax) continue;

			// 将车辆位置转换为屏幕坐标
			Vehicle.ScreenLocation = VectorHelper::WorldToScreen(Vehicle.Location);
			// 更新车辆与玩家的距离
			Vehicle.Distance = GameData.Camera.Location.Distance(Vehicle.Location) / 100.0f;
			FistPos = Vehicle.ScreenLocation;
			// 格式化文本，包括车辆名称和距离信息
			std::string Text = Utils::StringToUTF8(std::format("{}\n{}M", Vehicle.Name, (int)Vehicle.Distance));
			ImColor InfoColor = Utils::FloatToImColor(GameData.Config.Vehicle.Color); // 获取车辆信息颜色
			const int FontSize = GameData.Config.Vehicle.FontSize; // 获取字体大小
			ImVec2 ScreenLocation = { FistPos.X, FistPos.Y - (FontSize + 1)}; // 计算绘制位置
			// 绘制车辆信息文本
			ImVec2 HeadInfoSize = RenderHelper::StrokeText(Text.c_str(), { Vehicle.ScreenLocation.X, Vehicle.ScreenLocation.Y }, 
				InfoColor, GameData.Config.Vehicle.FontSize, true, true);

			float Scale = (FontSize / 14.f);
			std::string IconUrl = "Assets/image/Vehicle/" + Vehicle.Name + ".png"; // 图标路径
			if (GImGuiTextureMap[IconUrl].Width > 0) {
				float TargetHeight = 14.f * Scale;
				float HeightZoom = TargetHeight / GImGuiTextureMap[IconUrl].Height;
				float IconWidth = GImGuiTextureMap[IconUrl].Width * HeightZoom + 5.0f;
				float IconHeight = TargetHeight + 5.0f;

				ScreenLocation.x -= (FontSize + 4); // 调整位置

				RenderHelper::Image(GImGuiTextureMap[IconUrl].Texture, ScreenLocation, ImVec2(IconWidth, IconHeight), false); // 绘制图标
			}





			//计算血量和油量百分比
			float HealthPercent = Vehicle.VehicleHealth / Vehicle.VehicleHealthMax;
			float FuelPercent = Vehicle.VehicleFuel / Vehicle.VehicleFuelMax;
			if (GameData.Config.Vehicle.Health)
			{
				//设置血量条最大宽度
				float HealthMaxWidth = 41.0f; 
				float HealthWidth = HealthMaxWidth * HealthPercent;
				ImColor Fuelbarcolor = Utils::FloatToImColor(GameData.Config.Vehicle.Fuelbarcolor);
				ImVec2 HealthBarPos = { Vehicle.ScreenLocation.X - HealthWidth / 2,Vehicle.ScreenLocation.Y + HeadInfoSize.y - 20 };
				//绘制血量
				RenderHelper::window_filled_rect(HealthBarPos, ImVec2(HealthWidth, 4), Fuelbarcolor);
				RenderHelper::window_box(HealthBarPos - ImVec2(1, 1), ImVec2(HealthMaxWidth + 2, 6), IM_COL32(28, 28, 28, 255));
				//显示血量数值
				char healthText[16];
				snprintf(healthText, sizeof(healthText), U8("血量:%.0f%"), HealthPercent * 100.0f);//格式化“50%”
				ImVec2 healthTextPos = { HealthBarPos.x + HealthMaxWidth + 5 ,HealthBarPos.y - 5 };//位置调整到油量右侧
				RenderHelper::window_text(healthTextPos, 15.0f, Fuelbarcolor, healthText);//绘制数值
			}
			if (GameData.Config.Vehicle.Durability)
			{
				float FuelMaxWidth = 40.0f; //设置油条最大宽度
				float FuelWidth = FuelMaxWidth * FuelPercent;
				ImColor Healthbarcolor = Utils::FloatToImColor(GameData.Config.Vehicle.Healthbarcolor);
				ImVec2 FuelBarPos = { Vehicle.ScreenLocation.X - FuelMaxWidth / 2,Vehicle.ScreenLocation.Y + HeadInfoSize.y - 26 };
				//绘制油量条
				RenderHelper::window_filled_rect(FuelBarPos, ImVec2(FuelWidth, 4), Healthbarcolor);
				RenderHelper::window_box(FuelBarPos - ImVec2(1, 1), ImVec2(FuelMaxWidth + 2, 6), IM_COL32(28, 28, 28, 255));
				//显示血量数值
				char fuelText[16];
				snprintf(fuelText, sizeof(fuelText), U8("油量:%.0f%"), FuelPercent * 100.0f);//格式化“50%”
				ImVec2 fuelTextPos = { FuelBarPos.x + FuelMaxWidth + 5 ,FuelBarPos.y - 10 };//位置调整到油量右侧y-10  xy调整
					
				RenderHelper::window_text(fuelTextPos, 15.0f, Healthbarcolor, fuelText);//绘制数值  15.0f字体大小

			}
		}
	}
	static void DrawVehicleWheels(std::unordered_map<uint64_t, VehicleWheelInfo> VehicleWheels)
	{
		// 将 VehicleWheels 中的键值对复制到一个向量中
		std::vector<std::pair<uint64_t, VehicleWheelInfo>> Vectors(VehicleWheels.begin(), VehicleWheels.end());

		// 按照 Wheel 的距离进行排序，距离越远的排在前面
		std::sort(Vectors.begin(), Vectors.end(), [](const std::pair<uint64_t, VehicleWheelInfo>& a, const std::pair<uint64_t, VehicleWheelInfo>& b) {
			return a.second.Distance > b.second.Distance; // 使用大于号进行降序排序
			});

		// 遍历排序后的向量
		for (auto& Item : Vectors)
		{
			VehicleWheelInfo& Wheel = Item.second; // 获取 Wheel 信息

			// 将世界坐标转换为屏幕坐标
			Wheel.ScreenLocation = VectorHelper::WorldToScreen(Wheel.Location);

			// 计算 Wheel 与屏幕中心的距离
			float Distance = Utils::CalculateDistance(GameData.Config.Overlay.ScreenWidth / 2, GameData.Config.Overlay.ScreenHeight / 2, Wheel.ScreenLocation.X, Wheel.ScreenLocation.Y);

			// 格式化文本，包含距离和阻尼率
			std::string Text = std::format("{}M {}", (int)Distance, Wheel.DampingRate);
			ImColor InfoColor = ImColor(255, 255, 255); // 设置信息颜色为白色

			// 绘制文本，显示在屏幕上
			ImVec2 HeadInfoSize = RenderHelper::StrokeText(Text.c_str(), { Wheel.ScreenLocation.X, Wheel.ScreenLocation.Y }, InfoColor, 14, true, true);
		}
	}
	static PlayerRankInfo GetPlayerRankInfo(const std::unordered_map<std::string, PlayerRankList> PlayerRankLists, const Player Player)
	{
		PlayerRankInfo PlayerRankData; // 声明一个变量用于存储玩家排名信息

		// 检查玩家排名列表中是否包含该玩家的名称
		if (PlayerRankLists.count(Player.Name) > 0)
		{
			PlayerRankList PlayerRank = PlayerRankLists.at(Player.Name); // 获取该玩家的排名信息

			// 根据配置的排名模式，选择相应的排名数据
			switch (GameData.Config.PlayerList.RankMode) {
			case 1:
				PlayerRankData = PlayerRank.TPP; // 第三人称排名
				break;
			case 2:
				PlayerRankData = PlayerRank.SquadTPP; // 第三人称组队排名
				break;
			case 3:
				PlayerRankData = PlayerRank.FPP; // 第一人称排名
				break;
			case 4:
				PlayerRankData = PlayerRank.SquadFPP; // 第一人称组队排名
				break;
			default:
				break; // 如果没有匹配的模式，保持 PlayerRankData 为初始状态
			}
		}
		return PlayerRankData; // 返回玩家的排名信息
	}
	static void DrawRadars(std::unordered_map<uint64_t, Player> Players, std::unordered_map<uint64_t, VehicleInfo> Vehicles, std::unordered_map<uint64_t, PackageInfo> Packages)
	{
		for (auto& Item : Vehicles)
		{
			VehicleInfo& Vehicle = Item.second;

			std::string IconUrl = "Assets/image/Map/car_land.png";

			if (GameData.Radar.Visibility && GameData.Config.Radar.Main.ShowVehicle)
			{
				FVector2D RadarLocation = Radar::WorldToRadarLocation(Vehicle.Location);

				if (GImGuiTextureMap[IconUrl].Width > 0) {
					float IconSize = GImGuiTextureMap[IconUrl].Width * (0.25f * (GameData.Config.Radar.Main.FontSize / 14.f));
					RenderHelper::AddImageRotated(GImGuiTextureMap[IconUrl].Texture, { RadarLocation.X, RadarLocation.Y }, ImVec2(IconSize, IconSize), 0);
				}
			}

			if (GameData.Radar.MiniRadarVisibility && Vehicle.Distance < GameData.Radar.MiniRadarDistance && GameData.Config.Radar.Mini.ShowVehicle)
			{
				FVector2D MiniRadarLocation = Radar::WorldToMiniRadarLocation(Vehicle.Location);

				if (GImGuiTextureMap[IconUrl].Width > 0) {
					float IconSize = GImGuiTextureMap[IconUrl].Width * (0.20f * (GameData.Config.Radar.Mini.FontSize / 14.f));
					RenderHelper::AddImageRotated(GImGuiTextureMap[IconUrl].Texture, { MiniRadarLocation.X, MiniRadarLocation.Y }, ImVec2(IconSize, IconSize), 0);
				}
				//RenderHelper::StrokeText(Vehicle.Name.c_str(), { MiniRadarLocation.X, MiniRadarLocation.Y }, ImColor(Utils::FloatToImColor(GameData.Config.Radar.Mini.MapColor)), 10 * (GameData.Config.Radar.Mini.FontSize / 14.f));
			}
		}

		for (auto& Item : Packages)
		{
			PackageInfo& Package = Item.second;

			if (Package.Type == EntityType::AirDrop)
			{
				std::string IconUrl = "Assets/image/Map/Carapackage_RedBox_C.png";

				if (GameData.Radar.Visibility && GameData.Config.Radar.Main.ShowAirDrop)
				{
					FVector2D RadarLocation = Radar::WorldToRadarLocation(Package.Location);

					if (GImGuiTextureMap[IconUrl].Width > 0) {
						float IconSize = (0.25f * (GameData.Config.Radar.Main.FontSize / 14.f));//空投大小
						float IconWidth = GImGuiTextureMap[IconUrl].Width * IconSize;
						float IconHeight = GImGuiTextureMap[IconUrl].Height * IconSize;
						RenderHelper::AddImageRotated(GImGuiTextureMap[IconUrl].Texture, { RadarLocation.X, RadarLocation.Y }, ImVec2(IconWidth, IconHeight), 0);
					}
				
				}

				if (GameData.Radar.MiniRadarVisibility && Package.Distance < GameData.Radar.MiniRadarDistance && GameData.Config.Radar.Mini.ShowAirDrop)
				{
					FVector2D MiniRadarLocation = Radar::WorldToMiniRadarLocation(Package.Location);

					if (GImGuiTextureMap[IconUrl].Width > 0) {
						float IconSize = (0.1f * (GameData.Config.Radar.Main.FontSize / 14.f));//空投大小
						float IconWidth = GImGuiTextureMap[IconUrl].Width * IconSize;
						float IconHeight = GImGuiTextureMap[IconUrl].Height * IconSize;
						RenderHelper::AddImageRotated(GImGuiTextureMap[IconUrl].Texture, { MiniRadarLocation.X, MiniRadarLocation.Y }, ImVec2(IconWidth, IconHeight), 0);
					}
				
				}
			}
			else {////地图死亡
				std::string IconUrl = "Assets/image/Map/dead.png";

				if (GameData.Radar.Visibility && GameData.Config.Radar.Main.ShowDeadBox)
				{
					FVector2D RadarLocation = Radar::WorldToRadarLocation(Package.Location);

					if (GImGuiTextureMap[IconUrl].Width > 0) {
						float IconSize = GImGuiTextureMap[IconUrl].Width * (0.45f * (GameData.Config.Radar.Main.FontSize / 14.f));
						RenderHelper::AddImageRotated(GImGuiTextureMap[IconUrl].Texture, { RadarLocation.X, RadarLocation.Y }, ImVec2(IconSize, IconSize), 0);
					}
				}

				if (GameData.Radar.MiniRadarVisibility && Package.Distance < GameData.Radar.MiniRadarDistance && GameData.Config.Radar.Mini.ShowDeadBox)
				{
					FVector2D MiniRadarLocation = Radar::WorldToMiniRadarLocation(Package.Location);

					if (GImGuiTextureMap[IconUrl].Width > 0) {
						float IconSize = GImGuiTextureMap[IconUrl].Width * (0.40f * (GameData.Config.Radar.Mini.FontSize / 14.f));
						RenderHelper::AddImageRotated(GImGuiTextureMap[IconUrl].Texture, { MiniRadarLocation.X, MiniRadarLocation.Y }, ImVec2(IconSize, IconSize), 0);
					}
				}
			}
		}

		for (auto& Item : Players)
		{
			Player& Player = Item.second;

			if (Player.InFog || Player.IsMe || Player.IsMyTeam || (Player.State == CharacterState::Dead))
			{
				continue;
			}
			if (GameData.Config.Project.GrenadePrediction) {
				float health = Player.Health;
				//根据血量设置颜色
				ImColor HealthColor;
				if (health > 75.0f) {

					//血量大于75 绿色
					HealthColor = ImColor(0, 255, 0, 255);//绿色
				}
				else if (health > 50.0f) {

					HealthColor = ImColor(255, 255, 0, 255);//黄色

				}
				else if (health > 25.0f) {

					HealthColor = ImColor(255, 165, 0, 255);//橙色

				}
				else {

					HealthColor = ImColor(255, 0, 0, 255);//红色

				}
				//现有的投掷预测逻辑
				float ProjectHeight;
				float Distance = GameData.Camera.Location.Distance(Player.Location) / 100.0f;
				if (Distance > 16.0f && Distance < 18.0f)
				{
					ProjectHeight = 3970;

				}
				else if (Distance > 18.0f && Distance < 22.0f) {

					ProjectHeight = 4103;

				}
				else if (Distance > 22.0f && Distance < 28.0f) {

					ProjectHeight = 4328;
				}
				else if (Distance > 28.0f && Distance < 50.0f) {

					ProjectHeight = 4397;

				}
				else if (Distance > 50.0f && Distance < 55.0f) {

					ProjectHeight = 3809;

				}
				//根据玩家距离和投掷预测高度绘制圆圈
				if (Distance > 16.0f && Distance < 55.0f)
				{

					RenderHelper::DrawExplosiveRange(
						{ Player.Location.X, Player.Location.Y, Player.Location.Z + ProjectHeight },
						200, HealthColor, 3
					);

				}
			
			}
			//获取玩家血量;
			
		
			RenderHelper::PlayerColor PlayerColors = RenderHelper::GetPlayerColor(Player);

			FVector AimDirection = FRotator(0.0f, Player.AimOffsets.Yaw, 0.0f).GetUnitVector();
			FVector2D Direction = FVector2D{ AimDirection.X, AimDirection.Y };
			float AngleRadians = atan2(Direction.Y, Direction.X);
			float AngleDegrees = AngleRadians;

			if (GameData.Radar.Visibility && GameData.Config.Radar.Main.ShowPlayer)
			{
				FVector2D RadarLocation = Radar::WorldToRadarLocation2(Player.Location);

				//RenderHelper::Line({ RadarLocation.X, RadarLocation.Y }, { Direction.X, Direction.Y }, ImColor(255, 255, 255, 255), 1.f);
				RenderHelper::DrawRadarPlayerCircleWithText(Player.Type == EntityType::AI ? "BOT" : std::to_string(Player.TeamID).c_str(), PlayerColors.teamNumberColor, 10 * (GameData.Config.Radar.Main.FontSize / 14.f), { RadarLocation.X, RadarLocation.Y }, AngleDegrees, Player.RadarState);
			}

			if (GameData.Radar.MiniRadarVisibility&& Player.Distance < GameData.Radar.MiniRadarDistance&& GameData.Config.Radar.Mini.ShowPlayer)
			{
				FVector2D MiniRadarLocation = Radar::WorldToMiniRadarLocation(Player.Location);
				RenderHelper::DrawRadarPlayerCircleWithText(Player.Type == EntityType::AI ? "BOT" : std::to_string(Player.TeamID).c_str(), PlayerColors.teamNumberColor, 10 * (GameData.Config.Radar.Mini.FontSize / 14.f), { MiniRadarLocation.X, MiniRadarLocation.Y }, AngleDegrees, Player.RadarState);
			}
		}
	}

	static void DrawPlayers(std::unordered_map<uint64_t, Player> Players)
	{
		if (!GameData.Config.ESP.Enable) return;

		std::vector<std::pair<uint64_t, Player>> PlayersVector(Players.begin(), Players.end());

		std::sort(PlayersVector.begin(), PlayersVector.end(), [](const std::pair<uint64_t, Player>& a, const std::pair<uint64_t, Player>& b) {
			return a.second.Distance > b.second.Distance;
			});

		std::unordered_map<std::string, PlayerRankList> PlayerRankLists = Data::GetPlayerRankLists();
		int M200PlayerCount = 0;
		int PalyerBB = 500;//最近敌人
		std::string DisNane = "";

		for (auto& Item : PlayersVector)
		{
			Player& Player = Item.second;

			
			if (Player.IsMe || Player.IsMyTeam || (Player.State == CharacterState::Dead) || (Player.TeamID == 0))
			{
				continue;
			}
			
			Player.Distance = GameData.Camera.Location.Distance(Player.Location) / 100.0f;

			if (Player.Distance > GameData.Config.ESP.DistanceMax)
			{
				continue;
			}
			if (Player.Distance <= 200)
			{
				M200PlayerCount++;
				if (PalyerBB > Player.Distance)
				{
					PalyerBB = Player.Distance;
					DisNane = Player.Name;
				}

			}

			//是否可以显示信息
			bool bShowInfo = (Player.Distance <= GameData.Config.ESP.InfoDistanceMax) ||
				(GameData.Config.ESP.AimExpandInfo && GameData.AimBot.TargetPlayerInfo.Entity == Player.Entity);

			bool bShowWeapon = ((Player.Distance <= GameData.Config.ESP.WeaponDistanceMax && GameData.Config.ESP.Weapon) || bShowInfo) &&
				(Player.WeaponName != "fail" && Player.WeaponName != "Unknown") &&
				(Player.WeaponID > 0);


		
			// 预先缓存骨骼列表长度，避免重复获取
			
			
			const auto& bones = SkeletonLists::Bones;
			for (EBoneIndex Bone : bones) {
				Player.Skeleton.LocationBones[Bone] = VectorHelper::GetBoneWithRotation(Player.Skeleton.Bones[Bone], Player.ComponentToWorld);
				Player.Skeleton.ScreenBones[Bone] = VectorHelper::WorldToScreen(Player.Skeleton.LocationBones[Bone]);
				
			}
			

			RenderHelper::PlayerColor PlayerColors = RenderHelper::GetPlayerColor(Player);
			ImColor UseColor = PlayerColors.infoUseColor;
			
		
			if (!Player.InScreen)
				continue;

			//是否可以显示骨骼
			if (GameData.Config.ESP.Skeleton) {
				bool isLockedTarget = (GameData.AimBot.Target == Player.Entity && GameData.AimBot.Lock);
				bool shouldDrawSkeleton = !GameData.Config.ESP.LockedHiddenBones || !isLockedTarget;

				if (GameData.Config.ESP.Skeleton && (!GameData.Config.ESP.LockedHiddenBones || (GameData.AimBot.Target != Player.Entity || !GameData.AimBot.Lock)))
				{
					DrawSkeleton_Physx(Player, PlayerColors.skeletonUseColor, GameData.Config.ESP.SkeletonWidth);
				}

			}

			ImVec2 Pos = {
				static_cast<float>(static_cast<int>(Player.Skeleton.ScreenBones[EBoneIndex::Root].X)),
				static_cast<float>(static_cast<int>(Player.Skeleton.ScreenBones[EBoneIndex::ForeHead].Y))
			};

			int FontSize = GameData.Config.ESP.FontSize;

			auto Rect = Get2DBox(Player);

			FVector2D HeadPos = Player.Skeleton.ScreenBones[EBoneIndex::ForeHead];
			//鎖定變色 
			if (GameData.Config.ESP.soudingbianse) {
				if(GameData.AimBot.Target==Player.Entity && GameData.AimBot.Lock){
					PlayerColors.skeletonUseColor = Utils::FloatToImColor(GameData.Config.ESP.Color.aim.Skeleton);	
					//TriggerMouseLeft();
				}
				else if (!Player.IsVisible) {

					PlayerColors.skeletonUseColor = Utils::FloatToImColor(GameData.Config.ESP.Color.Default.Skeleton);
				}
			}
			if (GameData.Config.ESP.Skeleton && (!GameData.Config.ESP.LockedHiddenBones || (GameData.AimBot.Target != Player.Entity || !GameData.AimBot.Lock))) {
				DrawSkeleton_Physx(Player, PlayerColors.skeletonUseColor, GameData.Config.ESP.SkeletonWidth);
			}
			//头部骨骼
			if (GameData.Config.ESP.HeadDrawing)
			{
				ImVec2 Headpos = { (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::Head].X, (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::ForeHead].Y };
				if (GameData.Config.ESP.Skeleton &&
					(!GameData.Config.ESP.LockedHiddenBones ||
						(GameData.AimBot.Target != Player.Entity || !GameData.AimBot.Lock)))
					RenderHelper::Circle(ImVec2(Headpos.x, Headpos.y), Rect.w / 12, PlayerColors.skeletonUseColor, GameData.Config.ESP.SkeletonWidth, 32);
			}
			const auto h = (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::Foot_L].Y - (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::ForeHead].Y, w = (h / 4.0f) * 2.5f, x = (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::Root].X - (w / 2.0f), y = (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::ForeHead].Y;

			const float Scale = (float)FontSize / 14.f;
			ImVec2 HealthBarPos, HealthBarSize;

			auto health_xPos = NULL; // 定义变量，存储血条的x坐标

			//绘制血条
			float Health = Player.State == CharacterState::Groggy ? Player.GroggyHealth : Player.Health;
			if (Player.State == CharacterState::Groggy) UseColor = Utils::FloatToImColor(GameData.Config.ESP.Color.Groggy.Info);
			constexpr auto health_bar_width = 20.0f; // 血条的固定宽度
			constexpr auto health_bar_height = 4.0f;  // 血条的固定高度
			constexpr auto example_health_max_value = 100.0f;
			//只有在配置中启动血条时才绘制血条
			if (GameData.Config.ESP.health_bar && bShowInfo) {

				const auto clamped_health = std::clamp(Health, 0.0f, 100.0f);
				const auto health_fill_width = (health_bar_width * clamped_health) / example_health_max_value;
				const auto health_x = x + (w - health_bar_width) / 2.0f;

				const auto health_y = y - 5.0f;
				//检查是否绘制顶部血条
				if (GameData.Config.ESP.XueTiaoWEizhi == 0)//血条位置
				{
					if (GameData.Config.ESP.HealthBarStyle == 0)
					{
						constexpr auto health_bar_width = 20.0f; // 血条的固定宽度
						constexpr auto health_bar_height = 4.0f;  // 血条的固定高度
						constexpr auto example_health_max_value = 100.0f;
	
						const auto color_1 = ImColor{ 255, 0, 0 };
						const auto color_2 = ImColor{ 255, 128, 0 };
						const auto color_3 = ImColor{ 0, 255, 0 };
						const auto health_x = x + (w - health_bar_width) / 2.0f;
						health_xPos = health_x;

						//绘制血条背景
						RenderHelper::window_filled_rect({ health_x, y - 5.0f }, { health_bar_width,health_bar_height }, { 30, 30, 30 });
						const auto health_fill_width = (health_bar_width * clamped_health) / example_health_max_value;


						//// 根据健康值设置彩虹颜色
						const auto rainbow_bar = [](const ImVec2& position, const ImVec2& size, const ImColor& color1, const ImColor& color2, const ImColor& color3, const ImColor& color4)
							{
								RenderHelper::window_filled_rect_multicolor_horizontal(position, { size.x / 2.0f, size.y }, color1, color2);
								RenderHelper::window_filled_rect_multicolor_horizontal({ position.x + size.x / 2.0f, position.y }, { size.x / 2.0f, size.y }, color3, color4);
							};

						rainbow_bar
						(
							{ health_x + 1.0f, y - 4.0f }, { (health_fill_width - 2.0f),health_bar_height-1.0f },
							color_1,
							Health < 30.0f ? color_1 : color_2,
							Health < 30.0f ? color_1 : color_2,
							Health < 60.0f ? color_2 : color_3
						);


					}
					else if(GameData.Config.ESP.HealthBarStyle == 1){
						constexpr auto health_bar_width = 20.0f; // 血条的固定宽度
						constexpr auto health_bar_height = 4.0f;  // 血条的固定高度
						constexpr auto example_health_max_value = 100.0f;

						const auto color_3 = ImColor{ 0, 255, 0 };
						
						const auto health_x = x + (w - health_bar_width) / 2.0f;
						health_xPos = health_x;

						// 绘制血条背景
						RenderHelper::window_filled_rect({ health_x, y - 5.0f }, { health_bar_width, health_bar_height }, { 30, 30, 30 });

						const auto health_fill_width = (health_bar_width * Health) / example_health_max_value;

						// 根据健康值设置彩虹颜色
						const auto rainbow_bar = [](const ImVec2& position, const ImVec2& size, const ImColor& color_1, const ImColor& color_2, const ImColor& color3, const ImColor& color4) {
							RenderHelper::window_filled_rect_multicolor_horizontal(position, { size.x / 2.0f, size.y }, color_1, color_2);
							RenderHelper::window_filled_rect_multicolor_horizontal({ position.x + size.x / 2.0f, position.y }, { size.x / 2.0f, size.y }, color3, color4);
							};

						rainbow_bar
						(
							{ health_x + 1.0f, y - 4.0f },
							{ health_fill_width - 2.0f, health_bar_height - 1.0f },
							color_3,
							Health < 30.0f ? color_3 : color_3,
							Health < 30.0f ? color_3 : color_3,
							Health < 60.0f ? color_3 : color_3
						);

					}
					else if (GameData.Config.ESP.HealthBarStyle == 2) {

						auto example_health_value = Health;
						constexpr auto example_health_max_value = 100.0f;
						const auto color_1 = ImColor{ 255, 0, 0 };
						const auto green_color = ImColor{ 0, 255, 0 };

						auto nickname_label_size = ImVec2{w,6.0f * 3.0f};

						const auto health_w = (nickname_label_size.x < w) ? w : nickname_label_size.x;
						const auto health_x = x + (w - health_w) / 2.0f;
						health_xPos = health_x;

						// 绘制血条背景
						RenderHelper::window_filled_rect({ health_x, y - 5.0f }, { health_w, 3.0f }, { 30, 30, 30 });

						RenderHelper::window_filled_rect_multicolor_horizontal(
							{ health_x + 1.0f, y - 4.0f },
							{ (health_w - 2.0f * example_health_value)/ example_health_value, 2.0f },
							color_1,
							green_color
							
							
						);
					
					}
					else if (GameData.Config.ESP.HealthBarStyle == 3) {

						auto example_health_value = Health;
						constexpr auto example_health_max_value = 100.0f;

						const auto color_1 = ImColor{ 255, 0, 0 };
						const auto color_2 = ImColor{ 255, 128, 0 };
						const auto color_3 = ImColor{ 0, 255, 0 };

						auto nickname_label_size = ImVec2{ w,6.0f * 3.0f };

						const auto health_w = (nickname_label_size.x < w) ? w : nickname_label_size.x;

						const auto health_x = x + (w - health_w) / 2.0f;

						health_xPos = health_x;

						// 绘制血条背景
						RenderHelper::window_filled_rect({ health_x, y - 5.0f }, { health_w, 3.0f }, { 30, 30, 30 });



						// 根据健康值设置彩虹颜色
						const auto rainbow_bar = [](const ImVec2& position, const ImVec2& size, const ImColor& color1, const ImColor& color2, const ImColor& color3, const ImColor& color4) {
							RenderHelper::window_filled_rect_multicolor_horizontal(position, { size.x / 2.0f, size.y }, color1, color2);
							RenderHelper::window_filled_rect_multicolor_horizontal({ position.x + size.x / 2.0f, position.y }, { size.x / 2.0f, size.y }, color3, color4);
							};

						rainbow_bar
						(
							{ health_x + 1.0f, y - 4.0f },
							{ ((health_w  - 2.0f )* example_health_value) / example_health_max_value, 2.0f },
							color_1,
							example_health_value < 30.0f ? color_1 : color_2,
							example_health_value < 30.0f ? color_1 : color_2,
							example_health_value < 60.0f ? color_2 : color_3
						);


					}
				}
				else if (GameData.Config.ESP.XueTiaoWEizhi == 1)
				{
					//绘制左侧血条时不在绘制顶部血条
					HealthBarPos = { Rect.x - 6, Rect.y  };
					HealthBarSize = { std::clamp( Rect.w / 4.f, 0.f, 4.f) ,Rect.w - 1 };

					DrawHealthBar(Player.Entity, 100.f, Health, HealthBarPos, HealthBarSize, UseColor, true);
					health_xPos = HealthBarPos.x;
				}
				
			}
			std::string InfoText = "";

			std::string TeamID = std::to_string(Player.TeamID);

			if (Player.Type == EntityType::AI)
			{
				TeamID = "BOT";
			}
			//队伍编号
			ImVec2 textSize = ImGui::CalcTextSize(Utils::StringToUTF8(InfoText).c_str());
			if (GameData.Config.ESP.TeamID && bShowInfo) {
				//RenderHelper::DrawRadarPlayerCircleWithText(
				//	std::to_string(Player.TeamID).c_str(),
				//	PlayerColors.teamNumberColor,
				//	7 * (14 / 12.f),
				//	/*{ health_xPos - 25.0f, y - 5.0f },*/
				//	{ health_xPos - textSize.x / 7 , Pos.y + 2.0f },
				//	0);
				InfoText += std::format("[{}] ", Player.TeamID);
			}
				
			//战队
			if (GameData.Config.ESP.ClanName && Player.ClanName != "") InfoText += std::format("[{}] ", Player.ClanName);
			//名字
			if (GameData.Config.ESP.Nickname && bShowInfo) InfoText += Player.Name + " ";

			


			Utils::Trim(InfoText);//人名字绘制

			if (InfoText != "") {
				ImVec2 HeadInfoSize = RenderHelper::StrokeText(Utils::StringToUTF8(InfoText).c_str(), { Pos.x, Pos.y - 6.f }, PlayerColors.infoUseColor, FontSize, true, true);
				Pos.y -= HeadInfoSize.y;
			}
			auto Ranky = Pos.y;
			ImVec2 HeadInfoSize;
			PlayerRankInfo PlayerRankData;
			PlayerRankList temp = Data::GetPlayerSegmentListsItem(Player.Name);
			switch (GameData.Config.PlayerList.RankMode) {
			case 1:
				PlayerRankData = temp.TPP;
				break;
			case 2:
				PlayerRankData = temp.SquadTPP;
				break;
			case 3:
				PlayerRankData = temp.FPP;
				break;
			case 4:
				PlayerRankData = temp.SquadFPP;
				break;
			default:
				break;
			}

			// 只在段位不为空的情况下处理
			if (!PlayerRankData.TierToString.empty())
			{
				// 计算排名文本的大小
				HeadInfoSize = RenderHelper::StrokeText(PlayerRankData.TierToString.c_str(), { Pos.x - GameData.Config.ESP.FontSize + 1, Pos.y - 6.f }, PlayerColors.infoUseColor, FontSize, true, true);
				Pos.y -= HeadInfoSize.y;
				// 准备KDA文本和大小
				ImVec2 KDASize = ImVec2(0, 0); // 默认值
				std::string InfoText;
				if (PlayerRankData.KDAToString != "0.00")
				{
					InfoText = std::format(" {}", PlayerRankData.KDAToString);
					KDASize = RenderHelper::StrokeText(Utils::StringToUTF8(InfoText).c_str(), { Pos.x + GameData.Config.ESP.FontSize + 1, Pos.y - 6.f }, PlayerColors.infoUseColor, FontSize, true, false);
				}
				// 绘制排名图标
				//if (GameData.Config.ESP.showIcon) {
				//	std::string IconUrl = "Assets/RankImage/" + PlayerRankData.Tier + "-" + PlayerRankData.SubTier + ".png";
				//	if (PlayerRankData.Tier == "Master")
				//		IconUrl = "Assets/image/RankImage/" + PlayerRankData.Tier + ".png";
				//	if (PlayerRankData.Tier == "")
				//		IconUrl = "Assets/image/RankImage/Unranked.png";
				//	if (GImGuiTextureMap[IconUrl].Width > 0)
				//	{
				//		RenderHelper::Image(GImGuiTextureMap[IconUrl].Texture, { health_xPos - 62.0f - GameData.Config.ESP.FontSize , Ranky - 70.0f }, ImVec2(45, 45), false); // 向上移动图标
				//	}

				//}
				
			}
			std::string KDAText = "";
			if (GameData.Config.ESP.KDA && PlayerRankData.KDA > 0.0f)
			{
				std::ostringstream oss;
				oss << std::fixed << std::setprecision(2) << PlayerRankData.KDA;
				KDAText += "KD:" + oss.str();
				ImVec2 HeadInfoSize = RenderHelper::StrokeText(Utils::StringToUTF8(KDAText).c_str(), { Pos.x, Pos.y }, IM_COL32(255, 0, 0, 255), FontSize, true, false);
				Pos.y += HeadInfoSize.y - 2;
			}
		
			//枪械图标
			if (GameData.Config.ESP.Weapon && bShowWeapon)
			{
				std::string IconUrl = "Assets/image/Weapon/" + Player.WeaponName + ".png";
				if (GImGuiTextureMap[IconUrl].Width > 0) {
					float TargetHeight = 16.f * Scale;
					float HeightZoom = TargetHeight / GImGuiTextureMap[IconUrl].Height;
					float IconWidth = GImGuiTextureMap[IconUrl].Width * HeightZoom;
					float IconHeight = TargetHeight;
					Pos.x -= IconWidth / 2;
					Pos.y -= IconHeight + 3;// 
					if (!GameData.Config.PlayerList.RankMode)
						Pos.y += HeadInfoSize.y;
					RenderHelper::Image(GImGuiTextureMap[IconUrl].Texture, Pos, ImVec2(IconWidth, IconHeight), true, PlayerColors.infoUseColor);
				}
			}
			Pos.x = (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::Root].X;
			Pos.y = (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::Root].Y;
			if (GameData.Config.ESP.Dis)
			{
				//距离
				InfoText = "";

				InfoText += std::format("{}M", (int)Player.Distance);

				if (InfoText != "") {
					ImVec2 HeadInfoSize = RenderHelper::StrokeText(Utils::StringToUTF8(InfoText).c_str(), { Pos.x, Pos.y }, PlayerColors.infoUseColor, FontSize, true, false);
					Pos.y += HeadInfoSize.y - 2;
				}
			}
			if (GameData.Config.ESP.Health)
			{
				//血量
				InfoText = "";

				InfoText += std::format("{}HP", (int)Player.Health);

				if (InfoText != "") {
					ImVec2 HeadInfoSize = RenderHelper::StrokeText(Utils::StringToUTF8(InfoText).c_str(), { Pos.x, Pos.y }, PlayerColors.infoUseColor, FontSize, true, false);
					Pos.y += HeadInfoSize.y - 2;
				}
			}
			//方框
			if (GameData.Config.ESP.DisplayFrame)
			{
				RenderHelper::Line(ImVec2(Rect.x, Rect.y), ImVec2(Rect.x + Rect.z, Rect.y), PlayerColors.skeletonUseColor, 2.0f);// 上横
				RenderHelper::Line(ImVec2(Rect.x, Rect.y), ImVec2(Rect.x, Rect.y + Rect.z * 1.6), PlayerColors.skeletonUseColor, 2.0f);// 左竖
				RenderHelper::Line(ImVec2(Rect.x + Rect.z, Rect.y), ImVec2(Rect.x + Rect.z, Rect.y + Rect.z * 1.6), PlayerColors.skeletonUseColor, 2.0f);// 右竖
				RenderHelper::Line(ImVec2(Rect.x, Rect.y + Rect.z * 1.6), ImVec2(Rect.x + Rect.z, Rect.y + Rect.z * 1.6), PlayerColors.skeletonUseColor, 2.0f);// 左竖

			}
			//合作者
			if (GameData.Config.ESP.Partner && Player.PartnerLevel > 0 && bShowInfo) {
				InfoText = "";

				InfoText += std::format("走狗Lvl.{}", (int)Player.PartnerLevel);

				if (InfoText != "") {
					ImVec2 HeadInfoSize = RenderHelper::StrokeText(Utils::StringToUTF8(InfoText).c_str(), { Pos.x, Pos.y }, PlayerColors.infoUseColor, FontSize, true, false);
					Pos.y += HeadInfoSize.y - 2;
				}
			}
			//等级
			if (GameData.Config.ESP.等级)
			{
				InfoText = "";
				InfoText += std::format("Lv.{}", (int)Player.SurvivalLevel);

				if (InfoText != "") {
					ImVec2 HeadInfoSize = RenderHelper::StrokeText(Utils::StringToUTF8(InfoText).c_str(), { Pos.x, Pos.y }, PlayerColors.infoUseColor, FontSize, true, false);
					Pos.y += HeadInfoSize.y - 2;
				}
			}
			//击杀
			if (GameData.Config.ESP.击杀 && Player.KillCount > 0)
			{
				InfoText = "";
				InfoText += std::format("K.{}", (int)Player.KillCount);

				if (InfoText != "") {
					ImVec2 HeadInfoSize = RenderHelper::StrokeText(Utils::StringToUTF8(InfoText).c_str(), { Pos.x, Pos.y }, PlayerColors.infoUseColor, FontSize, true, false);
					Pos.y += HeadInfoSize.y - 2;
				}
			}
			//伤害
			if (GameData.Config.ESP.伤害 && Player.DamageDealtOnEnemy > 0)
			{
				InfoText = "";
				InfoText += std::format("S.{}", (int)Player.DamageDealtOnEnemy);

				if (InfoText != "") {
					ImVec2 HeadInfoSize = RenderHelper::StrokeText(Utils::StringToUTF8(InfoText).c_str(), { Pos.x, Pos.y }, PlayerColors.infoUseColor, FontSize, true, false);
					Pos.y += HeadInfoSize.y - 2;
				}
			}
			//掉线状态
			if (GameData.Config.ESP.ShowInfos && bShowInfo) {
				InfoText = "";
				switch (Player.CharacterState)
				{
				case ECharacterState::Offline:
					InfoText = U8("掉线");
					break;
				default:
					break;
				}
				if (InfoText != "") {
					ImVec2 HeadInfoSize = RenderHelper::StrokeText(InfoText.c_str(), { Pos.x, Pos.y }, PlayerColors.infoUseColor, FontSize, true, false);
					Pos.y += HeadInfoSize.y - 2;
				}
			}
			//观战
			if (GameData.Config.ESP.观战 && Player.SpectatedCount > 0)
			{
				InfoText = "";
				InfoText += std::format("G.{}", (int)Player.SpectatedCount);

				if (InfoText != "") {
					ImVec2 HeadInfoSize = RenderHelper::StrokeText(Utils::StringToUTF8(InfoText).c_str(), { Pos.x, Pos.y }, PlayerColors.infoUseColor, FontSize, true, false);
					Pos.y += HeadInfoSize.y - 2;
				}
			}
			//被瞄射线
			if (Player.IsAimMe && GameData.Config.ESP.TargetedRay)
			{
				auto IsAimMeX = Player.Skeleton.ScreenBones[EBoneIndex::ForeHead].X;
				auto IsAimMeY = Player.Skeleton.ScreenBones[EBoneIndex::ForeHead].Y;
				
				if (!Player.InScreen)
				{
					FVector2D LoctionToScreen = VectorHelper::WorldToScreen(Player.Location);
					IsAimMeY = LoctionToScreen.Y;
					IsAimMeY = LoctionToScreen.X;
				}

				if (!GameData.Config.ESP.VisibleCheckRay || Player.IsVisible)
				{
					
					RenderHelper::Line(ImVec2(IsAimMeX, IsAimMeY), ImVec2(GameData.Config.Overlay.ScreenWidth / 2, GameData.Config.Overlay.ScreenHeight / 2), ImGui::ColorConvertFloat4ToU32(Utils::FloatToImColor(GameData.Config.ESP.Color.Ray.Line)), GameData.Config.ESP.RayWidth);
				}
				
			}

		}
		//危险预警
		if (GameData.Config.ESP.DangerWarning)
		{

			// 计算相对位置（例如，固定在窗口的右下角）
			float posX = GameData.Config.Overlay.ScreenWidth / 2; // 相对于窗口宽度的85%
			float posY = GameData.Config.Overlay.ScreenHeight - 150; // 相对于窗口高度的90%
			int EnemiesBehindCount = 0;
			bool IsBeingAimedFromBehind = false;

			for (auto& Item : Players)
			{
				Player& Player = Item.second;
				if (!Player.IsAimMe) continue;

				float PlayerAngle = atan2(
					Player.Location.Y - GameData.LocalPlayerInfo.Location.Y,
					Player.Location.X - GameData.LocalPlayerInfo.Location.X

				) * 180 / M_PI;

				PlayerAngle = fmod(PlayerAngle - GameData.Camera.Rotation.Yaw + 360, 360);

				if (PlayerAngle >= 90 && PlayerAngle <= 270)
				{
					EnemiesBehindCount++;
					IsBeingAimedFromBehind = true;
				}
			}

			
			// 绘制文本
			if (M200PlayerCount > 0) {
				RenderHelper::StrokeText(Utils::StringToUTF8(std::format(
					"200米范围内敌人数量: {}  离你最近的敌人: {}米 最近的玩家名字: {}",
					M200PlayerCount,
					PalyerBB,
					DisNane
				)).c_str(), ImVec2(posX, posY), IM_COL32(255, 0, 0, 255), 14.0, true, true);
			}
			if (IsBeingAimedFromBehind)
			{
				std::string warningText = std::format(
					"敌人在背后瞄准你！"
				);

				ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(24, FLT_MAX, 0.f, warningText.c_str());
				ImVec2 textPos = { GameData.Config.Overlay.ScreenWidth / 2 - textSize.x / 2, 100 }; // 屏幕顶部

				RenderHelper::StrokeText(Utils::StringToUTF8(warningText).c_str(), textPos, IM_COL32(255, 0, 0, 255), 14, true, false);
			}

		}

	}

	static void DrawPlayersEarly(std::unordered_map<uint64_t, Player> Players)
	{
		if (!GameData.Config.Early.Enable) return;

		const float Scale = (float)GameData.Config.Early.FontSize / 14.f;
		float Size = 200.f * Scale;
		float AngleXSize = 2.f * Scale;
		float AngleYSize = 5.f * Scale;

		for (auto& Item : Players)
		{
			Player& Player = Item.second;

			if (Player.IsMe || Player.IsMyTeam || (Player.State == CharacterState::Dead))
			{
				continue;
			}

			float Distance = (GameData.Camera.Location).Distance(Player.Location) / 100;
			float Angle = atan2(GameData.Camera.Location.Y - Player.Location.Y, GameData.Camera.Location.X - Player.Location.X);
			Angle = Angle * 180 / M_PI;
			Angle = GameData.Camera.Rotation.Yaw + 360 - Angle; // Note: Y is actually X due to storage order differences

			ImVec2 A, B, C;
			A.x = (Size + AngleYSize) * sin(Angle * M_PI / 180) + GameData.Config.Overlay.ScreenWidth / 2;
			A.y = (Size + AngleYSize) * cos(Angle * M_PI / 180) + GameData.Config.Overlay.ScreenHeight / 2;

			B.x = (Size - AngleYSize) * sin((Angle - AngleXSize) * M_PI / 180) + GameData.Config.Overlay.ScreenWidth / 2;
			B.y = (Size - AngleYSize) * cos((Angle - AngleXSize) * M_PI / 180) + GameData.Config.Overlay.ScreenHeight / 2;

			C.x = (Size - AngleYSize) * sin((Angle + AngleXSize) * M_PI / 180) + GameData.Config.Overlay.ScreenWidth / 2;
			C.y = (Size - AngleYSize) * cos((Angle + AngleXSize) * M_PI / 180) + GameData.Config.Overlay.ScreenHeight / 2;

			ImU32 Color = ImColor(255, 0, 0, 255);

			float MaxDistance = GameData.Config.Early.DistanceMax;
			float Segment = MaxDistance / 4.0f;

			if (Distance < MaxDistance)
			{
				/*if (Distance < 150.0f)
					Color = ImColor(255, 0, 255, 255);
				else if (Distance < 300.0f)
					Color = ImColor(255, 255, 0, 255);
				else if (Distance < 400.0f)
					Color = ImColor(255, 0, 0, 255);*/

				if (Distance >= Segment && Distance < Segment * 2)
				{
					Color = ImColor(255, 165, 0, 255); // 红色
				}
				else if (Distance >= Segment * 2 && Distance < Segment * 3)
				{
					Color = ImColor(0, 255, 0, 255); // 橙黄色
				}
				else if (Distance >= Segment * 3)
				{
					Color = ImColor(255, 255, 255, 255); // 绿色
				}

				if (GameData.Config.Early.ShowDistance) ImVec2 HeadInfoSize = RenderHelper::StrokeText(std::format("{}M", (int)Distance).c_str(), { A.x + 13 * Scale, A.y }, Color, 13 * Scale, false, true);
				RenderHelper::DrawTriangle(A, B, C, Color, Player.IsAimMe);
			}
		}
	}

	static void DrawPackages(std::unordered_map<uint64_t, PackageInfo> Packages)
	{
		if (!GameData.Config.AirDrop.Enable && !GameData.Config.DeadBox.Enable) return;

		std::vector<std::pair<uint64_t, PackageInfo>> VectorItems(Packages.begin(), Packages.end());

		std::sort(VectorItems.begin(), VectorItems.end(), [](const std::pair<uint64_t, PackageInfo>& a, const std::pair<uint64_t, PackageInfo>& b) {
			return a.second.Distance > b.second.Distance;
			});

		for (auto& Item : VectorItems)
		{
			PackageInfo& Package = Item.second;

			if (Package.Type == EntityType::AirDrop)
			{
				if (Package.Distance > GameData.Config.AirDrop.DistanceMax || !GameData.Config.AirDrop.Enable) continue;

				Package.ScreenLocation = VectorHelper::WorldToScreen(Package.Location);
				Package.Distance = GameData.Camera.Location.Distance(Package.Location) / 100.0f;

				std::string Text = Utils::StringToUTF8(std::format("空投 [{}M]", (int)Package.Distance));

				ImColor InfoColor = Utils::FloatToImColor(GameData.Config.AirDrop.Color);

				ImVec2 HeadInfoSize = RenderHelper::StrokeText(Text.c_str(), { Package.ScreenLocation.X, Package.ScreenLocation.Y }, InfoColor, GameData.Config.AirDrop.FontSize, false, false);

				if (GameData.Config.AirDrop.ShowItems)
				{
					int Index = 0;
					int FistIndex = 0;
					FVector2D FistPos;

					for (auto& ItemInfo : Package.Items)
					{
						std::string Text = Utils::StringToUTF8(std::format("{}", ItemInfo.DisplayName));
						int GroupIndex = GameData.Config.Item.Lists[ItemInfo.Name].Group;
						ImColor TextColor = InfoColor;
						float FontSize = GameData.Config.AirDrop.FontSize - 1;

						switch (GroupIndex)
						{
						case 1:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupAColor);
							break;
						case 2:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupBColor);
							break;
						case 3:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupCColor);
							break;
						case 4:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupDColor);
							break;
						default:
							continue;
							break;
						}

						FistPos = Package.ScreenLocation;
						FistPos.Y += HeadInfoSize.y;

						ImVec2 ScreenLocation = { FistPos.X, FistPos.Y + ((FontSize + 1) * Index) };
						ImVec2 TextSize = RenderHelper::StrokeText(Text.c_str(), { ScreenLocation.x, ScreenLocation.y }, TextColor, FontSize, false, false);
						if (GameData.Config.Item.ShowIcon) {
							float Scale = (FontSize / 14.f);
							std::string IconUrl = "Assets/image/All/" + ItemInfo.Name + ".png";
							if (GImGuiTextureMap[IconUrl].Width > 0) {
								float TargetHeight = 14.f * Scale;
								float HeightZoom = TargetHeight / GImGuiTextureMap[IconUrl].Height;
								float IconWidth = GImGuiTextureMap[IconUrl].Width * HeightZoom;
								float IconHeight = TargetHeight;
								ScreenLocation.x -= (FontSize + 4);

								RenderHelper::Image(GImGuiTextureMap[IconUrl].Texture, ScreenLocation, ImVec2(IconWidth, IconHeight), false);
							}
						}

						Index++;
					}
				}
			}
			else {
				if (Package.Distance > GameData.Config.DeadBox.DistanceMax || !GameData.Config.DeadBox.Enable) continue;

				Package.ScreenLocation = VectorHelper::WorldToScreen(Package.Location);
				Package.Distance = GameData.Camera.Location.Distance(Package.Location) / 100.0f;

				std::string Text = Utils::StringToUTF8(std::format("骨灰盒 [{}M]", (int)Package.Distance));

				ImColor InfoColor = Utils::FloatToImColor(GameData.Config.DeadBox.Color);

				ImVec2 HeadInfoSize = RenderHelper::StrokeText(Text.c_str(), { Package.ScreenLocation.X, Package.ScreenLocation.Y }, InfoColor, GameData.Config.DeadBox.FontSize, false, false);

				if (GameData.Config.DeadBox.ShowItems)
				{
					int Index = 0;
					int FistIndex = 0;
					FVector2D FistPos;

					for (auto& ItemInfo : Package.Items)
					{
						std::string Text = Utils::StringToUTF8(std::format("{}", ItemInfo.DisplayName));
						int GroupIndex = GameData.Config.Item.Lists[ItemInfo.Name].Group;
						ImColor TextColor = InfoColor;
						float FontSize = GameData.Config.DeadBox.FontSize - 1;

						switch (GroupIndex)
						{
						case 1:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupAColor);
							break;
						case 2:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupBColor);
							break;
						case 3:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupCColor);
							break;
						case 4:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupDColor);
							break;
						default:
							continue;
							break;
						}

						FistPos = Package.ScreenLocation;
						FistPos.Y += HeadInfoSize.y + 2;

						ImVec2 ScreenLocation = { FistPos.X, FistPos.Y + ((FontSize + 1) * Index) };
						ImVec2 TextSize = RenderHelper::StrokeText(Text.c_str(), { ScreenLocation.x, ScreenLocation.y }, TextColor, FontSize, false, false);
						if (GameData.Config.Item.ShowIcon) {
							float Scale = (FontSize / 14.f);
							std::string IconUrl = "Assets/image/All/" + ItemInfo.Name + ".png";
							if (GImGuiTextureMap[IconUrl].Width > 0) {
								float TargetHeight = 14.f * Scale;
								float HeightZoom = TargetHeight / GImGuiTextureMap[IconUrl].Height;
								float IconWidth = GImGuiTextureMap[IconUrl].Width * HeightZoom;
								float IconHeight = TargetHeight;
								ScreenLocation.x -= (FontSize + 4);

								RenderHelper::Image(GImGuiTextureMap[IconUrl].Texture, ScreenLocation, ImVec2(IconWidth, IconHeight), false);
							}
						}

						Index++;
					}
				}
			}
		}
	}
	
	static void DrawNextHintMesh() {

		if (GameData.NextHintMeshData != nullptr && GameData.Config.ESP.PhysXDebug) {
			TriangleMeshData& mesh = *GameData.NextHintMeshData;

			try {

				// 如果mesh有顶点数据则渲染
				if (!mesh.Vertices.empty()
					&& !mesh.Indices.empty() && static_cast<uint32_t>(mesh.Type) < 10
					&& static_cast<uint32_t>(mesh.Type) >= 0
					&& mesh.Vertices.size() < 1000000
					&& mesh.Indices.size() < 1000000)
				{
					for (size_t i = 0; i < mesh.Indices.size(); i += 3)
					{
						if (i + 2 >= mesh.Indices.size())
							break;

						// 获取三角形的三个顶点
						Vector3 v0 = mesh.Vertices[mesh.Indices[i]];
						Vector3 v1 = mesh.Vertices[mesh.Indices[i + 1]];
						Vector3 v2 = mesh.Vertices[mesh.Indices[i + 2]];

						// 转换到屏幕坐标
						auto p0 = VectorHelper::WorldToScreen((FVector&)v0, true);
						auto p1 = VectorHelper::WorldToScreen((FVector&)v1, true);
						auto p2 = VectorHelper::WorldToScreen((FVector&)v2, true);

						//if (i == 0) {
						//	// 将flags转换为二进制字符串
						//	std::string binary;
						//	for (int bit = 31; bit >= 0; bit--) {
						//		binary += ((mesh.Flags >> bit) & 1) ? '1' : '0';
						//	}

						//	// 在三角形旁边渲染flags和filter_data的信息
						//	ImGui::GetForegroundDrawList()->AddText(
						//		ImVec2(p0.X, p0.Y - 60),
						//		ImColor(255, 255, 255, 255),
						//		(
						//			"Binary: " + binary + "\nDecimal: " + std::to_string(mesh.Flags) +
						//			"\nQuery Filter Data: " +
						//			std::to_string(mesh.QueryFilterData.word0) + "," +
						//			std::to_string(mesh.QueryFilterData.word1) + "," +
						//			std::to_string(mesh.QueryFilterData.word2) + "," +
						//			std::to_string(mesh.QueryFilterData.word3) +
						//			"\nSim Filter Data: " +
						//			std::to_string(mesh.SimulationFilterData.word0) + "," +
						//			std::to_string(mesh.SimulationFilterData.word1) + "," +
						//			std::to_string(mesh.SimulationFilterData.word2) + "," +
						//			std::to_string(mesh.SimulationFilterData.word3) +
						//			"\nVerties/Indices Num: " +
						//			std::to_string(mesh.Vertices.size()) + "," +
						//			std::to_string(mesh.Indices.size()) +
						//			"\nUniqueKey: " +
						//			std::to_string(mesh.UniqueKey1.Shape) + "," + 
						//			std::to_string(mesh.UniqueKey1.Actor)
						//		).c_str()
						//	);
						//}

						// 检查是否在屏幕内
						if (VectorHelper::IsInScreen(p0) || VectorHelper::IsInScreen(p1) || VectorHelper::IsInScreen(p2))
						{
							auto A = ImVec2(p0.X, p0.Y);
							auto B = ImVec2(p1.X, p1.Y);
							auto C = ImVec2(p2.X, p2.Y);
							// 绘制三角形边框
							RenderHelper::DrawTriangle(A, B, C, ImColor(255, 255, 255, 200));
						}
					}
				}

			}
			catch (...) {
				Utils::Log(2, "Wild pointer error");
			}
		}
	}

	static void DrawNearModel() {

		if (GameData.Scene != Scene::Gaming || !GameData.Config.ESP.PhysXDebug) {
			return;
		}

		std::vector<TriangleMeshData*> meshs{};
		if (GameData.NearDynamicMeshData.size() > 0) {
			meshs.insert(meshs.end(), GameData.NearDynamicMeshData.begin(), GameData.NearDynamicMeshData.end());
		}

		if (GameData.NearStaticMeshData.size() > 0) {
			meshs.insert(meshs.end(), GameData.NearStaticMeshData.begin(), GameData.NearStaticMeshData.end());
		}

		for (const auto meshPtr : meshs) {
			TriangleMeshData& mesh = *meshPtr;
			try {

				// 如果mesh有顶点数据则渲染
				if (!mesh.Vertices.empty()
					&& !mesh.Indices.empty() && static_cast<uint32_t>(mesh.Type) < 10
					&& static_cast<uint32_t>(mesh.Type) >= 0
					&& mesh.Vertices.size() < 1000000
					&& mesh.Indices.size() < 1000000)
				{
					for (size_t i = 0; i < mesh.Indices.size(); i += 3)
					{
						if (i + 2 >= mesh.Indices.size())
							break;

						// 获取三角形的三个顶点
						Vector3 v0 = mesh.Vertices[mesh.Indices[i]];
						Vector3 v1 = mesh.Vertices[mesh.Indices[i + 1]];
						Vector3 v2 = mesh.Vertices[mesh.Indices[i + 2]];

						// 转换到屏幕坐标
						auto p0 = VectorHelper::WorldToScreen((FVector&)v0, true);
						auto p1 = VectorHelper::WorldToScreen((FVector&)v1, true);
						auto p2 = VectorHelper::WorldToScreen((FVector&)v2, true);

						if (i == 0) {
							// 将flags转换为二进制字符串
							std::string binary;
							for (int bit = 31; bit >= 0; bit--) {
								binary += ((mesh.Flags >> bit) & 1) ? '1' : '0';
							}

							// 在三角形旁边渲染flags和filter_data的信息
							/*ImGui::GetForegroundDrawList()->AddText(
								ImVec2(p0.X, p0.Y - 60),
								ImColor(255, 255, 255, 255),
								(
									"Binary: " + binary + "\nDecimal: " + std::to_string(mesh.Flags) +
									"\nQuery Filter Data: " +
									std::to_string(mesh.QueryFilterData.word0) + "," +
									std::to_string(mesh.QueryFilterData.word1) + "," +
									std::to_string(mesh.QueryFilterData.word2) + "," +
									std::to_string(mesh.QueryFilterData.word3) +
									"\nSim Filter Data: " +
									std::to_string(mesh.SimulationFilterData.word0) + "," +
									std::to_string(mesh.SimulationFilterData.word1) + "," +
									std::to_string(mesh.SimulationFilterData.word2) + "," +
									std::to_string(mesh.SimulationFilterData.word3) +
									"\nVerties/Indices Num: " +
									std::to_string(mesh.Vertices.size()) + "," +
									std::to_string(mesh.Indices.size()) +
									"\nUniqueKey: " +
									std::to_string(mesh.UniqueKey1.Shape) + "," +
									std::to_string(mesh.UniqueKey1.Actor)
									).c_str()
							);*/
						}

						// 检查是否在屏幕内
						if (VectorHelper::IsInScreen(p0) || VectorHelper::IsInScreen(p1) || VectorHelper::IsInScreen(p2))
						{
							auto A = ImVec2(p0.X, p0.Y);
							auto B = ImVec2(p1.X, p1.Y);
							auto C = ImVec2(p2.X, p2.Y);
							// 绘制三角形边框
							RenderHelper::DrawTriangle(A, B, C, ImColor(255, 255, 255, 200));
						}
					}
				}

			}
			catch (...) {
				Utils::Log(2, "Wild pointer error");
			}
		}
	}

	static void DrawESP()
	{
		if (GameData.Config.Overlay.FusionMode) {

			ImGui::GetBackgroundDrawList()->AddRectFilled({ 0 ,0 }, { (float)GameData.Config.Overlay.ScreenWidth, (float)GameData.Config.Overlay.ScreenHeight }, ImColor(0, 0, 0, 255));

		}

		if (GameData.Scene != Scene::Gaming)
		{
			return;
		}

		std::unordered_map<uint64_t, Player> Players = Data::GetPlayers();
		std::unordered_map<uint64_t, VehicleWheelInfo> VehicleWheels = Data::GetVehicleWheels();
		std::unordered_map<uint64_t, VehicleInfo> Vehicles = Data::GetVehicles();
		std::unordered_map<uint64_t, ItemInfo> Items = Data::GetItems();
		std::unordered_map<uint64_t, ProjectInfo> Projects = Data::GetProjects();
		std::unordered_map<uint64_t, PackageInfo> Packages = Data::GetPackages();

		if (!GameData.bShowMouseCursor && !GameData.AimBot.Lock)
		{
			if (!GameData.Config.ESP.FocusMode)
			{
				DrawItems(Items);
				//DrawVehicleWheels(VehicleWheels);
				DrawVehicles(Vehicles);
				DrawPackages(Packages);
			}

			DrawFOV();

			DrawPlayersEarly(Players);
			//DrawTakeHitInstigator();
		}

		if (!GameData.Radar.Visibility)
		{
			DrawPlayers(Players);
		}
		DrawLocalPlayerProject();
		DrawAimBotPoint();
		DrawProjects(Projects);
		DrawRadars(Players, Vehicles, Packages);
		if (GameData.Config.ESP.PhysXDebug) {
			DrawNextHintMesh();
			//DrawNearModel();
		}
	};
};

