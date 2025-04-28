#include <WS2tcpip.h>
#include <iostream>
#include <regex>
#include <unordered_map>
#include "cJSON/cJSON.h"
#include <future>
#include <string>
#include <vector>
#include <wininet.h>
#include <map>

#include <Common/Data.h>

#pragma comment(lib, "wininet.lib")

class Rank
{
public:
    std::string sendGetRequestWithWininet(const std::string& host, int port, const std::string& path) {
        HINTERNET hInternet = InternetOpenA("WinINet Example", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) {
            return "hInternet error";
        }

        HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (!hConnect) {
            InternetCloseHandle(hInternet);
            return "hConnect error";
        }

        const char* acceptTypes[] = { "*/*", NULL };
        //const char* acceptTypes[] = { "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7", NULL };
        HINTERNET hRequest = HttpOpenRequestA(hConnect, "GET", path.c_str(), NULL, NULL, acceptTypes, INTERNET_FLAG_RELOAD, 0);
        if (!hRequest) {
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return "hRequest error";
        }

        //const char* headers = "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36\r\n";
        const char* headers = "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36\r\n";

        if (!HttpSendRequestA(hRequest, headers, strlen(headers), NULL, 0)) {
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return "headers error";
        }

        std::string response;
        char buffer[4096];
        DWORD bytesRead;
        while (InternetReadFile(hRequest, &buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
            response.append(buffer, bytesRead);
        }

        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        return response;
    }

    std::string sendPostRequestWithWininet(const std::string& host, int port, const std::string& path, const std::string& data) {
        HINTERNET hInternet = InternetOpenA("WinINet Example", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) {
            return "";
        }

        HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (!hConnect) {
            InternetCloseHandle(hInternet);
            return "";
        }

        const char* acceptTypes[] = { "*/*", NULL };
        HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", path.c_str(), NULL, NULL, acceptTypes, INTERNET_FLAG_RELOAD, 0);
        if (!hRequest) {
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return "";
        }

        std::string headers = "Content-Type: application/x-www-form-urlencoded\r\nContent-Length: " + std::to_string(data.length());
        if (!HttpSendRequestA(hRequest, headers.c_str(), headers.length(), (LPVOID)data.c_str(), data.length())) {
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return "";
        }

        std::string response;
        char buffer[4096];
        DWORD bytesRead;

        while (InternetReadFile(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
            response.append(buffer, bytesRead);
        }

        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        return response;
    }

    std::string extractOrReturnOriginal(const std::string& input) {
        std::regex pattern(R"(\[.*\]\s*(\S+))");
        std::smatch matches;

        if (std::regex_search(input, matches, pattern) && matches.size() > 1) {
            std::string result = matches[1].str();
            result.erase(remove(result.begin(), result.end(), ' '), result.end());
            return result;
        }
        else {
            return input;
        }
    }

    void fetcahwebget(const std::string& name)
    {
        std::string url = "/user/" + name;
        sendGetRequestWithWininet("pubg.op.gg", 80, url);
    }

    std::string extractSpecificString(const std::string& response) {
        std::regex pattern(R"(<div[^>]*\bid\s*=\s*\"userNickname\"[^>]*\bdata-user_id\s*=\s*\"([^\"]+)\")");
        std::smatch matches;

        if (std::regex_search(response, matches, pattern) && matches.size() > 1) {
            return matches[1].str();
        }
        return "";
    }

    std::string fetchWebGet(const std::string& name) {
        std::string url = "/user/" + name;
        return sendGetRequestWithWininet("pubg.op.gg", 80, url);
    }

    std::string fetchUserRankedStats(const std::string& userId) {
        std::string url = "/api/users/" + userId + GameData.Config.ESP.RankModeUrl;
        return sendGetRequestWithWininet("pubg.op.gg", 80, url);
    }

    void concurrentRequests(const std::vector<std::string>& names, std::unordered_map<std::string, bool>& QuerySuccess, std::unordered_map<std::string, int>& QueryCount) {

        std::map<std::string, std::future<std::string>> futures;
        std::mutex queryMutex;

        for (const auto& name : names) {

            if (QuerySuccess[name])
            {
                continue;
            }

            if (QueryCount[name] >= 2)
            {
                continue;
            }

            futures[name] = std::async(std::launch::async, [this, name]() {
                std::string url = "/user/" + name;
                return this->sendGetRequestWithWininet("pubg.op.gg", 80, url);
                });

            Sleep(2000);
        }

        for (auto& [name, future] : futures) {

            if (QuerySuccess[name])
            {
                continue;
            }

            if (QueryCount[name] >= 2)
            {
                continue;
            }

            std::string response = future.get();
            std::string extractedString = extractSpecificString(response);
            Utils::Log(1, "%s", Utils::StringToUTF8(response).c_str());



            if (!extractedString.empty()) {

                std::string rankedStatsResponse = fetchUserRankedStats(extractedString);

                cJSON* jsonRoot = cJSON_Parse(rankedStatsResponse.c_str());
                if (jsonRoot) {
                    PlayerRankList RankList;
                    {
                        std::lock_guard<std::mutex> lock(queryMutex);
                        QuerySuccess[name] = true;
                    }

                    if (GameData.Config.PlayerList.RankMode == 1)
                    {
                        cJSON* tier = cJSON_GetObjectItem(jsonRoot, "tier");
                        if (tier) {
                            cJSON* title = cJSON_GetObjectItem(tier, "title");
                            if (title) {
                                RankList.TPP.Tier = title->valuestring;
                                std::cout << "RankList.SquadFPP.Tier: " << RankList.TPP.Tier << std::endl;
                            }
                        }

                        cJSON* stats = cJSON_GetObjectItem(jsonRoot, "stats");
                        if (stats) {
                            int kills = 0;
                            int deaths = 0;

                            int matches_cnt = 0;
                            int win_matches_cnt = 0;

                            cJSON* rank_points = cJSON_GetObjectItem(stats, "rank_points");
                            if (rank_points) {
                                RankList.TPP.RankPoint = rank_points->valueint;
                                std::cout << "RankList.SquadFPP.RankPoint: " << RankList.TPP.RankPoint << std::endl;
                            }

                            cJSON* killsSum = cJSON_GetObjectItem(stats, "kills_sum");
                            if (killsSum) {
                                kills = killsSum->valueint;
                            }

                            cJSON* deathsSum = cJSON_GetObjectItem(stats, "deaths_sum");
                            if (deathsSum) {
                                deaths = deathsSum->valueint;
                            }

                            cJSON* matchesSum = cJSON_GetObjectItem(stats, "matches_cnt");
                            if (matchesSum) {
                                matches_cnt = matchesSum->valueint;
                            }

                            cJSON* win_matchesSum = cJSON_GetObjectItem(stats, "win_matches_cnt");
                            if (win_matchesSum) {
                                win_matches_cnt = win_matchesSum->valueint;
                            }

                            RankList.TPP.WinRatio = static_cast<double>(matches_cnt) / win_matches_cnt;

                            RankList.TPP.KDA = static_cast<double>(kills) / deaths;
                            std::cout << "RankList.SquadFPP.KDA: " << RankList.TPP.KDA << std::endl;
                            std::cout << "RankList.WinRatio: \n" << RankList.TPP.WinRatio << std::endl;
                        }
                    }
                    else if (GameData.Config.PlayerList.RankMode == 2)
                    {
                        cJSON* tier = cJSON_GetObjectItem(jsonRoot, "tier");
                        if (tier) {
                            cJSON* title = cJSON_GetObjectItem(tier, "title");
                            if (title) {
                                RankList.SquadTPP.Tier = title->valuestring;
                                std::cout << "RankList.SquadFPP.Tier: " << RankList.SquadTPP.Tier << std::endl;
                            }
                        }

                        cJSON* stats = cJSON_GetObjectItem(jsonRoot, "stats");
                        if (stats) {
                            int kills = 0;
                            int deaths = 0;

                            int matches_cnt = 0;
                            int win_matches_cnt = 0;

                            cJSON* rank_points = cJSON_GetObjectItem(stats, "rank_points");
                            if (rank_points) {
                                RankList.SquadTPP.RankPoint = rank_points->valueint;
                                std::cout << "RankList.SquadFPP.RankPoint: " << RankList.SquadTPP.RankPoint << std::endl;
                            }

                            cJSON* killsSum = cJSON_GetObjectItem(stats, "kills_sum");
                            if (killsSum) {
                                kills = killsSum->valueint;
                            }

                            cJSON* deathsSum = cJSON_GetObjectItem(stats, "deaths_sum");
                            if (deathsSum) {
                                deaths = deathsSum->valueint;
                            }

                            cJSON* matchesSum = cJSON_GetObjectItem(stats, "matches_cnt");
                            if (matchesSum) {
                                matches_cnt = matchesSum->valueint;
                            }

                            cJSON* win_matchesSum = cJSON_GetObjectItem(stats, "win_matches_cnt");
                            if (win_matchesSum) {
                                win_matches_cnt = win_matchesSum->valueint;
                            }

                            RankList.SquadTPP.WinRatio = static_cast<double>(matches_cnt) / win_matches_cnt;

                            RankList.SquadTPP.KDA = static_cast<double>(kills) / deaths;
                            std::cout << "RankList.SquadFPP.KDA: " << RankList.SquadTPP.KDA << std::endl;
                            std::cout << "RankList.WinRatio: \n" << RankList.SquadTPP.WinRatio << std::endl;
                        }
                    }
                    else if (GameData.Config.PlayerList.RankMode == 3)
                    {
                        cJSON* tier = cJSON_GetObjectItem(jsonRoot, "tier");
                        if (tier) {
                            cJSON* title = cJSON_GetObjectItem(tier, "title");
                            if (title) {
                                RankList.FPP.Tier = title->valuestring;
                                std::cout << "RankList.SquadFPP.Tier: " << RankList.FPP.Tier << std::endl;
                            }
                        }

                        cJSON* stats = cJSON_GetObjectItem(jsonRoot, "stats");
                        if (stats) {
                            int kills = 0;
                            int deaths = 0;

                            int matches_cnt = 0;
                            int win_matches_cnt = 0;

                            cJSON* rank_points = cJSON_GetObjectItem(stats, "rank_points");
                            if (rank_points) {
                                RankList.FPP.RankPoint = rank_points->valueint;
                                std::cout << "RankList.SquadFPP.RankPoint: " << RankList.FPP.RankPoint << std::endl;
                            }

                            cJSON* killsSum = cJSON_GetObjectItem(stats, "kills_sum");
                            if (killsSum) {
                                kills = killsSum->valueint;
                            }

                            cJSON* deathsSum = cJSON_GetObjectItem(stats, "deaths_sum");
                            if (deathsSum) {
                                deaths = deathsSum->valueint;
                            }

                            cJSON* matchesSum = cJSON_GetObjectItem(stats, "matches_cnt");
                            if (matchesSum) {
                                matches_cnt = matchesSum->valueint;
                            }

                            cJSON* win_matchesSum = cJSON_GetObjectItem(stats, "win_matches_cnt");
                            if (win_matchesSum) {
                                win_matches_cnt = win_matchesSum->valueint;
                            }

                            RankList.FPP.WinRatio = static_cast<double>(matches_cnt) / win_matches_cnt;

                            RankList.FPP.KDA = static_cast<double>(kills) / deaths;
                            std::cout << "RankList.SquadFPP.KDA: " << RankList.FPP.KDA << std::endl;
                            std::cout << "RankList.WinRatio: " << RankList.FPP.WinRatio << std::endl;
                        }
                    }
                    else if (GameData.Config.PlayerList.RankMode == 4)
                    {
                        cJSON* tier = cJSON_GetObjectItem(jsonRoot, "tier");
                        if (tier) {
                            cJSON* title = cJSON_GetObjectItem(tier, "title");
                            if (title) {
                                RankList.SquadFPP.Tier = title->valuestring;
                                std::cout << "RankList.SquadFPP.Tier: " << RankList.SquadFPP.Tier << std::endl;
                            }
                        }

                        cJSON* stats = cJSON_GetObjectItem(jsonRoot, "stats");
                        if (stats) {
                            int kills = 0;
                            int deaths = 0;

                            int matches_cnt = 0;
                            int win_matches_cnt = 0;

                            cJSON* rank_points = cJSON_GetObjectItem(stats, "rank_points");
                            if (rank_points) {
                                RankList.SquadFPP.RankPoint = rank_points->valueint;
                                std::cout << "RankList.SquadFPP.RankPoint: " << RankList.SquadFPP.RankPoint << std::endl;
                            }

                            cJSON* killsSum = cJSON_GetObjectItem(stats, "kills_sum");
                            if (killsSum) {
                                kills = killsSum->valueint;
                            }

                            cJSON* deathsSum = cJSON_GetObjectItem(stats, "deaths_sum");
                            if (deathsSum) {
                                deaths = deathsSum->valueint;
                            }

                            cJSON* matchesSum = cJSON_GetObjectItem(stats, "matches_cnt");
                            if (matchesSum) {
                                matches_cnt = matchesSum->valueint;
                            }

                            cJSON* win_matchesSum = cJSON_GetObjectItem(stats, "win_matches_cnt");
                            if (win_matchesSum) {
                                win_matches_cnt = win_matchesSum->valueint;
                            }

                            RankList.SquadFPP.WinRatio = static_cast<double>(matches_cnt) / win_matches_cnt;

                            RankList.SquadFPP.KDA = static_cast<double>(kills) / deaths;
                            std::cout << "RankList.SquadFPP.KDA: " << RankList.SquadFPP.KDA << std::endl;
                            std::cout << "RankList.WinRatio:\n " << RankList.SquadFPP.WinRatio << std::endl;
                        }
                    }
                    Data::SetPlayerRankListsItem(name, RankList);
                    cJSON_Delete(jsonRoot);
                }
                else {
                    {
                        std::lock_guard<std::mutex> lock(queryMutex);
                        QueryCount[name]++;
                    }
                    std::cerr << "Failed to 2 JSON response." << std::endl;
                }
            }
            else {
                {
                    std::lock_guard<std::mutex> lock(queryMutex);
                    QueryCount[name]++;
                }

                std::cerr << "User opggID not found." << std::endl;
            }
        }
    }

    std::pair<std::string, PlayerRankList> fetchPlayerRank(const std::string& name) {
        std::string url = "/user/" + name;
        std::string response = sendGetRequestWithWininet("pubg.op.gg", 80, url);

        std::string extractedString = extractSpecificString(response);

        //std::cout << "OpggID: " << extractedString << std::endl;

        PlayerRankList playerRankList;
        if (!extractedString.empty()) {
            std::string rankedStatsResponse = fetchUserRankedStats(extractedString);

            cJSON* jsonRoot = cJSON_Parse(rankedStatsResponse.c_str());
            if (jsonRoot) {
                int kills = 0;
                int deaths = 0;
                int rank = 0;

                cJSON* tier = cJSON_GetObjectItem(jsonRoot, "tier");
                if (tier) {
                    cJSON* title = cJSON_GetObjectItem(tier, "title");
                    if (title) {
                        playerRankList.SquadFPP.Tier = title->valuestring;
                    }
                }

                cJSON* stats = cJSON_GetObjectItem(jsonRoot, "stats");
                if (stats) {
                    cJSON* rank_points = cJSON_GetObjectItem(stats, "rank_points");
                    if (rank_points) {
                        playerRankList.SquadFPP.RankPoint = rank_points->valueint;
                    }

                    cJSON* killsSum = cJSON_GetObjectItem(stats, "kills_sum");
                    if (killsSum) {
                        kills = killsSum->valueint;
                    }

                    cJSON* deathsSum = cJSON_GetObjectItem(stats, "deaths_sum");
                    if (deathsSum) {
                        deaths = deathsSum->valueint;
                    }
                    playerRankList.SquadFPP.KDA = static_cast<double>(kills) / deaths;
                }

                cJSON_Delete(jsonRoot);
            }
        }

        return { name, playerRankList };
    }

    void concurrentRequestsPaid(const std::vector<std::string>& names, std::unordered_map<std::string, bool>& QuerySuccess, std::unordered_map<std::string, int>& QueryCount) {
        std::map<std::string, std::future<std::string>> futures;
        std::mutex queryMutex;

        for (const auto& name : names) {

            // 检查该玩家是否已经成功查询过
            if (QuerySuccess[name])
            {
                continue;
            }

            if (QueryCount[name] >= 2)
            {
                continue;
            }
            //printf("%s/n", name);
            futures[name] = std::async(std::launch::async, [this, name]() {
                std::string url = "/user/" + name;
                return this->extractPaidInterface(name, "TPP");
                });
        }




        std::vector<std::future<void>> futures_list;  // 用于存储所有的异步任务

        for (auto& [name, future] : futures) {
            // 检查该玩家是否已经成功查询过
            if (QuerySuccess[name]) {
                continue;
            }

            if (QueryCount[name] >= 2) {
                continue;
            }

            // 将每个任务异步化
            futures_list.push_back(std::async(std::launch::async, [&] {
                std::string response = future.get();
                if (!response.empty()) {
                    PlayerRankList RankList;

                    {
                        std::lock_guard<std::mutex> lock(queryMutex);
                        QuerySuccess[name] = true;
                    }

                    // 解析 JSON 数据
                    cJSON* root = cJSON_Parse(response.c_str());
                    if (root == nullptr) {
                        return;
                    }
                    // std::cout << "Root JSON: " << cJSON_Print(root) << std::endl;
                     // 获取 "data" 对象
                    cJSON* data = cJSON_GetObjectItemCaseSensitive(root, "data");
                    if (data == nullptr) {
                        std::cout << "Root: " << root << std::endl;
                        cJSON_Delete(root);
                        return;
                    }

                    cJSON* attributes = cJSON_GetObjectItemCaseSensitive(data, "attributes");
                    if (attributes == nullptr) {
                        cJSON_Delete(data);
                        return;
                    }

                    cJSON* rankedGameModeStats = cJSON_GetObjectItemCaseSensitive(attributes, "rankedGameModeStats");
                    if (rankedGameModeStats == nullptr) {
                        cJSON_Delete(attributes);
                        return;
                    }
                    RankList.SquadFPP.Tier = Utils::StringToUTF8("未定级");

                    // 处理 SquadFPP
                    cJSON* squad_fpp = cJSON_GetObjectItemCaseSensitive(rankedGameModeStats, "squad-fpp");
                    if (squad_fpp != nullptr) {
                        // 解析 "currentTier"
                        cJSON* currentTier = cJSON_GetObjectItemCaseSensitive(squad_fpp, "currentTier");
                        if (currentTier != nullptr) {
                            cJSON* tier = cJSON_GetObjectItemCaseSensitive(currentTier, "tier");
                            cJSON* subTier = cJSON_GetObjectItemCaseSensitive(currentTier, "subTier");

                            if (tier && subTier) {
                                // 拼接 "tier" 和 "subTier"
                                RankList.SquadFPP.Tier = std::string(tier->valuestring) + " " + subTier->valuestring;
                            }

                        }

                        // 获取其他字段
                        cJSON* currentRankPoint = cJSON_GetObjectItemCaseSensitive(squad_fpp, "currentRankPoint");
                        if (currentRankPoint != nullptr) {
                            RankList.SquadFPP.RankPoint = currentRankPoint->valueint;
                        }

                        cJSON* kda = cJSON_GetObjectItemCaseSensitive(squad_fpp, "kda");
                        if (kda != nullptr) {
                            RankList.SquadFPP.KDA = static_cast<float>(kda->valuedouble);
                        }

                        cJSON* winRatio = cJSON_GetObjectItemCaseSensitive(squad_fpp, "winRatio");
                        if (winRatio != nullptr) {
                            RankList.SquadFPP.WinRatio = static_cast<float>(winRatio->valuedouble);
                        }
                    }
                    RankList.SquadTPP.Tier = Utils::StringToUTF8("未定级");
                    // 处理 SquadTPP
                    cJSON* squad = cJSON_GetObjectItemCaseSensitive(rankedGameModeStats, "squad");
                    if (squad != nullptr) {
                        // 解析 "currentTier"
                        cJSON* currentTier = cJSON_GetObjectItemCaseSensitive(squad, "currentTier");
                        if (currentTier != nullptr) {
                            cJSON* tier = cJSON_GetObjectItemCaseSensitive(currentTier, "tier");
                            cJSON* subTier = cJSON_GetObjectItemCaseSensitive(currentTier, "subTier");

                            if (tier && subTier) {
                                // 拼接 "tier" 和 "subTier"
                                RankList.SquadTPP.Tier = std::string(tier->valuestring) + " " + subTier->valuestring;
                            }
                        }

                        // 获取其他字段
                        cJSON* currentRankPoint = cJSON_GetObjectItemCaseSensitive(squad, "currentRankPoint");
                        if (currentRankPoint != nullptr) {
                            RankList.SquadTPP.RankPoint = currentRankPoint->valueint;
                        }

                        cJSON* kda = cJSON_GetObjectItemCaseSensitive(squad, "kda");
                        if (kda != nullptr) {
                            RankList.SquadTPP.KDA = static_cast<float>(kda->valuedouble);
                        }

                        cJSON* winRatio = cJSON_GetObjectItemCaseSensitive(squad, "winRatio");
                        if (winRatio != nullptr) {
                            RankList.SquadTPP.WinRatio = static_cast<float>(winRatio->valuedouble);
                        }
                    }
                    RankList.TPP.Tier = Utils::StringToUTF8("未定级");
                    // 处理 Solo TPP
                    cJSON* solo = cJSON_GetObjectItemCaseSensitive(rankedGameModeStats, "solo");
                    if (solo != nullptr) {
                        // 解析 "currentTier"
                        cJSON* currentTier = cJSON_GetObjectItemCaseSensitive(solo, "currentTier");
                        if (currentTier != nullptr) {
                            cJSON* tier = cJSON_GetObjectItemCaseSensitive(currentTier, "tier");
                            cJSON* subTier = cJSON_GetObjectItemCaseSensitive(currentTier, "subTier");

                            if (tier && subTier) {
                                // 拼接 "tier" 和 "subTier"
                                RankList.TPP.Tier = std::string(tier->valuestring) + " " + subTier->valuestring;
                            }
                        }

                        // 获取其他字段
                        cJSON* currentRankPoint = cJSON_GetObjectItemCaseSensitive(solo, "currentRankPoint");
                        if (currentRankPoint != nullptr) {
                            RankList.TPP.RankPoint = currentRankPoint->valueint;
                        }

                        cJSON* kda = cJSON_GetObjectItemCaseSensitive(solo, "kda");
                        if (kda != nullptr) {
                            RankList.TPP.KDA = static_cast<float>(kda->valuedouble);
                        }

                        cJSON* winRatio = cJSON_GetObjectItemCaseSensitive(solo, "winRatio");
                        if (winRatio != nullptr) {
                            RankList.TPP.WinRatio = static_cast<float>(winRatio->valuedouble);
                        }
                    }

                    // 保存排名数据
                    Data::SetPlayerRankListsItem(name, RankList);

                    cJSON_Delete(root);  // 释放解析的 JSON 数据
                }
                else {
                    std::lock_guard<std::mutex> lock(queryMutex);
                    QueryCount[name]++;
                }
                }));
        }

        // 等待所有异步任务完成
        for (auto& fut : futures_list) {
            fut.get();
        }

    }
    //std::string extractPaidInterface(const std::string& Name, const std::string& ModeName) {
    //    std::cout << "Name: " << Name << std::endl;
    //    std::string url = "/getRankByName?name=" + Name;
    //    std::string data = sendGetRequestWithWininet("pubg.000881.xyz", 0, url);
    //    return data;

    //}
    std::string extractPaidInterface(const std::string& Name, const std::string& ModeName) {
        std::cout << "Name: " << Name << std::endl;
        std::string url = "/getRankByName?name=" + Name;
        std::string data = sendGetRequestWithWininet("pubg.000881.xyz", 0, url);//1504查询：45.205.30.22", 8080,
                                                                                //pubg.000881.xyz", 0,
        return data;

    }

    static void Update()
    {
        Rank rank;
        std::unordered_map<std::string, PlayerRankList> PlayerRankLists;
        std::vector<std::string> pPlayers;
        std::unordered_map<std::string, bool> QuerySuccess;
        std::unordered_map<std::string, int> QueryCount;


        while (true)
        {
            if (GameData.Scene != Scene::Gaming)
            {
                Sleep(GameData.ThreadSleep);
                continue;
            }

            if (!GameData.Config.PlayerList.RankMode)
            {
                Sleep(GameData.ThreadSleep);
                continue;
            }
            //https://pubg.op.gg/api/users/5e218a2030fa6b00140eee2/ranked-stats?season=pc-2018-31&queue_size=1&mode=competitive-tpp

            if (GameData.Config.PlayerList.RankMode == 1)
            {
                GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=1&mode=competitive-tpp";
            }
            else if (GameData.Config.PlayerList.RankMode == 2)
            {
                GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=4&mode=competitive-tpp";
            }
            else if (GameData.Config.PlayerList.RankMode == 3)
            {
                GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=1&mode=competitive-fpp";
            }
            else if (GameData.Config.PlayerList.RankMode == 4)
            {
                GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=4&mode=competitive-fpp";
            }

            PlayerRankLists = Data::GetPlayerRankLists();

            if (!GameData.Config.ESP.Tier && !GameData.Config.ESP.RankPoint && !GameData.Config.ESP.KDA)
            {
                PlayerRankLists.clear();
                pPlayers.clear();
                QuerySuccess.clear();
                QuerySuccess.clear();
                Sleep(GameData.ThreadSleep);
                continue;
            }

            for (auto& player : PlayerRankLists)
            {
                if (QuerySuccess.find(player.first) == QuerySuccess.end())
                {
                    QuerySuccess[player.first] = false;
                }

                if (QueryCount.find(player.first) == QueryCount.end())
                {
                    QueryCount[player.first] = 0;
                }

                if (QuerySuccess[player.first])
                {
                    continue;
                }

                pPlayers.push_back(player.first);
            }

            if (!pPlayers.empty())
            {
                rank.concurrentRequests(pPlayers, QuerySuccess, QueryCount);
            }


            PlayerRankLists.clear();
            pPlayers.clear();
        }
    }



    //去Hack.h 文件内把 std::thread UpdateRankThread(Rank::Update); 改成 std::thread UpdateRankThread(Rank::UpdatePaid);
    static void UpdatePaid() {

        Rank rank;
        std::unordered_map<std::string, PlayerRankList> PlayerRankLists;
        std::vector<std::string> pPlayers;
        std::unordered_map<std::string, bool> QuerySuccess;
        std::unordered_map<std::string, int> QueryCount;
        //Utils::Log(1, "%s", Utils::StringToUTF8(rank.extractPaidInterface("SSIBHACK", "TPP")).c_str());

        while (true) {

            if (GameData.Scene != Scene::Gaming) {
                Sleep(GameData.ThreadSleep);
                continue;
            }

            if (!GameData.Config.PlayerList.RankMode) {
                Sleep(GameData.ThreadSleep);
                continue;
            }

            if (GameData.Config.PlayerList.RankMode == 1)
            {
                GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=1&mode=competitive-tpp";
            }
            else if (GameData.Config.PlayerList.RankMode == 2)
            {
                GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=4&mode=competitive-tpp";
            }
            else if (GameData.Config.PlayerList.RankMode == 3)
            {
                GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=1&mode=competitive-fpp";
            }
            else if (GameData.Config.PlayerList.RankMode == 4)
            {
                GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=4&mode=competitive-fpp";
            }

            PlayerRankLists = Data::GetPlayerRankLists();
            if (!GameData.Config.ESP.Tier && !GameData.Config.ESP.RankPoint && !GameData.Config.ESP.KDA) {
                PlayerRankLists.clear();
                pPlayers.clear();
                QuerySuccess.clear();
                QuerySuccess.clear();
                Sleep(GameData.ThreadSleep);
                continue;
            }

            for (auto& player : PlayerRankLists) {
                if (QuerySuccess.find(player.first) == QuerySuccess.end()) {
                    QuerySuccess[player.first] = false;
                }

                if (QueryCount.find(player.first) == QueryCount.end()) {
                    QueryCount[player.first] = 0;
                }

                if (QuerySuccess[player.first]) {
                    continue;
                }

                pPlayers.push_back(player.first);
            }

            if (!pPlayers.empty()) {
                rank.concurrentRequestsPaid(pPlayers, QuerySuccess, QueryCount);
            }

            PlayerRankLists.clear();
            pPlayers.clear();
        }
    }
};
