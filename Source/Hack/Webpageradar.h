#pragma once
#include <Common/Data.h>
#include <nlohmann/json.hpp>
#include <winhttp.h>
#include <iostream>
#include <string>
#include <memory>

#pragma comment(lib, "winhttp.lib")

using json = nlohmann::json;

class WebRadar
{
public:

    static bool sendHttpPostRequest(const std::wstring& ip, int port, const std::wstring& path, const std::string& data) {
        HINTERNET session = NULL;
        HINTERNET connect = NULL;
        HINTERNET request = NULL;

        // Initialize WinHTTP session
        session = WinHttpOpen(L"A WinHTTP Example Program/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, 0);

        if (!session) {
            //std::cerr << "WinHttpOpen failed with error: " << GetLastError() << std::endl;
            return false;
        }

        // Connect to the server
        connect = WinHttpConnect(session, ip.c_str(), port, 0);
        if (!connect) {
            //std::cerr << "WinHttpConnect failed with error: " << GetLastError() << std::endl;
            WinHttpCloseHandle(session);
            return false;
        }

        // Create a request handle
        request = WinHttpOpenRequest(connect, L"POST", path.c_str(),
            NULL, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            0);

        if (!request) {
            //std::cerr << "WinHttpOpenRequest failed with error: " << GetLastError() << std::endl;
            WinHttpCloseHandle(connect);
            WinHttpCloseHandle(session);
            return false;
        }

        // Set request headers
        const wchar_t* headers = L"Content-Type: application/json";
        if (!WinHttpSendRequest(request, headers, -1, (LPVOID)data.c_str(), data.size(), data.size(), 0)) {
            //std::cerr << "WinHttpSendRequest failed with error: " << GetLastError() << std::endl;
            WinHttpCloseHandle(request);
            WinHttpCloseHandle(connect);
            WinHttpCloseHandle(session);
            return false;
        }

        // Receive the response
        if (!WinHttpReceiveResponse(request, NULL)) {
            //std::cerr << "WinHttpReceiveResponse failed with error: " << GetLastError() << std::endl;
            WinHttpCloseHandle(request);
            WinHttpCloseHandle(connect);
            WinHttpCloseHandle(session);
            return false;
        }

        // Clean up handles
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connect);
        WinHttpCloseHandle(session);

        return true;
    }

    static void Rundata() {

        std::vector<std::pair<std::string, int>> playerArray;

        std::unordered_map<std::string, int> nameToIndexVehicle = {
            {"ֱ    ", 0},
            {"        ", 1},
            {" ɻ ", 2},
            {"   г ", 3},
            {"װ ׳ ", 4},
            {"    ", 5},
            {"Ƥ  ", 6},
            {"     ", 7},
            {"   ʳ ", 8},
            {"Ħ  ͧ", 9},
            {"  ͧ", 10},
            {"  ͧ", 11},
            {"Ƥ    ", 12},
            {" ܳ ", 13},
            {"    ", 14},
            {"Ħ г ", 15},
            {"ѩ ؼ   ", 16},
            {"Ƥ  ", 17},
            {"      ", 18},
            {"̤ 峵", 19},
            {"ѩ  Ħ  ", 20},
            {"ɳ س ", 21},
            {"    ", 22},
            {"˫   ܳ ", 23},
            {"ԽҰĦ  ", 24},
            {"ȫ    Ħ г ", 25},
            {"    ", 26},
            {"    ", 27},
            {"ʳƷ   䳵", 28},
            {"NEOX BLANC SUV", 29}
        };

        int vehicleIndex = 13;

        while (true)
        {
            //     ״   Ϣ
            json j;
            json i = json::array();
            json v = json::array();


            std::unordered_map<uint64_t, Player> Players = Data::GetPlayers();


            if (playerArray.size() > 4)
                playerArray.clear();

            int a = 0;
            for (auto& pair : Players)
            {
                Player& detail = pair.second;

                if (detail.bAlwaysCreatePhysicsState == 4)
                    continue;
                //     playerArray  Ƿ     detail.Name
                auto it = std::find_if(playerArray.begin(), playerArray.end(), [&detail](const std::pair<std::string, int>& p) {
                    return p.first == detail.Name;
                    });

                if (detail.IsMyTeam && !detail.IsMe) {
                    if (it == playerArray.end()) {
                        //     detail.Name          У   Ӳ   ֵ a+1
                        playerArray.push_back(std::make_pair(detail.Name, a + 1));
                        a = a + 1;
                    }
                    else {
                        //     detail.Name  Ѿ        У ʹ ö Ӧ   int ֵ
                        a = it->second;
                    }
                    //printf("Name [%s]  ID [%d]\n", detail.Name.c_str(), a);
                }


                FVector AimDirection = FRotator(0.0f, detail.AimOffsets.Yaw, 0.0f).GetUnitVector();
                FVector2D Direction = FVector2D{ AimDirection.X, AimDirection.Y };
                float AngleRadians = atan2(Direction.Y, Direction.X);
                float AngleDegrees = AngleRadians;
                float sin_a = sinf(AngleDegrees), cos_a = cosf(AngleDegrees);
                if (detail.IsMyTeam && !detail.IsMe)
                    a = a + 1;
                json playerJson = json::array({
                    detail.ClanName,
                    detail.Name,
                    detail.Location.X + GameData.Radar.WorldOriginLocation.X,
                    detail.Location.Y + GameData.Radar.WorldOriginLocation.Y,
                    cos_a,
                    sin_a,
                    255,
                    255,
                    255,
                    0,
                    255,
                    0,
                    detail.TeamID,
                    detail.Health,
                    detail.GroggyHealth,
                    detail.IsMyTeam,
                    detail.KillCount,
                    detail.SpectatedCount,
                    detail.SquadMemberIndex
                    });
                i.push_back(playerJson);
            }
            std::unordered_map<uint64_t, VehicleInfo> Vehicles = Data::GetVehicles();
            for (auto& pair : Vehicles)
            {
                VehicleInfo& detail = pair.second;

                auto it = nameToIndexVehicle.find(detail.Name);
                if (it != nameToIndexVehicle.end()) {
                    vehicleIndex = it->second;
                }
                else {
                    vehicleIndex = 13;
                }

                json playerJson = json::array({
                    //13,//       Ƕ Ӧ      ID   ֵ
                    vehicleIndex,
                    //detail.Name,
                    detail.Location.X + GameData.Radar.WorldOriginLocation.X,
                    detail.Location.Y + GameData.Radar.WorldOriginLocation.Y,
                    });
                v.push_back(playerJson);
            }
            //       JSON      j
            j["_"] = json::array({
                json::array({
                    2,
                    GameData.MapName,
                    GameData.Radar.BlueZonePosition.X,
                    GameData.Radar.BlueZonePosition.Y,
                    GameData.Radar.BlueZoneRadius,
                    GameData.Radar.SafetyZonePosition.X,
                    GameData.Radar.SafetyZonePosition.Y,
                    GameData.Radar.SafetyZoneRadius
                }),
                i,
                v
                });

            std::string data_str = j.dump();
            //if (sendHttpPostRequest(L"192.168.10.225", 7891, L"/pudm", data_str));
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring server = converter.from_bytes(GameData.Config.ESP.服务器IP);
            if (server != L"")
            {
                if (sendHttpPostRequest(server, 7891, L"/pudm", data_str));
                //std::cout << data_str.c_str() << std::endl;
            }

        }

    }

};
