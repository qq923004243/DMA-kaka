//#pragma once
//#include <DMALibrary/Memory/Memory.h>
//#include <Common/Data.h>
//#include <Common/Entitys.h>
//#include <Utils/Utils.h>
//#include <Utils/Throttler.h>
//#include <Hack/GNames.h>
//#include <Hack/Decrypt.h>
//#include <Common/VisibleScene.h>
//
//namespace LineTrace
//{
//    static bool LineTraceSingle(FVector TraceStart, FVector TraceEnd)
//    {
//        if (GameData.Scene != Scene::Gaming || GameData.DynamicLoadScene == nullptr || GameData.HeightFieldScene == nullptr || GameData.DynamicRigidScene == nullptr) {
//            return true;
//        }
//        FVector origin = TraceStart + GameData.Radar.WorldOriginLocation;
//        FVector target = TraceEnd + GameData.Radar.WorldOriginLocation;
//        auto dynamicRayHit = GameData.DynamicLoadScene->Raycast(origin, target);
//        if (dynamicRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
//            return false;
//        }
//        auto heightFieldRayHit = GameData.HeightFieldScene->Raycast(origin, target);
//        if (heightFieldRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
//            return false;
//        }
//        auto globalSceneRayHit = GameData.DynamicRigidScene->Raycast(origin, target);
//        if (globalSceneRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
//            return false;
//        }
//        return true;
//    }
//
//    static TriangleMeshData* getNextHint() {
//        if (GameData.Scene != Scene::Gaming || GameData.DynamicLoadScene == nullptr || GameData.HeightFieldScene == nullptr || GameData.DynamicRigidScene == nullptr) {
//            return nullptr;
//        }
//        FVector forwardVector;
//        forwardVector.X = cos(GameData.Camera.Rotation.Yaw * M_PI / 180.0f) * cos(GameData.Camera.Rotation.Pitch * M_PI / 180.0f);
//        forwardVector.Y = sin(GameData.Camera.Rotation.Yaw * M_PI / 180.0f) * cos(GameData.Camera.Rotation.Pitch * M_PI / 180.0f);
//        forwardVector.Z = sin(GameData.Camera.Rotation.Pitch * M_PI / 180.0f);
//        FVector origin = GameData.Camera.Location + GameData.Radar.WorldOriginLocation;
//        FVector target = origin + forwardVector * 100000.0f;
//        auto dynamicRayHit = GameData.DynamicLoadScene->Raycast(origin, target);
//        auto heightFieldRayHit = GameData.HeightFieldScene->Raycast(origin, target);
//        auto globalSceneRayHit = GameData.DynamicRigidScene->Raycast(origin, target);
//
//        if (dynamicRayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID &&
//            heightFieldRayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID &&
//            globalSceneRayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
//            return nullptr;
//        }
//
//        // 如果有多个碰撞,返回最近的
//        float minDist = FLT_MAX;
//        TriangleMeshData* result = nullptr;
//
//        if (dynamicRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
//            if (dynamicRayHit.ray.tfar < minDist) {
//                minDist = dynamicRayHit.ray.tfar;
//                result = GameData.DynamicLoadScene->GetGeomeoryData(dynamicRayHit.hit.geomID);
//            }
//        }
//
//        if (heightFieldRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
//            if (heightFieldRayHit.ray.tfar < minDist) {
//                minDist = heightFieldRayHit.ray.tfar;
//                result = GameData.HeightFieldScene->GetGeomeoryData(heightFieldRayHit.hit.geomID);
//            }
//        }
//
//        if (globalSceneRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
//            if (globalSceneRayHit.ray.tfar < minDist) {
//                minDist = globalSceneRayHit.ray.tfar;
//                result = GameData.DynamicRigidScene->GetGeomeoryData(globalSceneRayHit.hit.geomID);
//            }
//        }
//
//        return result;
//    }
//}




#pragma once
#include <DMALibrary/Memory/Memory.h>
#include <Common/Data.h>
#include <Common/Entitys.h>
#include <Utils/Utils.h>
#include <Utils/Throttler.h>
#include <Hack/GNames.h>
#include <Hack/Decrypt.h>
#include <Common/VisibleScene.h>
#include "Hack/MeshPatcher.h"

namespace LineTrace
{

    // 定义一个静态的排除模型集合
    static const std::unordered_set<std::string> excludedModels = {
        "1259_6486", "1999_9570", "443_2061", "428_1998", "449_2088", "8_36"
    };


    static bool LineTraceSingle(FVector TraceStart, FVector TraceEnd)
    {
        try {
            // 确保场景有效
            if (GameData.Scene != Scene::Gaming ||
                GameData.DynamicLoadScene == nullptr ||
                GameData.HeightFieldScene == nullptr ||
                GameData.DynamicRigidScene == nullptr) {
                return true;
            }

            FVector origin = TraceStart + GameData.Radar.WorldOriginLocation;
            FVector target = TraceEnd + GameData.Radar.WorldOriginLocation;

            // 添加验证，确保向量不是无穷大或NaN
            if (origin.IsNearlyZero() || !std::isfinite(origin.X) || !std::isfinite(origin.Y) || !std::isfinite(origin.Z) ||
                !std::isfinite(target.X) || !std::isfinite(target.Y) || !std::isfinite(target.Z)) {
                return true;
            }

            // 尝试动态加载场景射线检测
            RTCRayHit dynamicRayHit = {};
            dynamicRayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
            if (GameData.DynamicLoadScene) {
                dynamicRayHit = GameData.DynamicLoadScene->Raycast(origin, target);
                if (dynamicRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                    return false;
                }
            }

            // 尝试高度场景射线检测
            RTCRayHit heightFieldRayHit = {};
            heightFieldRayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
            if (GameData.HeightFieldScene) {
                heightFieldRayHit = GameData.HeightFieldScene->Raycast(origin, target);
                if (heightFieldRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                    return false;
                }
            }

            // 尝试动态刚体场景射线检测
            RTCRayHit globalSceneRayHit = {};
            globalSceneRayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
            if (GameData.DynamicRigidScene) {
                globalSceneRayHit = GameData.DynamicRigidScene->Raycast(origin, target);
                if (globalSceneRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                    return false;
                }
            }

            return true;
        }
        catch (const std::exception& e) {
            Utils::Log(2, "LineTraceSingle exception: %s", e.what());
            return true;
        }
        catch (...) {
            Utils::Log(2, "LineTraceSingle unknown exception");
            return true;
        }
    }

    static TriangleMeshData* getNextHint() {
        try {
            if (GameData.Scene != Scene::Gaming ||
                GameData.DynamicLoadScene == nullptr ||
                GameData.HeightFieldScene == nullptr ||
                GameData.DynamicRigidScene == nullptr) {
                return nullptr;
            }

            // 计算前向向量
            FVector forwardVector;
            forwardVector.X = cos(GameData.Camera.Rotation.Yaw * M_PI / 180.0f) * cos(GameData.Camera.Rotation.Pitch * M_PI / 180.0f);
            forwardVector.Y = sin(GameData.Camera.Rotation.Yaw * M_PI / 180.0f) * cos(GameData.Camera.Rotation.Pitch * M_PI / 180.0f);
            forwardVector.Z = sin(GameData.Camera.Rotation.Pitch * M_PI / 180.0f);

            // 检查向量是否有效
            if (forwardVector.IsNearlyZero() ||
                !std::isfinite(forwardVector.X) ||
                !std::isfinite(forwardVector.Y) ||
                !std::isfinite(forwardVector.Z)) {
                return nullptr;
            }

            FVector origin = GameData.Camera.Location + GameData.Radar.WorldOriginLocation;
            FVector target = origin + forwardVector * 100000.0f;

            if (!std::isfinite(origin.X) || !std::isfinite(origin.Y) || !std::isfinite(origin.Z) ||
                !std::isfinite(target.X) || !std::isfinite(target.Y) || !std::isfinite(target.Z)) {
                return nullptr;
            }

            // 初始化射线碰撞结果
            RTCRayHit dynamicRayHit = {};
            RTCRayHit heightFieldRayHit = {};
            RTCRayHit globalSceneRayHit = {};

            dynamicRayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
            heightFieldRayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
            globalSceneRayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

            // 执行射线检测，并捕获可能的异常
            if (GameData.DynamicLoadScene) {
                dynamicRayHit = GameData.DynamicLoadScene->Raycast(origin, target);
            }

            if (GameData.HeightFieldScene) {
                heightFieldRayHit = GameData.HeightFieldScene->Raycast(origin, target);
            }

            if (GameData.DynamicRigidScene) {
                globalSceneRayHit = GameData.DynamicRigidScene->Raycast(origin, target);
            }

            // 如果都没有碰撞，返回nullptr
            if (dynamicRayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID &&
                heightFieldRayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID &&
                globalSceneRayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
                return nullptr;
            }

            float minDist = FLT_MAX;
            TriangleMeshData* result = nullptr;

            // 检查动态加载场景
            if (dynamicRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                TriangleMeshData* meshData = GameData.DynamicLoadScene->GetGeomeoryData(dynamicRayHit.hit.geomID);
                if (meshData && meshData->Vertices.size() > 0 && meshData->Indices.size() > 0) {
                    std::string keyStr = std::format("{}_{}", meshData->Vertices.size(), meshData->Indices.size());
                    if (excludedModels.find(keyStr) == excludedModels.end()) { // 不在排除列表中
                        if (dynamicRayHit.ray.tfar < minDist) {
                            minDist = dynamicRayHit.ray.tfar;
                            result = meshData;
                        }
                    }
                }
            }

            // 检查地形场景
            if (heightFieldRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                TriangleMeshData* meshData = GameData.HeightFieldScene->GetGeomeoryData(heightFieldRayHit.hit.geomID);
                if (meshData && meshData->Vertices.size() > 0 && meshData->Indices.size() > 0) {
                    std::string keyStr = std::format("{}_{}", meshData->Vertices.size(), meshData->Indices.size());
                    if (excludedModels.find(keyStr) == excludedModels.end()) { // 不在排除列表中
                        if (heightFieldRayHit.ray.tfar < minDist) {
                            minDist = heightFieldRayHit.ray.tfar;
                            result = meshData;
                        }
                    }
                }
            }

            // 检查全局刚体场景
            if (globalSceneRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                TriangleMeshData* meshData = GameData.DynamicRigidScene->GetGeomeoryData(globalSceneRayHit.hit.geomID);
                if (meshData && meshData->Vertices.size() > 0 && meshData->Indices.size() > 0) {
                    std::string keyStr = std::format("{}_{}", meshData->Vertices.size(), meshData->Indices.size());
                    if (excludedModels.find(keyStr) == excludedModels.end()) { // 不在排除列表中
                        if (globalSceneRayHit.ray.tfar < minDist) {
                            minDist = globalSceneRayHit.ray.tfar;
                            result = meshData;
                        }
                    }
                }
            }

            return result;
        }
        catch (const std::exception& e) {
            Utils::Log(2, "getNextHint exception: %s", e.what());
            return nullptr;
        }
        catch (...) {
            Utils::Log(2, "getNextHint unknown exception");
            return nullptr;
        }
    }

}
