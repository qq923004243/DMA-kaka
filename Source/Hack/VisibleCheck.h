//#pragma once
//#include <Common/VisibleScene.h>
//#include <Common/Data.h>
//#include <Utils/Utils.h>
//#include <Utils/Throttler.h>
//#include <mutex> // 添加互斥量头文件
//
//namespace VisibleCheck {
//
//    // 更新场景中的物体，通过范围进行加载
//    void UpdateSceneByRange() {
//        std::unordered_map<PrunerPayload, PxTransformT, PrunerPayloadHash> cache{};
//        std::set<PrunerPayload> currentSceneObjects{};
//        std::unordered_map<PrunerPayload, uint64_t, PrunerPayloadHash> alwaysCheckShape{};
//        uint32_t lastUpdateTimestamp = 0;
//        Throttler throttler;
//
//        while (GameData.Scene == Scene::Gaming) {
//            throttler.executeTaskWithSleep("UpdateSceneByRangeSleep", std::chrono::milliseconds(GameData.Config.ESP.PhysxStaticRefreshInterval), [&cache, &currentSceneObjects, &lastUpdateTimestamp, &alwaysCheckShape] {
//                std::set<PrunerPayload> willRemoveObjects{};
//                FVector currentPosition = GameData.Camera.Location + GameData.Radar.WorldOriginLocation;
//
//                // 如果当前位置几乎为零，直接返回
//                if (currentPosition.IsNearlyZero()) {
//                    return;
//                }
//
//#ifdef _PHYSX_DEBUG
//                auto start = std::chrono::high_resolution_clock::now();
//#endif
//                // 加载当前范围内的物体
//                auto Meshs = physx::LoadShapeByRange(
//                    lastUpdateTimestamp,
//                    cache,
//                    currentSceneObjects,
//                    willRemoveObjects,
//                    alwaysCheckShape,
//                    { currentPosition.X, currentPosition.Y, currentPosition.Z },
//                    GameData.Config.ESP.PhysxLoadRadius * 100.f,
//                    GameData.Config.ESP.PhysxRefreshLimit
//                );
//
//                // 如果有物体被加载或移除
//                if (!Meshs.empty() || !willRemoveObjects.empty()) {
//#ifdef _PHYSX_DEBUG
//                    auto end = std::chrono::high_resolution_clock::now();
//                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//                    Utils::Log(1, "Static: Load %d meshs cost %d ms", Meshs.size(), duration.count());
//#endif
//                    try {
//                        // 检查DynamicLoadScene是否为null
//                        if (GameData.DynamicLoadScene == nullptr) {
//                            Utils::Log(3, "Static: DynamicLoadScene is null, cannot update mesh.");
//                            return;
//                        }
//
//                        // 使用std::lock_guard锁住互斥量，确保线程安全
//                        std::lock_guard<std::mutex> lock(GameData.gameDataMutex);
//                        GameData.DynamicLoadScene->UpdateMesh(Meshs, willRemoveObjects);
//                    }
//                    catch (const std::exception& e) {
//                        Utils::Log(3, "Static: Update Mesh error: %s", e.what());
//                    }
//#ifdef _PHYSX_DEBUG
//                    end = std::chrono::high_resolution_clock::now();
//                    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//                    Utils::Log(3, "Static: Update embree scene cost %d ms", duration.count());
//#endif
//                }
//                });
//
//        }
//    }
//
//    // 更新动态高度场
//    void UpdateDynamicHeightField() {
//        std::set<PrunerPayload> UniqueSet{};
//        std::set<PrunerPayload> HeightFieldSet{};
//        std::set<uint64_t> HeightFieldSamplePtrSet{};
//        uint32_t lastUpdateTimestamp = 0;
//        Throttler throttler;
//
//        while (GameData.Scene == Scene::Gaming) {
//            throttler.executeTaskWithSleep("HeightFieldUpdateSleep", std::chrono::milliseconds(GameData.Config.ESP.PhysxDynamicRefreshInterva2), [&UniqueSet, &HeightFieldSet, &HeightFieldSamplePtrSet, &lastUpdateTimestamp] {
//                std::set<uint64_t> RemoveHeightFieldKey{};
//#ifdef _PHYSX_DEBUG
//                auto start = std::chrono::high_resolution_clock::now();
//#endif
//                // 刷新动态加载的高度场
//                auto Meshs = physx::RefreshDynamicLoadHeightField(
//                    lastUpdateTimestamp, UniqueSet,
//                    HeightFieldSet,
//                    HeightFieldSamplePtrSet,
//                    RemoveHeightFieldKey
//                );
//
//                // 如果有物体被加载或移除
//                if (!Meshs.empty() || !RemoveHeightFieldKey.empty()) {
//#ifdef _PHYSX_DEBUG
//                    auto end = std::chrono::high_resolution_clock::now();
//                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//                    Utils::Log(1, "Load %d meshs cost %d ms", Meshs.size(), duration.count());
//#endif
//                    try {
//                        // 检查 HeightFieldScene 是否为 null
//                        if (GameData.HeightFieldScene == nullptr) {
//                            Utils::Log(3, "HeightField: HeightFieldScene is null, cannot update mesh.");
//                            return;
//                        }
//
//                        // 使用 std::lock_guard 锁住互斥量，确保线程安全
//                        std::lock_guard<std::mutex> lock(GameData.gameDataMutex);
//                        GameData.HeightFieldScene->UpdateMesh(Meshs, RemoveHeightFieldKey);
//                    }
//                    catch (const std::exception& e) {
//                        Utils::Log(3, "Update Height Field Mesh error: %s", e.what());
//                    }
//
//#ifdef _PHYSX_DEBUG
//                    end = std::chrono::high_resolution_clock::now();
//                    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//                    Utils::Log(3, "Update embree scene cost %d ms", duration.count());
//#endif
//                }
//                });
//        }
//    }
//
//    // 更新动态刚体
//    void UpdateDynamicRigid() {
//        Throttler throttler;
//        std::unordered_map<PrunerPayload, PxTransformT, PrunerPayloadHash> cache{};
//        std::unordered_map<PrunerPayload, uint64_t, PrunerPayloadHash> ptrCache{};
//        std::set<PrunerPayload> currentSceneObjects{};
//
//        while (GameData.Scene == Scene::Gaming) {
//            throttler.executeTaskWithSleep("DynamicRigidUpdateSleep", std::chrono::milliseconds(GameData.Config.ESP.PhysxDynamicRefreshInterval), [&currentSceneObjects, &cache, &ptrCache] {
//                FVector currentPosition = GameData.Camera.Location + GameData.Radar.WorldOriginLocation;
//
//                // 如果当前位置几乎为零，直接返回
//                if (currentPosition.IsNearlyZero()) {
//                    return;
//                }
//
//                std::set<PrunerPayload> willRemoveShape{};
//#ifdef _PHYSX_DEBUG
//                auto start = std::chrono::high_resolution_clock::now();
//#endif
//                // 加载动态刚体
//                auto Meshs = physx::LoadDynamicRigidShape(
//                    currentSceneObjects,
//                    cache,
//                    ptrCache,
//                    willRemoveShape,
//                    { currentPosition.X, currentPosition.Y, currentPosition.Z },
//                    GameData.Config.ESP.PhysxLoadRadius * 100.f
//                );
//
//                // 如果有动态物体被加载或移除
//                if (!Meshs.empty() || !willRemoveShape.empty()) {
//#ifdef _PHYSX_DEBUG
//                    auto end = std::chrono::high_resolution_clock::now();
//                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//                    Utils::Log(1, "Dynamic: Load %d meshs cost %d ms", Meshs.size(), duration.count());
//#endif
//                    try {
//                        // 检查DynamicRigidScene是否为null
//                        if (GameData.DynamicRigidScene == nullptr) {
//                            Utils::Log(3, "Dynamic: DynamicRigidScene is null, cannot update mesh.");
//                            return;
//                        }
//
//                        // 使用std::lock_guard锁住互斥量，确保线程安全
//                        std::lock_guard<std::mutex> lock(GameData.gameDataMutex);
//                        GameData.DynamicRigidScene->UpdateMesh(Meshs, willRemoveShape);
//                    }
//                    catch (const std::exception& e) {
//                        Utils::Log(3, "Dynamic: Update Mesh error: %s", e.what());
//                    }
//
//#ifdef _PHYSX_DEBUG
//                    end = std::chrono::high_resolution_clock::now();
//                    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//                    Utils::Log(3, "Dynamic: Update embree scene cost %d ms", duration.count());
//#endif
//                }
//                });
//
//        }
//    }
//}








#pragma once
#include <Common/VisibleScene.h>
#include <Common/Data.h>
#include <Utils/Utils.h>
#include <Utils/Throttler.h>
#include <mutex> // 添加互斥量头文件

namespace VisibleCheck {

    // 更新场景中的物体，通过范围进行加载
    void UpdateSceneByRange() {
        std::unordered_map<PrunerPayload, PxTransformT, PrunerPayloadHash> cache{};
        std::set<PrunerPayload> currentSceneObjects{};
        std::unordered_map<PrunerPayload, uint64_t, PrunerPayloadHash> alwaysCheckShape{};
        uint32_t lastUpdateTimestamp = 0;
        Throttler throttler;

        while (GameData.Scene == Scene::Gaming) {
            throttler.executeTaskWithSleep("UpdateSceneByRangeSleep", std::chrono::milliseconds(GameData.Config.ESP.PhysxStaticRefreshInterval), [&cache, &currentSceneObjects, &lastUpdateTimestamp, &alwaysCheckShape] {
                std::set<PrunerPayload> willRemoveObjects{};
                FVector currentPosition = GameData.Camera.Location + GameData.Radar.WorldOriginLocation;

                // 如果当前位置几乎为零，直接返回
                if (currentPosition.IsNearlyZero()) {
                    return;
                }

#ifdef _PHYSX_DEBUG
                auto start = std::chrono::high_resolution_clock::now();
#endif
                // 加载当前范围内的物体
                auto Meshs = physx::LoadShapeByRange(
                    lastUpdateTimestamp,
                    cache,
                    currentSceneObjects,
                    willRemoveObjects,
                    alwaysCheckShape,
                    { currentPosition.X, currentPosition.Y, currentPosition.Z },
                    GameData.Config.ESP.PhysxLoadRadius * 100.f,
                    GameData.Config.ESP.PhysxRefreshLimit
                );

                // 如果有物体被加载或移除
                if (!Meshs.empty() || !willRemoveObjects.empty()) {
#ifdef _PHYSX_DEBUG
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    Utils::Log(1, "Static: Load %d meshs cost %d ms", Meshs.size(), duration.count());
#endif
                    try {
                        // 检查DynamicLoadScene是否为null
                        if (GameData.DynamicLoadScene == nullptr) {
                            Utils::Log(3, "Static: DynamicLoadScene is null, cannot update mesh.");
                            return;
                        }

                        // 使用std::lock_guard锁住互斥量，确保线程安全
                        std::lock_guard<std::mutex> lock(GameData.gameDataMutex);
                        GameData.DynamicLoadScene->UpdateMesh(Meshs, willRemoveObjects);
                    }
                    catch (const std::exception& e) {
                        Utils::Log(3, "Static: Update Mesh error: %s", e.what());
                    }
#ifdef _PHYSX_DEBUG
                    end = std::chrono::high_resolution_clock::now();
                    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    Utils::Log(3, "Static: Update embree scene cost %d ms", duration.count());
#endif
                }
                });

        }
    }

    // 更新动态高度场
    void UpdateDynamicHeightField() {
        std::set<PrunerPayload> UniqueSet{};
        std::set<PrunerPayload> HeightFieldSet{};
        std::set<uint64_t> HeightFieldSamplePtrSet{};
        uint32_t lastUpdateTimestamp = 0;
        Throttler throttler;

        while (GameData.Scene == Scene::Gaming) {
            throttler.executeTaskWithSleep("HeightFieldUpdateSleep", std::chrono::milliseconds(GameData.Config.ESP.PhysxDynamicRefreshInterva2), [&UniqueSet, &HeightFieldSet, &HeightFieldSamplePtrSet, &lastUpdateTimestamp] {
                std::set<uint64_t> RemoveHeightFieldKey{};
#ifdef _PHYSX_DEBUG
                auto start = std::chrono::high_resolution_clock::now();
#endif
                // 刷新动态加载的高度场
                auto Meshs = physx::RefreshDynamicLoadHeightField(
                    lastUpdateTimestamp, UniqueSet,
                    HeightFieldSet,
                    HeightFieldSamplePtrSet,
                    RemoveHeightFieldKey
                );

                // 如果有物体被加载或移除
                if (!Meshs.empty() || !RemoveHeightFieldKey.empty()) {
#ifdef _PHYSX_DEBUG
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    Utils::Log(1, "Load %d meshs cost %d ms", Meshs.size(), duration.count());
#endif
                    try {
                        // 检查 HeightFieldScene 是否为 null
                        if (GameData.HeightFieldScene == nullptr) {
                            Utils::Log(3, "HeightField: HeightFieldScene is null, cannot update mesh.");
                            return;
                        }

                        // 使用 std::lock_guard 锁住互斥量，确保线程安全
                        std::lock_guard<std::mutex> lock(GameData.gameDataMutex);
                        GameData.HeightFieldScene->UpdateMesh(Meshs, RemoveHeightFieldKey);
                    }
                    catch (const std::exception& e) {
                        Utils::Log(3, "Update Height Field Mesh error: %s", e.what());
                    }

#ifdef _PHYSX_DEBUG
                    end = std::chrono::high_resolution_clock::now();
                    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    Utils::Log(3, "Update embree scene cost %d ms", duration.count());
#endif
                }
                });
        }
    }

    // 更新动态刚体
    void UpdateDynamicRigid() {
        Throttler throttler;
        std::unordered_map<PrunerPayload, PxTransformT, PrunerPayloadHash> cache{};
        std::unordered_map<PrunerPayload, uint64_t, PrunerPayloadHash> ptrCache{};
        std::set<PrunerPayload> currentSceneObjects{};

        while (GameData.Scene == Scene::Gaming) {
            throttler.executeTaskWithSleep("DynamicRigidUpdateSleep", std::chrono::milliseconds(GameData.Config.ESP.PhysxDynamicRefreshInterval), [&currentSceneObjects, &cache, &ptrCache] {
                FVector currentPosition = GameData.Camera.Location + GameData.Radar.WorldOriginLocation;

                // 如果当前位置几乎为零，直接返回
                if (currentPosition.IsNearlyZero()) {
                    return;
                }

                std::set<PrunerPayload> willRemoveShape{};
#ifdef _PHYSX_DEBUG
                auto start = std::chrono::high_resolution_clock::now();
#endif
                // 加载动态刚体
                auto Meshs = physx::LoadDynamicRigidShape(
                    currentSceneObjects,
                    cache,
                    ptrCache,
                    willRemoveShape,
                    { currentPosition.X, currentPosition.Y, currentPosition.Z },
                    GameData.Config.ESP.PhysxLoadRadius * 100.f
                );

                // 如果有动态物体被加载或移除
                if (!Meshs.empty() || !willRemoveShape.empty()) {
#ifdef _PHYSX_DEBUG
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    Utils::Log(1, "Dynamic: Load %d meshs cost %d ms", Meshs.size(), duration.count());
#endif
                    try {
                        // 检查DynamicRigidScene是否为null
                        if (GameData.DynamicRigidScene == nullptr) {
                            Utils::Log(3, "Dynamic: DynamicRigidScene is null, cannot update mesh.");
                            return;
                        }

                        // 使用std::lock_guard锁住互斥量，确保线程安全
                        std::lock_guard<std::mutex> lock(GameData.gameDataMutex);
                        GameData.DynamicRigidScene->UpdateMesh(Meshs, willRemoveShape);
                    }
                    catch (const std::exception& e) {
                        Utils::Log(3, "Dynamic: Update Mesh error: %s", e.what());
                    }

#ifdef _PHYSX_DEBUG
                    end = std::chrono::high_resolution_clock::now();
                    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    Utils::Log(3, "Dynamic: Update embree scene cost %d ms", duration.count());
#endif
                }
                });

        }
    }
}

