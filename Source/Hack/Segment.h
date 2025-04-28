#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <windows.h>
#include <winhttp.h>
#include "cJSON/cJSON.h"
#include <memory>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <mutex>

#pragma comment(lib, "winhttp.lib")

class HttpHandle {
public:
	explicit HttpHandle(HINTERNET handle) : handle_(handle) {}
	~HttpHandle() { if (handle_) WinHttpCloseHandle(handle_); }
	HINTERNET get() const { return handle_; }
	bool isValid() const { return handle_ != nullptr; }
private:
	HINTERNET handle_;
};

class JsonHandle {
public:
	explicit JsonHandle(cJSON* json) : json_(json) {}
	~JsonHandle() { if (json_) cJSON_Delete(json_); }
	cJSON* get() const { return json_; }
	bool isValid() const { return json_ != nullptr; }
private:
	cJSON* json_;
};

class Segment {
public:

	static std::string SendHttpPostRequest(const std::string& url, const std::string& postData) {
		HttpHandle hSession(WinHttpOpen(L"WinHTTP Example/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0));

		if (!hSession.isValid()) {
			std::cerr << "Failed to open HTTP session." << std::endl;
			return "";
		}

		std::wstring wurl(url.begin(), url.end());
		URL_COMPONENTS urlComp = {};
		urlComp.dwStructSize = sizeof(urlComp);
		wchar_t hostName[256], urlPath[256];
		urlComp.lpszHostName = hostName;
		urlComp.dwHostNameLength = _countof(hostName);
		urlComp.lpszUrlPath = urlPath;
		urlComp.dwUrlPathLength = _countof(urlPath);

		if (!WinHttpCrackUrl(wurl.c_str(), (DWORD)wurl.length(), 0, &urlComp)) {
			std::cerr << "Failed to crack URL." << std::endl;
			return "";
		}

		HttpHandle hConnect(WinHttpConnect(hSession.get(), urlComp.lpszHostName, urlComp.nPort, 0));
		if (!hConnect.isValid()) {
			std::cerr << "Failed to connect to server." << std::endl;
			return "";
		}

		HttpHandle hRequest(WinHttpOpenRequest(hConnect.get(), L"POST", urlComp.lpszUrlPath,
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			urlComp.nPort == INTERNET_DEFAULT_HTTPS_PORT ? WINHTTP_FLAG_SECURE : 0));

		if (!hRequest.isValid()) {
			std::cerr << "Failed to open HTTP request." << std::endl;
			return "";
		}

		// 设置超时为 5 秒 (5000 毫秒)
		DWORD timeout = 15000;
		WinHttpSetOption(hRequest.get(), WINHTTP_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
		WinHttpSetOption(hRequest.get(), WINHTTP_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
		WinHttpSetOption(hRequest.get(), WINHTTP_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));

		// 设置请求头
		const wchar_t* headers = L"Content-Type: application/x-www-form-urlencoded";
		if (!WinHttpSendRequest(hRequest.get(), headers, -1L,
			(LPVOID)postData.c_str(), (DWORD)postData.size(),
			(DWORD)postData.size(), 0)) {
			std::cerr << "Error in WinHttpSendRequest: " << GetLastError() << std::endl;
			return "";
		}

		if (!WinHttpReceiveResponse(hRequest.get(), NULL)) {
			std::cerr << "Error in WinHttpReceiveResponse: " << GetLastError() << std::endl;
			return "";
		}

		std::string response;
		DWORD dwSize = 0;
		do {
			if (WinHttpQueryDataAvailable(hRequest.get(), &dwSize) && dwSize > 0) {
				std::vector<char> buffer(dwSize + 1, 0);
				DWORD dwDownloaded = 0;
				if (WinHttpReadData(hRequest.get(), buffer.data(), dwSize, &dwDownloaded)) {
					response.append(buffer.data(), dwDownloaded);
				}
			}
		} while (dwSize > 0);

		return response;
	}

	static std::string GetResponse(const std::string& userName) {
		std::string url = "http://49.235.186.109:2365/postRank";//段位服务器
		std::string postData = "account=" + userName; // 根据需要构造POST数据
		return SendHttpPostRequest(url, postData);
	}

	static std::string DoubleToString(double value) {
		// 取整操作（可以使用 round、floor、ceil 等函数）
		int intValue = static_cast<int>(std::round(value)); // 这里使用 round 四舍五入

		// 转换为字符串
		return std::to_string(intValue);
	}

	static std::string DoubleToString2(double value) {
		std::ostringstream stream;
		// 设置流的格式，保留小数点后一位
		stream << std::fixed << std::setprecision(2) << value;
		return stream.str();
	}

	static void Update() {
		std::unordered_map<std::string, int> requestCountMap;
		std::unordered_map<std::string, PlayerRankList> PlayerRankLists;
		std::mutex dataMutex;

		while (true) {
			if (GameData.Scene != Scene::Gaming) {
				requestCountMap.clear();
				PlayerRankLists.clear();
				Data::SetPlayerRankLists({});
				Data::SetPlayerSegmentLists({});
				std::this_thread::sleep_for(std::chrono::milliseconds(GameData.ThreadSleep));
				continue;
			}

			auto PlayerSegmentLists = Data::GetPlayerSegmentLists();
			auto PlayerRankLists = Data::GetPlayerRankLists();

			for (auto& pair : PlayerRankLists) {
				PlayerRankList& detail = pair.second;
				if (detail.Tem > 0 && detail.Tem < 100) {
					const std::string& playerName = detail.PlayerName;
					const std::string& AccountId = detail.AccountId;
					//printf("AccountId [%s]\n", detail.AccountId.c_str());

					PlayerRankList& temp = PlayerSegmentLists[playerName];
					if (temp.TPP.Updated && temp.FPP.Updated && temp.SquadTPP.Updated && temp.SquadFPP.Updated)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
						continue;
					}

					if (detail.Survivallevel < 80)
					{
						std::lock_guard<std::mutex> lock(dataMutex);
						if (!temp.TPP.Updated) {
							temp.TPP.TierToString = U8("    未定级");
							temp.TPP.Updated = true;
						}
						if (!temp.FPP.Updated) {
							temp.FPP.TierToString = U8("    未定级");
							temp.FPP.Updated = true;
						}
						if (!temp.SquadTPP.Updated) {
							temp.SquadTPP.TierToString = U8("    未定级");
							temp.SquadTPP.Updated = true;
						}
						if (!temp.SquadFPP.Updated) {
							temp.SquadFPP.TierToString = U8("    未定级");
							temp.SquadFPP.Updated = true;
						}
						Data::SetPlayerSegmentListsItem(playerName, temp);
						continue;
					}
					if (requestCountMap[playerName] >= 2) {
						std::lock_guard<std::mutex> lock(dataMutex);
						if (!temp.TPP.Updated) {
							temp.TPP.TierToString = U8("    未定级");
							temp.TPP.Updated = true;
						}
						if (!temp.FPP.Updated) {
							temp.FPP.TierToString = U8("    未定级");
							temp.FPP.Updated = true;
						}
						if (!temp.SquadTPP.Updated) {
							temp.SquadTPP.TierToString = U8("    未定级");
							temp.SquadTPP.Updated = true;
						}
						if (!temp.SquadFPP.Updated) {
							temp.SquadFPP.TierToString = U8("    未定级");
							temp.SquadFPP.Updated = true;
						}
						Data::SetPlayerSegmentListsItem(playerName, temp);
						continue;
					}

					std::string response = GetResponse(AccountId);//playerName AccountId

					//printf("AccountId [%s]  playerName [%s]\n", AccountId.c_str(), playerName.c_str());

					if (response.empty()) {
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
						continue;
					}


					JsonHandle root(cJSON_Parse(response.c_str()));
					if (!root.isValid()) {
						std::cerr << "Failed to parse JSON response." << std::endl;
						requestCountMap[playerName]++;
						continue;
					}

					cJSON* data = cJSON_GetObjectItem(root.get(), "data");
					if (!data) {
						requestCountMap[playerName]++;
						continue;
					}
					cJSON* attributes = cJSON_GetObjectItem(data, "attributes");
					if (attributes) {
						cJSON* rankedGameModeStats = cJSON_GetObjectItem(attributes, "rankedGameModeStats");
						if (cJSON_GetArraySize(rankedGameModeStats) == 0)
						{
							std::lock_guard<std::mutex> lock(dataMutex);
							if (!temp.TPP.Updated) {
								temp.TPP.TierToString = U8("    未定级");
								temp.TPP.Updated = true;
							}
							if (!temp.FPP.Updated) {
								temp.FPP.TierToString = U8("    未定级");
								temp.FPP.Updated = true;
							}
							if (!temp.SquadTPP.Updated) {
								temp.SquadTPP.TierToString = U8("    未定级");
								temp.SquadTPP.Updated = true;
							}
							if (!temp.SquadFPP.Updated) {
								temp.SquadFPP.TierToString = U8("    未定级");
								temp.SquadFPP.Updated = true;
							}
							Data::SetPlayerSegmentListsItem(playerName, temp);
							continue;
						}
						else
						{
							ParseGameModeStats(temp, rankedGameModeStats, "squad", 0);// TPP
							ParseGameModeStats(temp, rankedGameModeStats, "solo", 1);// 单人TPP
							ParseGameModeStats(temp, rankedGameModeStats, "squad-fpp", 2);// FPP
							ParseGameModeStats(temp, rankedGameModeStats, "solo-fpp", 3); //单人FPP
						}
					}

					std::lock_guard<std::mutex> lock(dataMutex);
					if (temp.TPP.Updated || temp.FPP.Updated || temp.SquadTPP.Updated || temp.SquadFPP.Updated) {
						Data::SetPlayerSegmentListsItem(playerName, temp);
						requestCountMap[playerName]++;
					}
					//printf("playerName [%s]\n", playerName.c_str());
					//printf("response [%s]\n", response.c_str());
					//requestCountMap[playerName]++;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(30));
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

private:
	static void ParseGameModeStats(PlayerRankList& temp, cJSON* stats, const char* mode, int Issquad) {
		cJSON* modeData = cJSON_GetObjectItem(stats, mode);
		if (!modeData) return;

		PlayerRankInfo& rank = Issquad == 0 ? temp.SquadTPP : Issquad == 1 ? temp.TPP : Issquad == 2 ? temp.SquadFPP : temp.FPP;
		if (rank.Updated) return;

		cJSON* currentTier = cJSON_GetObjectItem(modeData, "currentTier");
		if (currentTier) {
			cJSON* tier = cJSON_GetObjectItem(currentTier, "tier");
			cJSON* subTier = cJSON_GetObjectItem(currentTier, "subTier");
			if (cJSON_IsString(tier) && cJSON_IsString(subTier)) {
				rank.Tier = tier->valuestring;
				rank.SubTier = subTier->valuestring;
				rank.TierToString = RankTierToChinese[rank.Tier] + rank.SubTier;
			}
		}

		cJSON* KDA = cJSON_GetObjectItem(modeData, "kda");
		if (KDA && cJSON_IsNumber(KDA)) {
			rank.KDAToString = DoubleToString2(KDA->valuedouble);
		}

		cJSON* winRatio = cJSON_GetObjectItem(modeData, "winRatio");
		if (winRatio && cJSON_IsNumber(winRatio)) {
			rank.WinRatioToString = DoubleToString(winRatio->valuedouble);
		}

		cJSON* score = cJSON_GetObjectItem(modeData, "currentRankPoint");
		if (score && cJSON_IsNumber(score)) {
			rank.RankPoint = score->valuedouble;
		}

		//printf("TierToString  [%s]\n", rank.TierToString.c_str());
		if (!rank.TierToString.empty()) {
			rank.Updated = true;
		}
	}
};
