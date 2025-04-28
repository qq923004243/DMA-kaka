#pragma once
#include<iostream>
#include<vector>
#include"struct.h"
#include <mutex>


namespace global
{
	inline const char* gamewindowclassname = "UnrealWindow";
	inline const char* gamewindowtextname = "PUBG���������� ";
	inline const char* gamewindowtextname2 = "PUBG: BATTLEGROUNDS ";
	inline const char* ��Ϸģ���� = "TslGame.exe";

	inline std::string G_card;
	
	inline std::vector<CustomData> g_players;
	inline std::vector<car> g_cars;
	inline std::vector<item> g_items;


	// ���һ�� mutex ���ڱ���ȫ�ֱ���
	inline std::mutex g_mutex;
	inline std::mutex g_itemmutex;

    inline std::string generateCardKey(const std::string& username) {
        // Ŀ�꿨�ܳ���
        const size_t targetLength = 34;

        // �����û����ĳ���
        size_t usernameLength = username.length();

        // ����û����Ѿ���34λ�������ֱ�ӷ����û�����ǰ34λ
        if (usernameLength >= targetLength) {
            return username.substr(0, targetLength);
        }

        // ���򣬽��û����ظ���ֱ�����ȴ��ڵ���34λ
        std::string cardKey = username;
        while (cardKey.length() < targetLength) {
            cardKey += username;
        }

        // ��ȡǰ34λ
        return cardKey.substr(0, targetLength);
    }

}

