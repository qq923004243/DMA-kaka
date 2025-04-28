#pragma once
#include<iostream>
#include<vector>
#include"struct.h"
#include <mutex>


namespace global
{
	inline const char* gamewindowclassname = "UnrealWindow";
	inline const char* gamewindowtextname = "PUBG：绝地求生 ";
	inline const char* gamewindowtextname2 = "PUBG: BATTLEGROUNDS ";
	inline const char* 游戏模块名 = "TslGame.exe";

	inline std::string G_card;
	
	inline std::vector<CustomData> g_players;
	inline std::vector<car> g_cars;
	inline std::vector<item> g_items;


	// 添加一个 mutex 用于保护全局变量
	inline std::mutex g_mutex;
	inline std::mutex g_itemmutex;

    inline std::string generateCardKey(const std::string& username) {
        // 目标卡密长度
        const size_t targetLength = 34;

        // 计算用户名的长度
        size_t usernameLength = username.length();

        // 如果用户名已经是34位或更长，直接返回用户名的前34位
        if (usernameLength >= targetLength) {
            return username.substr(0, targetLength);
        }

        // 否则，将用户名重复，直到长度大于等于34位
        std::string cardKey = username;
        while (cardKey.length() < targetLength) {
            cardKey += username;
        }

        // 截取前34位
        return cardKey.substr(0, targetLength);
    }

}

