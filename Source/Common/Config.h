#pragma once
#include "Data.h"
#include "utils/utils.h"
#include <nlohmann/json.hpp>

class Config
{
public:
	template <typename T>
	static bool SetConfigItem(const nlohmann::json& config, const std::vector<std::string>& keys, T& value)
	{
		if (keys.empty()) {
			return false;
		}

		const std::string& currentKey = keys[0];

		if (config.count(currentKey) > 0) {
			if (keys.size() == 1) {
				value = config[currentKey].get<T>();
				return true;
			}
			else {
				return SetConfigItem(config[currentKey], std::vector<std::string>(keys.begin() + 1, keys.end()), value);
			}
		}
		else {
			return false;
		}
	}

	static bool Save()
	{
		nlohmann::json Config;

		[&] {
			std::string ConfigName = "Function";
			Config[ConfigName]["ClearKey"] = GameData.Config.Function.ClearKey;
		}();

		[&] {
				std::string ConfigName = "Vehicle";
				Config[ConfigName]["Enable"] = GameData.Config.Vehicle.Enable;
				Config[ConfigName]["EnableKey"] = GameData.Config.Vehicle.EnableKey;
				Config[ConfigName]["DistanceMax"] = GameData.Config.Vehicle.DistanceMax;
				Config[ConfigName]["FontSize"] = GameData.Config.Vehicle.FontSize;
				Config[ConfigName]["Health"] = GameData.Config.Vehicle.Health;
				Config[ConfigName]["Durability"] = GameData.Config.Vehicle.Durability;
				Config[ConfigName]["Color"]["0"] = GameData.Config.Vehicle.Color[0];
				Config[ConfigName]["Color"]["1"] = GameData.Config.Vehicle.Color[1];
				Config[ConfigName]["Color"]["2"] = GameData.Config.Vehicle.Color[2];
				Config[ConfigName]["Color"]["3"] = GameData.Config.Vehicle.Color[3];
		}();

		[&] {
				std::string ConfigName = "AirDrop";
				Config[ConfigName]["Enable"] = GameData.Config.AirDrop.Enable;
				Config[ConfigName]["EnableKey"] = GameData.Config.AirDrop.EnableKey;
				Config[ConfigName]["DistanceMax"] = GameData.Config.AirDrop.DistanceMax;
				Config[ConfigName]["FontSize"] = GameData.Config.AirDrop.FontSize;
				Config[ConfigName]["ShowItems"] = GameData.Config.AirDrop.ShowItems;

				Config[ConfigName]["Color"]["0"] = GameData.Config.AirDrop.Color[0];
				Config[ConfigName]["Color"]["1"] = GameData.Config.AirDrop.Color[1];
				Config[ConfigName]["Color"]["2"] = GameData.Config.AirDrop.Color[2];
				Config[ConfigName]["Color"]["3"] = GameData.Config.AirDrop.Color[3];
		}();

		[&] {
				std::string ConfigName = "DeadBox";
				Config[ConfigName]["Enable"] = GameData.Config.DeadBox.Enable;
				Config[ConfigName]["EnableKey"] = GameData.Config.DeadBox.EnableKey;
				Config[ConfigName]["DistanceMax"] = GameData.Config.DeadBox.DistanceMax;
				Config[ConfigName]["FontSize"] = GameData.Config.DeadBox.FontSize;
				Config[ConfigName]["ShowItems"] = GameData.Config.DeadBox.ShowItems;

				Config[ConfigName]["Color"]["0"] = GameData.Config.DeadBox.Color[0];
				Config[ConfigName]["Color"]["1"] = GameData.Config.DeadBox.Color[1];
				Config[ConfigName]["Color"]["2"] = GameData.Config.DeadBox.Color[2];
				Config[ConfigName]["Color"]["3"] = GameData.Config.DeadBox.Color[3];
		}();

		[&] {
				std::string ConfigName = "Early";
				Config[ConfigName]["Enable"] = GameData.Config.Early.Enable;
				Config[ConfigName]["DistanceMax"] = GameData.Config.Early.DistanceMax;
				Config[ConfigName]["FontSize"] = GameData.Config.Early.FontSize;
				Config[ConfigName]["ShowDistance"] = GameData.Config.Early.ShowDistance;
		}();

		[&] {
				std::string ConfigName = "PlayerList";
				Config[ConfigName]["RankMode"] = GameData.Config.PlayerList.RankMode;
		}();

		[&] {
				std::string ConfigName = "Project";
				Config[ConfigName]["Enable"] = GameData.Config.Project.Enable;
				Config[ConfigName]["DistanceMax"] = GameData.Config.Project.DistanceMax;
				Config[ConfigName]["GrenadePrediction"] = GameData.Config.Project.GrenadePrediction;
				Config[ConfigName]["FOV"] = GameData.Config.Project.FOV;
				Config[ConfigName]["FontSize"] = GameData.Config.Project.FontSize;
				Config[ConfigName]["TextShowChareTime"] = GameData.Config.Project.TextShowChareTime;
				Config[ConfigName]["BarShowChareTime"] = GameData.Config.Project.BarShowChareTime;
				Config[ConfigName]["ShowChareTime"] = GameData.Config.Project.ShowChareTime;
				Config[ConfigName]["ChareFontSize"] = GameData.Config.Project.ChareFontSize;

				Config[ConfigName]["ChareColor"]["0"] = GameData.Config.Project.ChareColor[0];
				Config[ConfigName]["ChareColor"]["1"] = GameData.Config.Project.ChareColor[1];
				Config[ConfigName]["ChareColor"]["2"] = GameData.Config.Project.ChareColor[2];
				Config[ConfigName]["ChareColor"]["3"] = GameData.Config.Project.ChareColor[3];

				Config[ConfigName]["Color"]["0"] = GameData.Config.Project.Color[0];
				Config[ConfigName]["Color"]["1"] = GameData.Config.Project.Color[1];
				Config[ConfigName]["Color"]["2"] = GameData.Config.Project.Color[2];
				Config[ConfigName]["Color"]["3"] = GameData.Config.Project.Color[3];
		}();

		[&] {
				std::string ConfigName = "AutoRecoil";
				Config[ConfigName]["Enabled"] = GameData.Config.AimBot.Recoilenanlek;
				Config[ConfigName]["Red Dot Amplitude"] = GameData.Config.AimBot.yRecoil[0];
				Config[ConfigName]["Double Amplitude"] = GameData.Config.AimBot.yRecoil[1];
				Config[ConfigName]["Triple Amplitude"] = GameData.Config.AimBot.yRecoil[2];
				Config[ConfigName]["Four Times The Amplitude"] = GameData.Config.AimBot.yRecoil[3];
				Config[ConfigName]["Six Times Amplitude"] = GameData.Config.AimBot.yRecoil[4];
				Config[ConfigName]["Eight Times Amplitude"] = GameData.Config.AimBot.yRecoil[5];
				Config[ConfigName]["Red Dot Delay"] = GameData.Config.AimBot.interval[0];
				Config[ConfigName]["Double Latency"] = GameData.Config.AimBot.interval[1];
				Config[ConfigName]["Triple Delay"] = GameData.Config.AimBot.interval[2];
				Config[ConfigName]["Four Times Delay"] = GameData.Config.AimBot.interval[3];
				Config[ConfigName]["Six Times Delay"] = GameData.Config.AimBot.interval[4];
				Config[ConfigName]["Eight Times Delay"] = GameData.Config.AimBot.interval[5];
		}();




		[&] {
				std::string ConfigName = "Radar";
				Config[ConfigName]["Main"]["ShowPlayer"] = GameData.Config.Radar.Main.ShowPlayer;
				Config[ConfigName]["Main"]["ShowVehicle"] = GameData.Config.Radar.Main.ShowVehicle;
				Config[ConfigName]["Main"]["ShowAirDrop"] = GameData.Config.Radar.Main.ShowAirDrop;
				Config[ConfigName]["Main"]["ShowDeadBox"] = GameData.Config.Radar.Main.ShowDeadBox;
				Config[ConfigName]["Main"]["FontSize"] = GameData.Config.Radar.Main.FontSize;
				Config[ConfigName]["Main"]["MapColor"]["0"] = GameData.Config.Radar.Main.MapColor[0];
				Config[ConfigName]["Main"]["MapColor"]["1"] = GameData.Config.Radar.Main.MapColor[1];
				Config[ConfigName]["Main"]["MapColor"]["2"] = GameData.Config.Radar.Main.MapColor[2];
				Config[ConfigName]["Main"]["MapColor"]["3"] = GameData.Config.Radar.Main.MapColor[3];
				Config[ConfigName]["Main"]["Map_size"] = GameData.Config.Radar.Main.Map_size;

				Config[ConfigName]["Mini"]["ShowPlayer"] = GameData.Config.Radar.Mini.ShowPlayer;
				Config[ConfigName]["Mini"]["ShowVehicle"] = GameData.Config.Radar.Mini.ShowVehicle;
				Config[ConfigName]["Mini"]["ShowAirDrop"] = GameData.Config.Radar.Mini.ShowAirDrop;
				Config[ConfigName]["Mini"]["ShowDeadBox"] = GameData.Config.Radar.Mini.ShowDeadBox;
				Config[ConfigName]["Mini"]["FontSize"] = GameData.Config.Radar.Mini.FontSize;
				Config[ConfigName]["Mini"]["MapColor"]["0"] = GameData.Config.Radar.Mini.MapColor[0];
				Config[ConfigName]["Mini"]["MapColor"]["1"] = GameData.Config.Radar.Mini.MapColor[1];
				Config[ConfigName]["Mini"]["MapColor"]["2"] = GameData.Config.Radar.Mini.MapColor[2];
				Config[ConfigName]["Mini"]["MapColor"]["3"] = GameData.Config.Radar.Mini.MapColor[3];
				Config[ConfigName]["Mini"]["Map_size"] = GameData.Config.Radar.Mini.Map_size;
		}();

		[&] {
				std::string ConfigName = "Item";
				Config[ConfigName]["Enable"] = GameData.Config.Item.Enable;
				Config[ConfigName]["EnableKey"] = GameData.Config.Item.EnableKey;
				//Config[ConfigName]["GroupKey"] = GameData.Config.Item.GroupKey;

				Config[ConfigName]["FilterEnable"] = GameData.Config.Item.FilterEnable;
				Config[ConfigName]["FirstAidKit"] = GameData.Config.Item.FirstAidKit;
				Config[ConfigName]["PainKiller"] = GameData.Config.Item.PainKiller;
				Config[ConfigName]["EnergyDrink"] = GameData.Config.Item.EnergyDrink;
				Config[ConfigName]["Bandage"] = GameData.Config.Item.Bandage;
				Config[ConfigName]["Epinephrine"] = GameData.Config.Item.Epinephrine;
				Config[ConfigName]["Medicalkit"] = GameData.Config.Item.Medicalkit;
				Config[ConfigName]["Grenade"] = GameData.Config.Item.Grenade;
				Config[ConfigName]["FlashGrenade"] = GameData.Config.Item.FlashGrenade;
				Config[ConfigName]["SmokeGrenade"] = GameData.Config.Item.SmokeGrenade;
				Config[ConfigName]["MolotovGrenade"] = GameData.Config.Item.MolotovGrenade;
				Config[ConfigName]["BluezoneGrenade"] = GameData.Config.Item.BluezoneGrenade;



				Config[ConfigName]["DistanceMax"] = GameData.Config.Item.DistanceMax;
				Config[ConfigName]["FontSize"] = GameData.Config.Item.FontSize;
				Config[ConfigName]["Combination"] = GameData.Config.Item.Combination;
				Config[ConfigName]["ShowIcon"] = GameData.Config.Item.ShowIcon;
				Config[ConfigName]["ShowDistance"] = GameData.Config.Item.ShowDistance;
				Config[ConfigName]["AccessoriesFilter"] = GameData.Config.Item.AccessoriesFilter;
				Config[ConfigName]["ThresholdX"] = GameData.Config.Item.ThresholdX;
				Config[ConfigName]["ThresholdY"] = GameData.Config.Item.ThresholdY;
				Config[ConfigName]["GroupAKey"] = GameData.Config.Item.GroupAKey;
				Config[ConfigName]["GroupBKey"] = GameData.Config.Item.GroupBKey;
				Config[ConfigName]["GroupCKey"] = GameData.Config.Item.GroupCKey;
				Config[ConfigName]["GroupDKey"] = GameData.Config.Item.GroupDKey;
				Config[ConfigName]["ShowGroup"] = GameData.Config.Item.ShowGroup;

				Config[ConfigName]["GroupAColor"]["0"] = GameData.Config.Item.GroupAColor[0];
				Config[ConfigName]["GroupAColor"]["1"] = GameData.Config.Item.GroupAColor[1];
				Config[ConfigName]["GroupAColor"]["2"] = GameData.Config.Item.GroupAColor[2];
				Config[ConfigName]["GroupAColor"]["3"] = GameData.Config.Item.GroupAColor[3];

				Config[ConfigName]["GroupBColor"]["0"] = GameData.Config.Item.GroupBColor[0];
				Config[ConfigName]["GroupBColor"]["1"] = GameData.Config.Item.GroupBColor[1];
				Config[ConfigName]["GroupBColor"]["2"] = GameData.Config.Item.GroupBColor[2];
				Config[ConfigName]["GroupBColor"]["3"] = GameData.Config.Item.GroupBColor[3];

				Config[ConfigName]["GroupCColor"]["0"] = GameData.Config.Item.GroupCColor[0];
				Config[ConfigName]["GroupCColor"]["1"] = GameData.Config.Item.GroupCColor[1];
				Config[ConfigName]["GroupCColor"]["2"] = GameData.Config.Item.GroupCColor[2];
				Config[ConfigName]["GroupCColor"]["3"] = GameData.Config.Item.GroupCColor[3];

				Config[ConfigName]["GroupDColor"]["0"] = GameData.Config.Item.GroupDColor[0];
				Config[ConfigName]["GroupDColor"]["1"] = GameData.Config.Item.GroupDColor[1];
				Config[ConfigName]["GroupDColor"]["2"] = GameData.Config.Item.GroupDColor[2];
				Config[ConfigName]["GroupDColor"]["3"] = GameData.Config.Item.GroupDColor[3];

				Config[ConfigName]["Lists"] = {};

				for (const auto& pair : GameData.Config.Item.Lists) {
					const std::string& key = pair.first;
					const ItemDetail& detail = pair.second;

					nlohmann::json ItemDetail = {
						{"Name", detail.Name},
						{"DisplayName", Utils::StringToUTF8(detail.DisplayName)},
						{"Type", static_cast<int>(detail.Type)},
						{"Group", detail.Group}
					};
					Config[ConfigName]["Lists"][key] = ItemDetail;
				}
		}();

		[&] {
				std::string ConfigName = "ESP";
				Config[ConfigName]["AdjustableDistance"] = GameData.Config.ESP.AdjustableDistance;
				Config[ConfigName]["DangerWarning"] = GameData.Config.ESP.DangerWarning;
				Config[ConfigName]["XueTiaoWEizhi"] = GameData.Config.ESP.XueTiaoWEizhi;
				Config[ConfigName]["PhysXDebug"] = GameData.Config.ESP.PhysXDebug;
				Config[ConfigName]["PhysxLoadRadius"] = GameData.Config.ESP.PhysxLoadRadius;
				Config[ConfigName]["PhysxStaticRefreshInterval"] = GameData.Config.ESP.PhysxStaticRefreshInterval;
				Config[ConfigName]["PhysxDynamicRefreshInterval"] = GameData.Config.ESP.PhysxDynamicRefreshInterval;
				Config[ConfigName]["PhysxRefreshLimit"] = GameData.Config.ESP.PhysxRefreshLimit;

				Config[ConfigName]["Enable"] = GameData.Config.ESP.Enable;//开启敌人透视
				Config[ConfigName]["Stroke"] = GameData.Config.ESP.Stroke;
				Config[ConfigName]["miwu"] = GameData.Config.ESP.miwu;
				Config[ConfigName]["DistanceStyle"] = GameData.Config.ESP.DistanceStyle;//透视距离
				Config[ConfigName]["HealthBarStyle"] = GameData.Config.ESP.HealthBarStyle;
				Config[ConfigName]["VisibleCheck"] = GameData.Config.ESP.VisibleCheck;
				Config[ConfigName]["AimExpandInfo"] = GameData.Config.ESP.AimExpandInfo;
				Config[ConfigName]["TargetedRay"] = GameData.Config.ESP.TargetedRay;//被瞄射线
				Config[ConfigName]["VisibleCheckRay"] = GameData.Config.ESP.VisibleCheckRay;//可视检测
				Config[ConfigName]["LockedHiddenBones"] = GameData.Config.ESP.LockedHiddenBones;//锁定不显
				Config[ConfigName]["Server IP"] = GameData.Config.ESP.服务器IP;
				Config[ConfigName]["shouchi"] = GameData.Config.ESP.Weapon;//手持
				Config[ConfigName]["hezuozhe"] = GameData.Config.ESP.Partner;//合作者
				
				Config[ConfigName]["jisha"] = GameData.Config.ESP.击杀;//击杀
				Config[ConfigName]["shanghai"] = GameData.Config.ESP.伤害;//伤害
				Config[ConfigName]["diaoxian"] = GameData.Config.ESP.ShowInfos;//掉线显示

				Config[ConfigName]["Nickname"] = GameData.Config.ESP.Nickname;//名字
				Config[ConfigName]["WeaponDistanceMax"] = GameData.Config.ESP.WeaponDistanceMax;//名字
		
				Config[ConfigName]["TeamID"] = GameData.Config.ESP.TeamID;//對標
				Config[ConfigName]["ClanName"] = GameData.Config.ESP.ClanName;//戰隊
				Config[ConfigName]["Dis"] = GameData.Config.ESP.Dis;//距离
				Config[ConfigName]["health_bar"] = GameData.Config.ESP.health_bar;//血條
				Config[ConfigName]["Health"] = GameData.Config.ESP.Health;//血量
				Config[ConfigName]["Tier"] = GameData.Config.ESP.Tier;//段位
				Config[ConfigName]["RankPoint"] = GameData.Config.ESP.RankPoint;//分數
				Config[ConfigName]["KDA"] = GameData.Config.ESP.KDA;//KDA
				Config[ConfigName]["dengji"] = GameData.Config.ESP.等级;//等级
				Config[ConfigName]["guanzhan"] = GameData.Config.ESP.观战;//观战
				Config[ConfigName]["Skeleton"] = GameData.Config.ESP.Skeleton;//骨骼顯示
				Config[ConfigName]["DisplayFrame"] = GameData.Config.ESP.DisplayFrame;//骨骼顯示
				Config[ConfigName]["TargetedRay"] = GameData.Config.ESP.TargetedRay;//骨骼顯示
				Config[ConfigName]["showIcon"] = GameData.Config.ESP.showIcon;
				Config[ConfigName]["DistanceMax"] = GameData.Config.ESP.DistanceMax;
				Config[ConfigName]["HeadDrawing"] = GameData.Config.ESP.HeadDrawing;
				Config[ConfigName]["SkeletonWidth"] = GameData.Config.ESP.SkeletonWidth;//骨骼粗细
				Config[ConfigName]["FontSize"] = GameData.Config.ESP.FontSize;//信息大小
				Config[ConfigName]["WeaponDistanceMax"] = GameData.Config.ESP.WeaponDistanceMax;//武器距离
				Config[ConfigName]["InfoDistanceMax"] = GameData.Config.ESP.InfoDistanceMax;//信息距离
				Config[ConfigName]["RayWidth"] = GameData.Config.ESP.RayWidth;
				Config[ConfigName]["FocusModeKey"] = GameData.Config.ESP.FocusModeKey;//战斗模式
				Config[ConfigName]["ConfigIndex"] = GameData.Config.AimBot.ConfigIndex;//
				Config[ConfigName]["duiyouKey"] = GameData.Config.ESP.duiyouKey;//
				Config[ConfigName]["DataSwitchkey"] = GameData.Config.ESP.DataSwitchkey;//
				Config[ConfigName]["FocusMode"] = GameData.Config.ESP.FocusMode;
				

				Config[ConfigName]["Color"]["Ray"]["Line"]["0"] = GameData.Config.ESP.Color.Ray.Line[0];
				Config[ConfigName]["Color"]["Ray"]["Line"]["1"] = GameData.Config.ESP.Color.Ray.Line[1];
				Config[ConfigName]["Color"]["Ray"]["Line"]["2"] = GameData.Config.ESP.Color.Ray.Line[2];
				Config[ConfigName]["Color"]["Ray"]["Line"]["3"] = GameData.Config.ESP.Color.Ray.Line[3];

				Config[ConfigName]["Color"]["Default"]["Info"]["0"] = GameData.Config.ESP.Color.Default.Info[0];
				Config[ConfigName]["Color"]["Default"]["Info"]["1"] = GameData.Config.ESP.Color.Default.Info[1];
				Config[ConfigName]["Color"]["Default"]["Info"]["2"] = GameData.Config.ESP.Color.Default.Info[2];
				Config[ConfigName]["Color"]["Default"]["Info"]["3"] = GameData.Config.ESP.Color.Default.Info[3];
				Config[ConfigName]["Color"]["Default"]["Skeleton"]["0"] = GameData.Config.ESP.Color.Default.Skeleton[0];
				Config[ConfigName]["Color"]["Default"]["Skeleton"]["1"] = GameData.Config.ESP.Color.Default.Skeleton[1];
				Config[ConfigName]["Color"]["Default"]["Skeleton"]["2"] = GameData.Config.ESP.Color.Default.Skeleton[2];
				Config[ConfigName]["Color"]["Default"]["Skeleton"]["3"] = GameData.Config.ESP.Color.Default.Skeleton[3];

				Config[ConfigName]["Color"]["Visible"]["Skeleton"]["0"] = GameData.Config.ESP.Color.Visible.Skeleton[0];
				Config[ConfigName]["Color"]["Visible"]["Skeleton"]["1"] = GameData.Config.ESP.Color.Visible.Skeleton[1];
				Config[ConfigName]["Color"]["Visible"]["Skeleton"]["2"] = GameData.Config.ESP.Color.Visible.Skeleton[2];
				Config[ConfigName]["Color"]["Visible"]["Skeleton"]["3"] = GameData.Config.ESP.Color.Visible.Skeleton[3];
				Config[ConfigName]["Color"]["Visible"]["Info"]["0"] = GameData.Config.ESP.Color.Visible.Info[0];
				Config[ConfigName]["Color"]["Visible"]["Info"]["1"] = GameData.Config.ESP.Color.Visible.Info[1];
				Config[ConfigName]["Color"]["Visible"]["Info"]["2"] = GameData.Config.ESP.Color.Visible.Info[2];
				Config[ConfigName]["Color"]["Visible"]["Info"]["3"] = GameData.Config.ESP.Color.Visible.Info[3];

				Config[ConfigName]["Color"]["Partner"]["Skeleton"]["0"] = GameData.Config.ESP.Color.Partner.Skeleton[0];
				Config[ConfigName]["Color"]["Partner"]["Skeleton"]["1"] = GameData.Config.ESP.Color.Partner.Skeleton[1];
				Config[ConfigName]["Color"]["Partner"]["Skeleton"]["2"] = GameData.Config.ESP.Color.Partner.Skeleton[2];
				Config[ConfigName]["Color"]["Partner"]["Skeleton"]["3"] = GameData.Config.ESP.Color.Partner.Skeleton[3];
				Config[ConfigName]["Color"]["Partner"]["Info"]["0"] = GameData.Config.ESP.Color.Partner.Info[0];
				Config[ConfigName]["Color"]["Partner"]["Info"]["1"] = GameData.Config.ESP.Color.Partner.Info[1];
				Config[ConfigName]["Color"]["Partner"]["Info"]["2"] = GameData.Config.ESP.Color.Partner.Info[2];
				Config[ConfigName]["Color"]["Partner"]["Info"]["3"] = GameData.Config.ESP.Color.Partner.Info[3];

				Config[ConfigName]["Color"]["Groggy"]["Skeleton"]["0"] = GameData.Config.ESP.Color.Groggy.Skeleton[0];
				Config[ConfigName]["Color"]["Groggy"]["Skeleton"]["1"] = GameData.Config.ESP.Color.Groggy.Skeleton[1];
				Config[ConfigName]["Color"]["Groggy"]["Skeleton"]["2"] = GameData.Config.ESP.Color.Groggy.Skeleton[2];
				Config[ConfigName]["Color"]["Groggy"]["Skeleton"]["3"] = GameData.Config.ESP.Color.Groggy.Skeleton[3];
				Config[ConfigName]["Color"]["Groggy"]["Info"]["0"] = GameData.Config.ESP.Color.Groggy.Info[0];
				Config[ConfigName]["Color"]["Groggy"]["Info"]["1"] = GameData.Config.ESP.Color.Groggy.Info[1];
				Config[ConfigName]["Color"]["Groggy"]["Info"]["2"] = GameData.Config.ESP.Color.Groggy.Info[2];
				Config[ConfigName]["Color"]["Groggy"]["Info"]["3"] = GameData.Config.ESP.Color.Groggy.Info[3];

				Config[ConfigName]["Color"]["Dangerous"]["Skeleton"]["0"] = GameData.Config.ESP.Color.Dangerous.Skeleton[0];
				Config[ConfigName]["Color"]["Dangerous"]["Skeleton"]["1"] = GameData.Config.ESP.Color.Dangerous.Skeleton[1];
				Config[ConfigName]["Color"]["Dangerous"]["Skeleton"]["2"] = GameData.Config.ESP.Color.Dangerous.Skeleton[2];
				Config[ConfigName]["Color"]["Dangerous"]["Skeleton"]["3"] = GameData.Config.ESP.Color.Dangerous.Skeleton[3];
				Config[ConfigName]["Color"]["Dangerous"]["Info"]["0"] = GameData.Config.ESP.Color.Dangerous.Info[0];
				Config[ConfigName]["Color"]["Dangerous"]["Info"]["1"] = GameData.Config.ESP.Color.Dangerous.Info[1];
				Config[ConfigName]["Color"]["Dangerous"]["Info"]["2"] = GameData.Config.ESP.Color.Dangerous.Info[2];
				Config[ConfigName]["Color"]["Dangerous"]["Info"]["3"] = GameData.Config.ESP.Color.Dangerous.Info[3];

				Config[ConfigName]["Color"]["Blacklist"]["Skeleton"]["0"] = GameData.Config.ESP.Color.Blacklist.Skeleton[0];
				Config[ConfigName]["Color"]["Blacklist"]["Skeleton"]["1"] = GameData.Config.ESP.Color.Blacklist.Skeleton[1];
				Config[ConfigName]["Color"]["Blacklist"]["Skeleton"]["2"] = GameData.Config.ESP.Color.Blacklist.Skeleton[2];
				Config[ConfigName]["Color"]["Blacklist"]["Skeleton"]["3"] = GameData.Config.ESP.Color.Blacklist.Skeleton[3];
				Config[ConfigName]["Color"]["Blacklist"]["Info"]["0"] = GameData.Config.ESP.Color.Blacklist.Info[0];
				Config[ConfigName]["Color"]["Blacklist"]["Info"]["1"] = GameData.Config.ESP.Color.Blacklist.Info[1];
				Config[ConfigName]["Color"]["Blacklist"]["Info"]["2"] = GameData.Config.ESP.Color.Blacklist.Info[2];
				Config[ConfigName]["Color"]["Blacklist"]["Info"]["3"] = GameData.Config.ESP.Color.Blacklist.Info[3];

				Config[ConfigName]["Color"]["Whitelist"]["Skeleton"]["0"] = GameData.Config.ESP.Color.Whitelist.Skeleton[0];
				Config[ConfigName]["Color"]["Whitelist"]["Skeleton"]["1"] = GameData.Config.ESP.Color.Whitelist.Skeleton[1];
				Config[ConfigName]["Color"]["Whitelist"]["Skeleton"]["2"] = GameData.Config.ESP.Color.Whitelist.Skeleton[2];
				Config[ConfigName]["Color"]["Whitelist"]["Skeleton"]["3"] = GameData.Config.ESP.Color.Whitelist.Skeleton[3];
				Config[ConfigName]["Color"]["Whitelist"]["Info"]["0"] = GameData.Config.ESP.Color.Whitelist.Info[0];
				Config[ConfigName]["Color"]["Whitelist"]["Info"]["1"] = GameData.Config.ESP.Color.Whitelist.Info[1];
				Config[ConfigName]["Color"]["Whitelist"]["Info"]["2"] = GameData.Config.ESP.Color.Whitelist.Info[2];
				Config[ConfigName]["Color"]["Whitelist"]["Info"]["3"] = GameData.Config.ESP.Color.Whitelist.Info[3];

				Config[ConfigName]["Color"]["AI"]["Skeleton"]["0"] = GameData.Config.ESP.Color.AI.Skeleton[0];
				Config[ConfigName]["Color"]["AI"]["Skeleton"]["1"] = GameData.Config.ESP.Color.AI.Skeleton[1];
				Config[ConfigName]["Color"]["AI"]["Skeleton"]["2"] = GameData.Config.ESP.Color.AI.Skeleton[2];
				Config[ConfigName]["Color"]["AI"]["Skeleton"]["3"] = GameData.Config.ESP.Color.AI.Skeleton[3];
				Config[ConfigName]["Color"]["AI"]["Info"]["0"] = GameData.Config.ESP.Color.AI.Info[0];
				Config[ConfigName]["Color"]["AI"]["Info"]["1"] = GameData.Config.ESP.Color.AI.Info[1];
				Config[ConfigName]["Color"]["AI"]["Info"]["2"] = GameData.Config.ESP.Color.AI.Info[2];
				Config[ConfigName]["Color"]["AI"]["Info"]["3"] = GameData.Config.ESP.Color.AI.Info[3];
		}();

		[&] {
				std::string ConfigName = "AimBot";
				Config[ConfigName]["Enable"] = GameData.Config.AimBot.Enable;

				Config[ConfigName]["qjyaqiang"] = GameData.Config.AimBot.qjyaqiang;
				Config[ConfigName]["wybYSpeed"] = GameData.Config.AimBot.wybYSpeed;

				Config[ConfigName]["COM"] = GameData.Config.AimBot.COM;
				Config[ConfigName]["ConfigIndex"] = GameData.Config.AimBot.ConfigIndex;
				Config[ConfigName]["Controller"] = GameData.Config.AimBot.Controller;
				Config[ConfigName]["IP"] = GameData.Config.AimBot.IP;
				Config[ConfigName]["Port"] = GameData.Config.AimBot.Port;
				Config[ConfigName]["UUID"] = GameData.Config.AimBot.UUID;
				Config[ConfigName]["ShowFOV"] = GameData.Config.AimBot.ShowFOV;
				Config[ConfigName]["ShowWheelFOV"] = GameData.Config.AimBot.ShowWheelFOV;
				Config[ConfigName]["ShowPoint"] = GameData.Config.AimBot.ShowPoint;
				Config[ConfigName]["PointSize"] = GameData.Config.AimBot.PointSize;
				
				Config[ConfigName]["FOVColor"]["0"] = GameData.Config.AimBot.FOVColor[0];
				Config[ConfigName]["FOVColor"]["1"] = GameData.Config.AimBot.FOVColor[1];
				Config[ConfigName]["FOVColor"]["2"] = GameData.Config.AimBot.FOVColor[2];
				Config[ConfigName]["FOVColor"]["3"] = GameData.Config.AimBot.FOVColor[3];

				Config[ConfigName]["WheelFOVColor"]["0"] = GameData.Config.AimBot.WheelFOVColor[0];
				Config[ConfigName]["WheelFOVColor"]["1"] = GameData.Config.AimBot.WheelFOVColor[1];
				Config[ConfigName]["WheelFOVColor"]["2"] = GameData.Config.AimBot.WheelFOVColor[2];
				Config[ConfigName]["WheelFOVColor"]["3"] = GameData.Config.AimBot.WheelFOVColor[3];

				Config[ConfigName]["PointColor"]["0"] = GameData.Config.AimBot.PointColor[0];
				Config[ConfigName]["PointColor"]["1"] = GameData.Config.AimBot.PointColor[1];
				Config[ConfigName]["PointColor"]["2"] = GameData.Config.AimBot.PointColor[2];
				Config[ConfigName]["PointColor"]["3"] = GameData.Config.AimBot.PointColor[3];

				for (auto& Item : GameData.Config.AimBot.Configs)
				{
					std::string Index = std::to_string(Item.first);
					Config[ConfigName]["Configs"][Index]["Key"] = Item.second.Key;

					for (auto& Weapon : Item.second.Weapon)
					{
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["VisibleCheck"] = Weapon.second.VisibleCheck;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["AutoSwitch"] = Weapon.second.AutoSwitch;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["AimMode"] = Weapon.second.AimMode;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["enable"] = Weapon.second.enable;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["LockMode"] = Weapon.second.LockMode;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["Threshold"] = Weapon.second.Threshold;
					
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["FOVenable"] = Weapon.second.FOVenable;


						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["Delay1"] = Weapon.second.Delay1;

						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["banjiAimDistance"] = Weapon.second.banjiAimDistance;


						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["HotkeyMerge"] = Weapon.second.HotkeyMerge;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["AimWheel"] = Weapon.second.AimWheel;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["NoRecoil"] = Weapon.second.NoRecoil;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["bIsScoping_CP"] = Weapon.second.bIsScoping_CP;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["OriginalRecoil"] = Weapon.second.OriginalRecoil;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["Prediction"] = Weapon.second.Prediction;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["AimAndShot"] = Weapon.second.AimAndShot;

						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["DynamicFov"] = Weapon.second.DynamicFov;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["InitialValue"] = Weapon.second.InitialValue;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["FOV"] = Weapon.second.FOV;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["WheelFOV"] = Weapon.second.WheelFOV;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["SwitchingDelay"] = Weapon.second.SwitchingDelay;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["AimDistance"] = Weapon.second.AimDistance;

						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["AimPre_X"] = Weapon.second.AimPre_X;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["AimPre_Y"] = Weapon.second.AimPre_Y;

						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["XXSpeed"] = Weapon.second.XXSpeed;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["YYSpeed"] = Weapon.second.YYSpeed;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["RecoilTime"] = Weapon.second.RecoilTime;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["XSpeed"] = Weapon.second.XSpeed;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["YSpeed"] = Weapon.second.YSpeed;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["AimWheelSpeed"] = Weapon.second.AimWheelSpeed;
					
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["FPS"] = Weapon.second.FPS;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["AimSpeedMaxFactor"] = Weapon.second.AimSpeedMaxFactor;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["IgnoreGroggy"] = Weapon.second.IgnoreGroggy;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["LineTraceSingle"] = Weapon.second.LineTraceSingle;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["AimWheelBone"] = Weapon.second.AimWheelBone;
						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["PredictionMode"] = Weapon.second.PredictionMode;

						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["First"]["Key"] = Weapon.second.First.Key;
						for (size_t i = 0; i < 17; i++)
						{
							Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["First"]["Bones"][std::to_string(i)] = Weapon.second.First.Bones[i];
						}

						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["Second"]["Key"] = Weapon.second.Second.Key;
						for (size_t i = 0; i < 17; i++)
						{
							Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["Second"]["Bones"][std::to_string(i)] = Weapon.second.Second.Bones[i];
						}

						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["Groggy"]["Key"] = Weapon.second.Groggy.Key;
						for (size_t i = 0; i < 17; i++)
						{
							Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["Groggy"]["Bones"][std::to_string(i)] = Weapon.second.Groggy.Bones[i];
						}

						Config[ConfigName]["Configs"][Index]["Weapon"][Weapon.first]["Wheel"]["Key"] = Weapon.second.Wheel.Key;
					}
				}
		}();

		[&] {
				std::string ConfigName = "Overlay";
				Config[ConfigName]["VSync"] = GameData.Config.Overlay.VSync;
				Config[ConfigName]["zhixiangmoshi"] = GameData.Config.Overlay.zhixiangmoshi;

				Config[ConfigName]["Enable"] = GameData.Config.Overlay.Enable;
				Config[ConfigName]["UseLastFrameCameraCache"] = GameData.Config.Overlay.UseLastFrameCameraCache;
				Config[ConfigName]["UseThread"] = GameData.Config.Overlay.UseThread;
				Config[ConfigName]["SafeExit"] = GameData.Config.Overlay.Quit_key;
				Config[ConfigName]["ShowMenu"] = GameData.Config.Menu.ShowKey;
				Config[ConfigName]["rankList"] = GameData.Config.Overlay.rankList;
				Config[ConfigName]["FusionModeKey"] = GameData.Config.Overlay.FusionModeKey;
        }();

				Utils::WriteConfigFile("Config/AKMConfig.bak", Config.dump());
				return true;
	};

	static void Load()
	{
		std::string ConfigText = Utils::ReadConfigFile("Config/AKMConfig.bak");

		if (ConfigText == "")
		{
			return;
		}

		std::string Text = Utils::ReadConfigFile("Config/AKMConfig.bak");

		auto Config = nlohmann::json::parse(Text);

		[&] {
			std::string ConfigName = "AimBot";
			SetConfigItem(Config, { ConfigName, "Enable" }, GameData.Config.AimBot.Enable);
			SetConfigItem(Config, { ConfigName, "COM" }, GameData.Config.AimBot.COM);
			SetConfigItem(Config, { ConfigName, "ConfigIndex" }, GameData.Config.AimBot.ConfigIndex);
			SetConfigItem(Config, { ConfigName, "Controller" }, GameData.Config.AimBot.Controller);
			SetConfigItem(Config, { ConfigName, "qjyaqiang" }, GameData.Config.AimBot.qjyaqiang);
			SetConfigItem(Config, { ConfigName, "wybYSpeed" }, GameData.Config.AimBot.wybYSpeed);
		

			if (Config[ConfigName].count("IP"))
			{
				snprintf(GameData.Config.AimBot.IP, sizeof(GameData.Config.AimBot.IP), "%s", Utils::StringToUTF8(Config[ConfigName]["IP"]));
			}

			if (Config[ConfigName].count("Port"))
			{
				snprintf(GameData.Config.AimBot.Port, sizeof(GameData.Config.AimBot.Port), "%s", Utils::StringToUTF8(Config[ConfigName]["Port"]));
			}

			if (Config[ConfigName].count("UUID"))
			{
				snprintf(GameData.Config.AimBot.UUID, sizeof(GameData.Config.AimBot.UUID), "%s", Utils::StringToUTF8(Config[ConfigName]["UUID"]));
			}
			
			SetConfigItem(Config, { ConfigName, "ShowFOV" }, GameData.Config.AimBot.ShowFOV);
			SetConfigItem(Config, { ConfigName, "ShowWheelFOV" }, GameData.Config.AimBot.ShowWheelFOV);
			SetConfigItem(Config, { ConfigName, "ShowPoint" }, GameData.Config.AimBot.ShowPoint);
			SetConfigItem(Config, { ConfigName, "PointSize" }, GameData.Config.AimBot.PointSize);
			SetConfigItem(Config, { ConfigName, "FOVColor", "0" }, GameData.Config.AimBot.FOVColor[0]);
			SetConfigItem(Config, { ConfigName, "FOVColor", "1" }, GameData.Config.AimBot.FOVColor[1]);
			SetConfigItem(Config, { ConfigName, "FOVColor", "2" }, GameData.Config.AimBot.FOVColor[2]);
			SetConfigItem(Config, { ConfigName, "FOVColor", "3" }, GameData.Config.AimBot.FOVColor[3]);
			SetConfigItem(Config, { ConfigName, "WheelFOVColor", "0" }, GameData.Config.AimBot.WheelFOVColor[0]);
			SetConfigItem(Config, { ConfigName, "WheelFOVColor", "1" }, GameData.Config.AimBot.WheelFOVColor[1]);
			SetConfigItem(Config, { ConfigName, "WheelFOVColor", "2" }, GameData.Config.AimBot.WheelFOVColor[2]);
			SetConfigItem(Config, { ConfigName, "WheelFOVColor", "3" }, GameData.Config.AimBot.WheelFOVColor[3]);
			SetConfigItem(Config, { ConfigName, "PointColor", "0" }, GameData.Config.AimBot.PointColor[0]);
			SetConfigItem(Config, { ConfigName, "PointColor", "1" }, GameData.Config.AimBot.PointColor[1]);
			SetConfigItem(Config, { ConfigName, "PointColor", "2" }, GameData.Config.AimBot.PointColor[2]);
			SetConfigItem(Config, { ConfigName, "PointColor", "3" }, GameData.Config.AimBot.PointColor[3]);

			for (auto& Item : GameData.Config.AimBot.Configs)
			{
				std::string Index = std::to_string(Item.first);
				SetConfigItem(Config, { ConfigName, "Configs", Index, "Key" }, Item.second.Key);

				for (auto& Weapon : Item.second.Weapon)
				{
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "VisibleCheck" }, Weapon.second.VisibleCheck);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "Threshold" }, Weapon.second.Threshold);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "AimMode" }, Weapon.second.AimMode);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "AutoSwitch" }, Weapon.second.AutoSwitch);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "NoBulletNotAim" }, Weapon.second.NoBulletNotAim);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "FOVenable" }, Weapon.second.FOVenable);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "enable" }, Weapon.second.enable);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "LockMode" }, Weapon.second.LockMode);
					
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "Delay1" }, Weapon.second.Delay1);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "banjiAimDistance" }, Weapon.second.banjiAimDistance);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "HotkeyMerge" }, Weapon.second.HotkeyMerge);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "AimWheel" }, Weapon.second.AimWheel);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "NoRecoil" }, Weapon.second.NoRecoil);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "bIsScoping_CP" }, Weapon.second.bIsScoping_CP);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "OriginalRecoil" }, Weapon.second.OriginalRecoil);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "Prediction" }, Weapon.second.Prediction);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "AimAndShot" }, Weapon.second.AimAndShot);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "DynamicFov" }, Weapon.second.DynamicFov);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "InitialValue" }, Weapon.second.InitialValue);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "FOV" }, Weapon.second.FOV);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "WheelFOV" }, Weapon.second.WheelFOV);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "SwitchingDelay" }, Weapon.second.SwitchingDelay);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "AimDistance" }, Weapon.second.AimDistance);

					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "AimPre_X" }, Weapon.second.AimPre_X);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "AimPre_Y" }, Weapon.second.AimPre_Y);

					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "XXSpeed" }, Weapon.second.XXSpeed);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "YYSpeed" }, Weapon.second.YYSpeed);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "RecoilTime" }, Weapon.second.RecoilTime);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "XSpeed" }, Weapon.second.XSpeed);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "YSpeed" }, Weapon.second.YSpeed);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "AimWheelSpeed" }, Weapon.second.AimWheelSpeed);
				
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "FPS" }, Weapon.second.FPS);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "AimSpeedMaxFactor" }, Weapon.second.AimSpeedMaxFactor);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "IgnoreGroggy" }, Weapon.second.IgnoreGroggy);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "LineTraceSingle" }, Weapon.second.LineTraceSingle);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "AimWheelBone" }, Weapon.second.AimWheelBone);
					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "PredictionMode" }, Weapon.second.PredictionMode);

					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "First", "Key" }, Weapon.second.First.Key);
					for (size_t i = 0; i < 17; i++)
					{
						SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "First", "Bones", std::to_string(i) }, Weapon.second.First.Bones[i]);
					}

					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "Second", "Key" }, Weapon.second.Second.Key);
					for (size_t i = 0; i < 17; i++)
					{
						SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "Second", "Bones", std::to_string(i) }, Weapon.second.Second.Bones[i]);
					}

					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "Groggy", "Key" }, Weapon.second.Groggy.Key);
					for (size_t i = 0; i < 17; i++)
					{
						SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "Groggy", "Bones", std::to_string(i) }, Weapon.second.Groggy.Bones[i]);
					}

					SetConfigItem(Config, { ConfigName, "Configs", Index, "Weapon", Weapon.first, "Wheel", "Key" }, Weapon.second.Wheel.Key);
				}
			}
			}();

		[&] {
			std::string ConfigName = "Overlay";
			SetConfigItem(Config, { ConfigName, "zhixiangmoshi" }, GameData.Config.Overlay.zhixiangmoshi);
			SetConfigItem(Config, { ConfigName, "VSync" }, GameData.Config.Overlay.VSync);
			SetConfigItem(Config, { ConfigName, "Enable" }, GameData.Config.Overlay.Enable);
			SetConfigItem(Config, { ConfigName, "UseLastFrameCameraCache" }, GameData.Config.Overlay.UseLastFrameCameraCache);
			SetConfigItem(Config, { ConfigName, "UseThread" }, GameData.Config.Overlay.UseThread);
			SetConfigItem(Config, { ConfigName, "rankList" }, GameData.Config.Overlay.rankList);
		}();

		[&] {
			std::string ConfigName = "Function";
			SetConfigItem(Config, { ConfigName, "ClearKey" }, GameData.Config.Function.ClearKey);
		}();

		[&] {
			std::string ConfigName = "Vehicle";
			SetConfigItem(Config, { ConfigName, "Enable" }, GameData.Config.Vehicle.Enable);
			SetConfigItem(Config, { ConfigName, "Health" }, GameData.Config.Vehicle.Health);
			SetConfigItem(Config, { ConfigName, "Durability" }, GameData.Config.Vehicle.Durability);
			SetConfigItem(Config, { ConfigName, "EnableKey" }, GameData.Config.Vehicle.EnableKey);
			SetConfigItem(Config, { ConfigName, "DistanceMax" }, GameData.Config.Vehicle.DistanceMax);
			SetConfigItem(Config, { ConfigName, "FontSize" }, GameData.Config.Vehicle.FontSize);

			SetConfigItem(Config, { ConfigName, "Color", "0" }, GameData.Config.Vehicle.Color[0]);
			SetConfigItem(Config, { ConfigName, "Color", "1" }, GameData.Config.Vehicle.Color[1]);
			SetConfigItem(Config, { ConfigName, "Color", "2" }, GameData.Config.Vehicle.Color[2]);
			SetConfigItem(Config, { ConfigName, "Color", "3" }, GameData.Config.Vehicle.Color[3]);
			}();

		[&] {
			std::string ConfigName = "AirDrop";
			SetConfigItem(Config, { ConfigName, "Enable" }, GameData.Config.AirDrop.Enable);
			SetConfigItem(Config, { ConfigName, "EnableKey" }, GameData.Config.AirDrop.EnableKey);
			SetConfigItem(Config, { ConfigName, "DistanceMax" }, GameData.Config.AirDrop.DistanceMax);
			SetConfigItem(Config, { ConfigName, "FontSize" }, GameData.Config.AirDrop.FontSize);
			SetConfigItem(Config, { ConfigName, "ShowItems" }, GameData.Config.AirDrop.ShowItems);

			SetConfigItem(Config, { ConfigName, "Color", "0" }, GameData.Config.AirDrop.Color[0]);
			SetConfigItem(Config, { ConfigName, "Color", "1" }, GameData.Config.AirDrop.Color[1]);
			SetConfigItem(Config, { ConfigName, "Color", "2" }, GameData.Config.AirDrop.Color[2]);
			SetConfigItem(Config, { ConfigName, "Color", "3" }, GameData.Config.AirDrop.Color[3]);
			}();

		[&] {
			std::string ConfigName = "DeadBox";
			SetConfigItem(Config, { ConfigName, "Enable" }, GameData.Config.DeadBox.Enable);
			
			SetConfigItem(Config, { ConfigName, "EnableKey" }, GameData.Config.DeadBox.EnableKey);
			SetConfigItem(Config, { ConfigName, "DistanceMax" }, GameData.Config.DeadBox.DistanceMax);
			SetConfigItem(Config, { ConfigName, "FontSize" }, GameData.Config.DeadBox.FontSize);
			SetConfigItem(Config, { ConfigName, "ShowItems" }, GameData.Config.DeadBox.ShowItems);

			SetConfigItem(Config, { ConfigName, "Color", "0" }, GameData.Config.DeadBox.Color[0]);
			SetConfigItem(Config, { ConfigName, "Color", "1" }, GameData.Config.DeadBox.Color[1]);
			SetConfigItem(Config, { ConfigName, "Color", "2" }, GameData.Config.DeadBox.Color[2]);
			SetConfigItem(Config, { ConfigName, "Color", "3" }, GameData.Config.DeadBox.Color[3]);
			}();

		[&] {
			std::string ConfigName = "Early";
			SetConfigItem(Config, { ConfigName, "Enable" }, GameData.Config.Early.Enable);
			SetConfigItem(Config, { ConfigName, "DistanceMax" }, GameData.Config.Early.DistanceMax);
			SetConfigItem(Config, { ConfigName, "FontSize" }, GameData.Config.Early.FontSize);
			SetConfigItem(Config, { ConfigName, "ShowDistance" }, GameData.Config.Early.ShowDistance);
			}();

		[&] {
			std::string ConfigName = "PlayerList";
			SetConfigItem(Config, { ConfigName, "RankMode" }, GameData.Config.PlayerList.RankMode);
			}();

		[&] {
			std::string ConfigName = "Item";
			SetConfigItem(Config, { ConfigName, "Enable" }, GameData.Config.Item.Enable);

			SetConfigItem(Config, { ConfigName, "FilterEnable" }, GameData.Config.Item.FilterEnable);
			SetConfigItem(Config, { ConfigName, "FirstAidKit" }, GameData.Config.Item.FirstAidKit);
			SetConfigItem(Config, { ConfigName, "PainKiller" }, GameData.Config.Item.PainKiller);
			SetConfigItem(Config, { ConfigName, "EnergyDrink" }, GameData.Config.Item.EnergyDrink);
			SetConfigItem(Config, { ConfigName, "Bandage" }, GameData.Config.Item.Bandage);
			SetConfigItem(Config, { ConfigName, "Epinephrine" }, GameData.Config.Item.Epinephrine);
			SetConfigItem(Config, { ConfigName, "Medicalkit" }, GameData.Config.Item.Medicalkit);
			SetConfigItem(Config, { ConfigName, "Grenade" }, GameData.Config.Item.Grenade);

			SetConfigItem(Config, { ConfigName, "FlashGrenade" }, GameData.Config.Item.FlashGrenade);
			SetConfigItem(Config, { ConfigName, "SmokeGrenade" }, GameData.Config.Item.SmokeGrenade);
			SetConfigItem(Config, { ConfigName, "MolotovGrenade" }, GameData.Config.Item.MolotovGrenade);
			SetConfigItem(Config, { ConfigName, "BluezoneGrenade" }, GameData.Config.Item.BluezoneGrenade);


			SetConfigItem(Config, { ConfigName, "EnableKey" }, GameData.Config.Item.EnableKey);
			//SetConfigItem(Config, { ConfigName, "GroupKey" }, GameData.Config.Item.GroupKey);
			SetConfigItem(Config, { ConfigName, "DistanceMax" }, GameData.Config.Item.DistanceMax);
			SetConfigItem(Config, { ConfigName, "FontSize" }, GameData.Config.Item.FontSize);
			SetConfigItem(Config, { ConfigName, "Combination" }, GameData.Config.Item.Combination);
			SetConfigItem(Config, { ConfigName, "ShowIcon" }, GameData.Config.Item.ShowIcon);
			SetConfigItem(Config, { ConfigName, "ShowDistance" }, GameData.Config.Item.ShowDistance);
			SetConfigItem(Config, { ConfigName, "AccessoriesFilter" }, GameData.Config.Item.AccessoriesFilter);
			SetConfigItem(Config, { ConfigName, "ThresholdX" }, GameData.Config.Item.ThresholdX);
			SetConfigItem(Config, { ConfigName, "ThresholdY" }, GameData.Config.Item.ThresholdY);
			SetConfigItem(Config, { ConfigName, "GroupAKey" }, GameData.Config.Item.GroupAKey);
			SetConfigItem(Config, { ConfigName, "GroupBKey" }, GameData.Config.Item.GroupBKey);
			SetConfigItem(Config, { ConfigName, "GroupCKey" }, GameData.Config.Item.GroupCKey);
			SetConfigItem(Config, { ConfigName, "GroupDKey" }, GameData.Config.Item.GroupDKey);
			SetConfigItem(Config, { ConfigName, "ShowGroup" }, GameData.Config.Item.ShowGroup);

			SetConfigItem(Config, { ConfigName, "GroupAColor", "0" }, GameData.Config.Item.GroupAColor[0]);
			SetConfigItem(Config, { ConfigName, "GroupAColor", "1" }, GameData.Config.Item.GroupAColor[1]);
			SetConfigItem(Config, { ConfigName, "GroupAColor", "2" }, GameData.Config.Item.GroupAColor[2]);
			SetConfigItem(Config, { ConfigName, "GroupAColor", "3" }, GameData.Config.Item.GroupAColor[3]);

			SetConfigItem(Config, { ConfigName, "GroupBColor", "0" }, GameData.Config.Item.GroupBColor[0]);
			SetConfigItem(Config, { ConfigName, "GroupBColor", "1" }, GameData.Config.Item.GroupBColor[1]);
			SetConfigItem(Config, { ConfigName, "GroupBColor", "2" }, GameData.Config.Item.GroupBColor[2]);
			SetConfigItem(Config, { ConfigName, "GroupBColor", "3" }, GameData.Config.Item.GroupBColor[3]);

			SetConfigItem(Config, { ConfigName, "GroupCColor", "0" }, GameData.Config.Item.GroupCColor[0]);
			SetConfigItem(Config, { ConfigName, "GroupCColor", "1" }, GameData.Config.Item.GroupCColor[1]);
			SetConfigItem(Config, { ConfigName, "GroupCColor", "2" }, GameData.Config.Item.GroupCColor[2]);
			SetConfigItem(Config, { ConfigName, "GroupCColor", "3" }, GameData.Config.Item.GroupCColor[3]);

			SetConfigItem(Config, { ConfigName, "GroupDColor", "0" }, GameData.Config.Item.GroupDColor[0]);
			SetConfigItem(Config, { ConfigName, "GroupDColor", "1" }, GameData.Config.Item.GroupDColor[1]);
			SetConfigItem(Config, { ConfigName, "GroupDColor", "2" }, GameData.Config.Item.GroupDColor[2]);
			SetConfigItem(Config, { ConfigName, "GroupDColor", "3" }, GameData.Config.Item.GroupDColor[3]);

			for (auto& pair : GameData.Config.Item.Lists) {
				const std::string& key = pair.first;
				ItemDetail& detail = pair.second;
				SetConfigItem(Config, { ConfigName, "Lists", key, "Group" }, detail.Group);
			}
			}();

		[&] {
			std::string ConfigName = "ESP";
			if (Config[ConfigName].count("Server IP"))
			{
				snprintf(GameData.Config.ESP.服务器IP, sizeof(GameData.Config.ESP.服务器IP), "%s", Utils::StringToUTF8(Config[ConfigName]["Server IP"]));
			}
			SetConfigItem(Config, { ConfigName, "XueTiaoWEizhi" }, GameData.Config.ESP.XueTiaoWEizhi);
			SetConfigItem(Config, { ConfigName, "PhysXDebug" }, GameData.Config.ESP.PhysXDebug);
			SetConfigItem(Config, { ConfigName, "DangerWarning" }, GameData.Config.ESP.DangerWarning);
			SetConfigItem(Config, { ConfigName, "AdjustableDistance" }, GameData.Config.ESP.AdjustableDistance);
			SetConfigItem(Config, { ConfigName, "PhysxLoadRadius" }, GameData.Config.ESP.PhysxLoadRadius);
			SetConfigItem(Config, { ConfigName, "PhysxStaticRefreshInterval" }, GameData.Config.ESP.PhysxStaticRefreshInterval);
			SetConfigItem(Config, { ConfigName, "PhysxDynamicRefreshInterval" }, GameData.Config.ESP.PhysxDynamicRefreshInterval);
			SetConfigItem(Config, { ConfigName, "PhysxRefreshLimit" }, GameData.Config.ESP.PhysxRefreshLimit);
			SetConfigItem(Config, { ConfigName, "miwu" }, GameData.Config.ESP.miwu);
			SetConfigItem(Config, { ConfigName, "Enable" }, GameData.Config.ESP.Enable);
			SetConfigItem(Config, { ConfigName, "Stroke" }, GameData.Config.ESP.Stroke);
			SetConfigItem(Config, { ConfigName, "DistanceStyle" }, GameData.Config.ESP.DistanceStyle);
			SetConfigItem(Config, { ConfigName, "HealthBarStyle" }, GameData.Config.ESP.HealthBarStyle);
			SetConfigItem(Config, { ConfigName, "VisibleCheck" }, GameData.Config.ESP.VisibleCheck);
			SetConfigItem(Config, { ConfigName, "AimExpandInfo" }, GameData.Config.ESP.AimExpandInfo);
			SetConfigItem(Config, { ConfigName, "TargetedRay" }, GameData.Config.ESP.TargetedRay);
			SetConfigItem(Config, { ConfigName, "VisibleCheckRay" }, GameData.Config.ESP.VisibleCheckRay);
			SetConfigItem(Config, { ConfigName, "LockedHiddenBones" }, GameData.Config.ESP.LockedHiddenBones);
			SetConfigItem(Config, { ConfigName, "shouchi" }, GameData.Config.ESP.Weapon);
			SetConfigItem(Config, { ConfigName, "Partner" }, GameData.Config.ESP.Partner);

			SetConfigItem(Config, { ConfigName, "shanghai" }, GameData.Config.ESP.伤害);
			SetConfigItem(Config, { ConfigName, "diaoxian" }, GameData.Config.ESP.ShowInfos);
			SetConfigItem(Config, { ConfigName, "jisha" }, GameData.Config.ESP.击杀);
			SetConfigItem(Config, { ConfigName, "RankPoint" }, GameData.Config.ESP.RankPoint);
			SetConfigItem(Config, { ConfigName, "KDA" }, GameData.Config.ESP.KDA);
			SetConfigItem(Config, { ConfigName, "dengji" }, GameData.Config.ESP.等级);
			SetConfigItem(Config, { ConfigName, "guanzhan" }, GameData.Config.ESP.观战);
			SetConfigItem(Config, { ConfigName, "Skeleton" }, GameData.Config.ESP.Skeleton);
			SetConfigItem(Config, { ConfigName, "showIcon" }, GameData.Config.ESP.showIcon);
			/*for (size_t i = 0; i < 17; i++)
			{
				SetConfigItem(Config, { ConfigName, "ShowInfos", std::to_string(i) }, GameData.Config.ESP.ShowInfos[i]);
			}*/
			SetConfigItem(Config, { ConfigName, "Nickname" }, GameData.Config.ESP.Nickname);
			SetConfigItem(Config, { ConfigName, "TeamID" }, GameData.Config.ESP.TeamID);
			SetConfigItem(Config, { ConfigName, "ClanName" }, GameData.Config.ESP.ClanName);
			SetConfigItem(Config, { ConfigName, "Dis" }, GameData.Config.ESP.Dis);
			SetConfigItem(Config, { ConfigName, "health_bar" }, GameData.Config.ESP.health_bar);
			SetConfigItem(Config, { ConfigName, "Health" }, GameData.Config.ESP.Health);
			SetConfigItem(Config, { ConfigName, "TeamID" }, GameData.Config.ESP.TeamID);
			SetConfigItem(Config, { ConfigName, "Tier" }, GameData.Config.ESP.Tier);
			SetConfigItem(Config, { ConfigName, "DisplayFrame" }, GameData.Config.ESP.DisplayFrame);
			SetConfigItem(Config, { ConfigName, "HeadDrawing" }, GameData.Config.ESP.HeadDrawing);
			SetConfigItem(Config, { ConfigName, "DistanceMax" }, GameData.Config.ESP.DistanceMax);
		
			SetConfigItem(Config, { ConfigName, "WeaponDistanceMax" }, GameData.Config.ESP.WeaponDistanceMax);

			SetConfigItem(Config, { ConfigName, "SkeletonWidth" }, GameData.Config.ESP.SkeletonWidth);
			SetConfigItem(Config, { ConfigName, "FontSize" }, GameData.Config.ESP.FontSize);
			SetConfigItem(Config, { ConfigName, "WeaponDistanceMax" }, GameData.Config.ESP.WeaponDistanceMax);
			SetConfigItem(Config, { ConfigName, "InfoDistanceMax" }, GameData.Config.ESP.InfoDistanceMax);
			SetConfigItem(Config, { ConfigName, "RayWidth" }, GameData.Config.ESP.RayWidth);
			SetConfigItem(Config, { ConfigName, "FocusModeKey" }, GameData.Config.ESP.FocusModeKey);
			SetConfigItem(Config, { ConfigName, "duiyouKey" }, GameData.Config.ESP.duiyouKey);
			SetConfigItem(Config, { ConfigName, "DataSwitchkey" }, GameData.Config.ESP.DataSwitchkey);
			SetConfigItem(Config, { ConfigName, "ConfigIndex" }, GameData.Config.AimBot.ConfigIndex);
			SetConfigItem(Config, { ConfigName, "FocusMode" }, GameData.Config.ESP.FocusMode);
			

			SetConfigItem(Config, { ConfigName, "Color", "Ray", "Line", "0" }, GameData.Config.ESP.Color.Ray.Line[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Ray", "Line", "1" }, GameData.Config.ESP.Color.Ray.Line[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Ray", "Line", "2" }, GameData.Config.ESP.Color.Ray.Line[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Ray", "Line", "3" }, GameData.Config.ESP.Color.Ray.Line[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Default", "Info", "0" }, GameData.Config.ESP.Color.Default.Info[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Default", "Info", "1" }, GameData.Config.ESP.Color.Default.Info[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Default", "Info", "2" }, GameData.Config.ESP.Color.Default.Info[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Default", "Info", "3" }, GameData.Config.ESP.Color.Default.Info[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Default", "Skeleton", "0" }, GameData.Config.ESP.Color.Default.Skeleton[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Default", "Skeleton", "1" }, GameData.Config.ESP.Color.Default.Skeleton[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Default", "Skeleton", "2" }, GameData.Config.ESP.Color.Default.Skeleton[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Default", "Skeleton", "3" }, GameData.Config.ESP.Color.Default.Skeleton[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Visible", "Skeleton", "0" }, GameData.Config.ESP.Color.Visible.Skeleton[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Visible", "Skeleton", "1" }, GameData.Config.ESP.Color.Visible.Skeleton[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Visible", "Skeleton", "2" }, GameData.Config.ESP.Color.Visible.Skeleton[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Visible", "Skeleton", "3" }, GameData.Config.ESP.Color.Visible.Skeleton[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Visible", "Info", "0" }, GameData.Config.ESP.Color.Visible.Info[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Visible", "Info", "1" }, GameData.Config.ESP.Color.Visible.Info[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Visible", "Info", "2" }, GameData.Config.ESP.Color.Visible.Info[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Visible", "Info", "3" }, GameData.Config.ESP.Color.Visible.Info[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Partner", "Skeleton", "0" }, GameData.Config.ESP.Color.Partner.Skeleton[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Partner", "Skeleton", "1" }, GameData.Config.ESP.Color.Partner.Skeleton[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Partner", "Skeleton", "2" }, GameData.Config.ESP.Color.Partner.Skeleton[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Partner", "Skeleton", "3" }, GameData.Config.ESP.Color.Partner.Skeleton[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Partner", "Info", "0" }, GameData.Config.ESP.Color.Partner.Info[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Partner", "Info", "1" }, GameData.Config.ESP.Color.Partner.Info[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Partner", "Info", "2" }, GameData.Config.ESP.Color.Partner.Info[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Partner", "Info", "3" }, GameData.Config.ESP.Color.Partner.Info[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Groggy", "Skeleton", "0" }, GameData.Config.ESP.Color.Groggy.Skeleton[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Groggy", "Skeleton", "1" }, GameData.Config.ESP.Color.Groggy.Skeleton[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Groggy", "Skeleton", "2" }, GameData.Config.ESP.Color.Groggy.Skeleton[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Groggy", "Skeleton", "3" }, GameData.Config.ESP.Color.Groggy.Skeleton[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Groggy", "Info", "0" }, GameData.Config.ESP.Color.Groggy.Info[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Groggy", "Info", "1" }, GameData.Config.ESP.Color.Groggy.Info[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Groggy", "Info", "2" }, GameData.Config.ESP.Color.Groggy.Info[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Groggy", "Info", "3" }, GameData.Config.ESP.Color.Groggy.Info[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Dangerous", "Skeleton", "0" }, GameData.Config.ESP.Color.Dangerous.Skeleton[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Dangerous", "Skeleton", "1" }, GameData.Config.ESP.Color.Dangerous.Skeleton[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Dangerous", "Skeleton", "2" }, GameData.Config.ESP.Color.Dangerous.Skeleton[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Dangerous", "Skeleton", "3" }, GameData.Config.ESP.Color.Dangerous.Skeleton[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Dangerous", "Info", "0" }, GameData.Config.ESP.Color.Dangerous.Info[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Dangerous", "Info", "1" }, GameData.Config.ESP.Color.Dangerous.Info[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Dangerous", "Info", "2" }, GameData.Config.ESP.Color.Dangerous.Info[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Dangerous", "Info", "3" }, GameData.Config.ESP.Color.Dangerous.Info[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Blacklist", "Skeleton", "0" }, GameData.Config.ESP.Color.Blacklist.Skeleton[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Blacklist", "Skeleton", "1" }, GameData.Config.ESP.Color.Blacklist.Skeleton[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Blacklist", "Skeleton", "2" }, GameData.Config.ESP.Color.Blacklist.Skeleton[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Blacklist", "Skeleton", "3" }, GameData.Config.ESP.Color.Blacklist.Skeleton[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Blacklist", "Info", "0" }, GameData.Config.ESP.Color.Blacklist.Info[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Blacklist", "Info", "1" }, GameData.Config.ESP.Color.Blacklist.Info[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Blacklist", "Info", "2" }, GameData.Config.ESP.Color.Blacklist.Info[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Blacklist", "Info", "3" }, GameData.Config.ESP.Color.Blacklist.Info[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Whitelist", "Skeleton", "0" }, GameData.Config.ESP.Color.Whitelist.Skeleton[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Whitelist", "Skeleton", "1" }, GameData.Config.ESP.Color.Whitelist.Skeleton[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Whitelist", "Skeleton", "2" }, GameData.Config.ESP.Color.Whitelist.Skeleton[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Whitelist", "Skeleton", "3" }, GameData.Config.ESP.Color.Whitelist.Skeleton[3]);

			SetConfigItem(Config, { ConfigName, "Color", "Whitelist", "Info", "0" }, GameData.Config.ESP.Color.Whitelist.Info[0]);
			SetConfigItem(Config, { ConfigName, "Color", "Whitelist", "Info", "1" }, GameData.Config.ESP.Color.Whitelist.Info[1]);
			SetConfigItem(Config, { ConfigName, "Color", "Whitelist", "Info", "2" }, GameData.Config.ESP.Color.Whitelist.Info[2]);
			SetConfigItem(Config, { ConfigName, "Color", "Whitelist", "Info", "3" }, GameData.Config.ESP.Color.Whitelist.Info[3]);

			SetConfigItem(Config, { ConfigName, "Color", "AI", "Skeleton", "0" }, GameData.Config.ESP.Color.AI.Skeleton[0]);
			SetConfigItem(Config, { ConfigName, "Color", "AI", "Skeleton", "1" }, GameData.Config.ESP.Color.AI.Skeleton[1]);
			SetConfigItem(Config, { ConfigName, "Color", "AI", "Skeleton", "2" }, GameData.Config.ESP.Color.AI.Skeleton[2]);
			SetConfigItem(Config, { ConfigName, "Color", "AI", "Skeleton", "3" }, GameData.Config.ESP.Color.AI.Skeleton[3]);
			}();

		[&] {
			std::string ConfigName = "Project";
			SetConfigItem(Config, { ConfigName, "Enable" }, GameData.Config.Project.Enable);
			SetConfigItem(Config, { ConfigName, "DistanceMax" }, GameData.Config.Project.DistanceMax);
			SetConfigItem(Config, { ConfigName, "FOV" }, GameData.Config.Project.FOV);
			SetConfigItem(Config, { ConfigName, "FontSize" }, GameData.Config.Project.FontSize);
			SetConfigItem(Config, { ConfigName, "TextShowChareTime" }, GameData.Config.Project.TextShowChareTime);
			SetConfigItem(Config, { ConfigName, "BarShowChareTime" }, GameData.Config.Project.BarShowChareTime);
			SetConfigItem(Config, { ConfigName, "ChareFontSize" }, GameData.Config.Project.ChareFontSize);
			SetConfigItem(Config, { ConfigName, "GrenadePrediction" }, GameData.Config.Project.GrenadePrediction);
			SetConfigItem(Config, { ConfigName, "ShowChareTime" }, GameData.Config.Project.ShowChareTime);
			SetConfigItem(Config, { ConfigName, "ChareColor", "0" }, GameData.Config.Project.ChareColor[0]);
			SetConfigItem(Config, { ConfigName, "ChareColor", "1" }, GameData.Config.Project.ChareColor[1]);
			SetConfigItem(Config, { ConfigName, "ChareColor", "2" }, GameData.Config.Project.ChareColor[2]);
			SetConfigItem(Config, { ConfigName, "ChareColor", "3" }, GameData.Config.Project.ChareColor[3]);//30109433

			SetConfigItem(Config, { ConfigName, "Color", "0" }, GameData.Config.Project.Color[0]);
			SetConfigItem(Config, { ConfigName, "Color", "1" }, GameData.Config.Project.Color[1]);
			SetConfigItem(Config, { ConfigName, "Color", "2" }, GameData.Config.Project.Color[2]);
			SetConfigItem(Config, { ConfigName, "Color", "3" }, GameData.Config.Project.Color[3]);
			}();

		[&] {
			std::string ConfigName = "AutoRecoil";
			SetConfigItem(Config, { ConfigName, "Enabled" }, GameData.Config.AimBot.Recoilenanlek);

			SetConfigItem(Config, { ConfigName, "Red Dot Amplitude" }, GameData.Config.AimBot.yRecoil[0]);
			SetConfigItem(Config, { ConfigName, "Double Amplitude" }, GameData.Config.AimBot.yRecoil[1]);
			SetConfigItem(Config, { ConfigName, "Triple Amplitude" }, GameData.Config.AimBot.yRecoil[2]);
			SetConfigItem(Config, { ConfigName, "Four Times The Amplitude" }, GameData.Config.AimBot.yRecoil[3]);
			SetConfigItem(Config, { ConfigName, "Six Times Amplitude" }, GameData.Config.AimBot.yRecoil[4]);
			SetConfigItem(Config, { ConfigName, "Eight Times Amplitude" }, GameData.Config.AimBot.yRecoil[5]);
			SetConfigItem(Config, { ConfigName, "Red Dot Delay" }, GameData.Config.AimBot.interval[0]);
			SetConfigItem(Config, { ConfigName, "Double Latency" }, GameData.Config.AimBot.interval[1]);
			SetConfigItem(Config, { ConfigName, "Triple Delay" }, GameData.Config.AimBot.interval[2]);
			SetConfigItem(Config, { ConfigName, "Four Times Delay" }, GameData.Config.AimBot.interval[3]);
			SetConfigItem(Config, { ConfigName, "Six Times Delay" }, GameData.Config.AimBot.interval[4]);
			SetConfigItem(Config, { ConfigName, "Eight Times Delay" }, GameData.Config.AimBot.interval[5]);

			}();


		[&] {
			std::string ConfigName = "Radar";
			SetConfigItem(Config, { ConfigName, "Main", "ShowPlayer" }, GameData.Config.Radar.Main.ShowPlayer);
			SetConfigItem(Config, { ConfigName, "Main", "ShowVehicle" }, GameData.Config.Radar.Main.ShowVehicle);
			SetConfigItem(Config, { ConfigName, "Main", "ShowAirDrop" }, GameData.Config.Radar.Main.ShowAirDrop);
			SetConfigItem(Config, { ConfigName, "Main", "ShowDeadBox" }, GameData.Config.Radar.Main.ShowDeadBox);
			SetConfigItem(Config, { ConfigName, "Main", "FontSize" }, GameData.Config.Radar.Main.FontSize);

			SetConfigItem(Config, { ConfigName, "Main", "MapColor", "0" }, GameData.Config.Radar.Main.MapColor[0]);
			SetConfigItem(Config, { ConfigName, "Main", "MapColor", "1" }, GameData.Config.Radar.Main.MapColor[1]);
			SetConfigItem(Config, { ConfigName, "Main", "MapColor", "2" }, GameData.Config.Radar.Main.MapColor[2]);
			SetConfigItem(Config, { ConfigName, "Main", "MapColor", "3" }, GameData.Config.Radar.Main.MapColor[3]);

			SetConfigItem(Config, { ConfigName, "Main", "Map_size" }, GameData.Config.Radar.Main.Map_size);
			SetConfigItem(Config, { ConfigName, "Mini", "ShowPlayer" }, GameData.Config.Radar.Mini.ShowPlayer);
			SetConfigItem(Config, { ConfigName, "Mini", "ShowVehicle" }, GameData.Config.Radar.Mini.ShowVehicle);
			SetConfigItem(Config, { ConfigName, "Mini", "ShowAirDrop" }, GameData.Config.Radar.Mini.ShowAirDrop);
			SetConfigItem(Config, { ConfigName, "Mini", "ShowDeadBox" }, GameData.Config.Radar.Mini.ShowDeadBox);
			SetConfigItem(Config, { ConfigName, "Mini", "FontSize" }, GameData.Config.Radar.Mini.FontSize);

			SetConfigItem(Config, { ConfigName, "Mini", "MapColor", "0" }, GameData.Config.Radar.Mini.MapColor[0]);
			SetConfigItem(Config, { ConfigName, "Mini", "MapColor", "1" }, GameData.Config.Radar.Mini.MapColor[1]);
			SetConfigItem(Config, { ConfigName, "Mini", "MapColor", "2" }, GameData.Config.Radar.Mini.MapColor[2]);
			SetConfigItem(Config, { ConfigName, "Mini", "MapColor", "3" }, GameData.Config.Radar.Mini.MapColor[3]);

			SetConfigItem(Config, { ConfigName, "Mini", "Map_size" }, GameData.Config.Radar.Mini.Map_size);
			}();

		[&] {
			std::string ConfigName = "Overlay";
			SetConfigItem(Config, { ConfigName, "VSync" }, GameData.Config.Overlay.VSync);
			SetConfigItem(Config, { ConfigName, "Enable" }, GameData.Config.Overlay.Enable);
			SetConfigItem(Config, { ConfigName, "UseLastFrameCameraCache" }, GameData.Config.Overlay.UseLastFrameCameraCache);
			SetConfigItem(Config, { ConfigName, "UseThread" }, GameData.Config.Overlay.UseThread);
			SetConfigItem(Config, { ConfigName, "SafeExit" }, GameData.Config.Overlay.Quit_key);
			SetConfigItem(Config, { ConfigName, "FusionModeKey" }, GameData.Config.Overlay.FusionModeKey);
			SetConfigItem(Config, { ConfigName, "ShowMenu" }, GameData.Config.Menu.ShowKey);
			}();
	}
};