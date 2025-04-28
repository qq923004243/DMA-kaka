//#pragma once
//#include <Common/VisibleScene.h>
//#include <Common/Data.h>
//#include <Utils/Utils.h>
//#include <Utils/Throttler.h>
//#include <mutex> // ��ӻ�����ͷ�ļ�
//
//namespace VisibleCheck {
//
//    // ���³����е����壬ͨ����Χ���м���
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
//                // �����ǰλ�ü���Ϊ�㣬ֱ�ӷ���
//                if (currentPosition.IsNearlyZero()) {
//                    return;
//                }
//
//#ifdef _PHYSX_DEBUG
//                auto start = std::chrono::high_resolution_clock::now();
//#endif
//                // ���ص�ǰ��Χ�ڵ�����
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
//                // ��������屻���ػ��Ƴ�
//                if (!Meshs.empty() || !willRemoveObjects.empty()) {
//#ifdef _PHYSX_DEBUG
//                    auto end = std::chrono::high_resolution_clock::now();
//                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//                    Utils::Log(1, "Static: Load %d meshs cost %d ms", Meshs.size(), duration.count());
//#endif
//                    try {
//                        // ���DynamicLoadScene�Ƿ�Ϊnull
//                        if (GameData.DynamicLoadScene == nullptr) {
//                            Utils::Log(3, "Static: DynamicLoadScene is null, cannot update mesh.");
//                            return;
//                        }
//
//                        // ʹ��std::lock_guard��ס��������ȷ���̰߳�ȫ
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
//    // ���¶�̬�߶ȳ�
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
//                // ˢ�¶�̬���صĸ߶ȳ�
//                auto Meshs = physx::RefreshDynamicLoadHeightField(
//                    lastUpdateTimestamp, UniqueSet,
//                    HeightFieldSet,
//                    HeightFieldSamplePtrSet,
//                    RemoveHeightFieldKey
//                );
//
//                // ��������屻���ػ��Ƴ�
//                if (!Meshs.empty() || !RemoveHeightFieldKey.empty()) {
//#ifdef _PHYSX_DEBUG
//                    auto end = std::chrono::high_resolution_clock::now();
//                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//                    Utils::Log(1, "Load %d meshs cost %d ms", Meshs.size(), duration.count());
//#endif
//                    try {
//                        // ��� HeightFieldScene �Ƿ�Ϊ null
//                        if (GameData.HeightFieldScene == nullptr) {
//                            Utils::Log(3, "HeightField: HeightFieldScene is null, cannot update mesh.");
//                            return;
//                        }
//
//                        // ʹ�� std::lock_guard ��ס��������ȷ���̰߳�ȫ
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
//    // ���¶�̬����
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
//                // �����ǰλ�ü���Ϊ�㣬ֱ�ӷ���
//                if (currentPosition.IsNearlyZero()) {
//                    return;
//                }
//
//                std::set<PrunerPayload> willRemoveShape{};
//#ifdef _PHYSX_DEBUG
//                auto start = std::chrono::high_resolution_clock::now();
//#endif
//                // ���ض�̬����
//                auto Meshs = physx::LoadDynamicRigidShape(
//                    currentSceneObjects,
//                    cache,
//                    ptrCache,
//                    willRemoveShape,
//                    { currentPosition.X, currentPosition.Y, currentPosition.Z },
//                    GameData.Config.ESP.PhysxLoadRadius * 100.f
//                );
//
//                // ����ж�̬���屻���ػ��Ƴ�
//                if (!Meshs.empty() || !willRemoveShape.empty()) {
//#ifdef _PHYSX_DEBUG
//                    auto end = std::chrono::high_resolution_clock::now();
//                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//                    Utils::Log(1, "Dynamic: Load %d meshs cost %d ms", Meshs.size(), duration.count());
//#endif
//                    try {
//                        // ���DynamicRigidScene�Ƿ�Ϊnull
//                        if (GameData.DynamicRigidScene == nullptr) {
//                            Utils::Log(3, "Dynamic: DynamicRigidScene is null, cannot update mesh.");
//                            return;
//                        }
//
//                        // ʹ��std::lock_guard��ס��������ȷ���̰߳�ȫ
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
#include <mutex> // ��ӻ�����ͷ�ļ�

namespace VisibleCheck {

    // ���³����е����壬ͨ����Χ���м���
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

                // �����ǰλ�ü���Ϊ�㣬ֱ�ӷ���
                if (currentPosition.IsNearlyZero()) {
                    return;
                }

#ifdef _PHYSX_DEBUG
                auto start = std::chrono::high_resolution_clock::now();
#endif
                // ���ص�ǰ��Χ�ڵ�����
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

                // ��������屻���ػ��Ƴ�
                if (!Meshs.empty() || !willRemoveObjects.empty()) {
#ifdef _PHYSX_DEBUG
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    Utils::Log(1, "Static: Load %d meshs cost %d ms", Meshs.size(), duration.count());
#endif
                    try {
                        // ���DynamicLoadScene�Ƿ�Ϊnull
                        if (GameData.DynamicLoadScene == nullptr) {
                            Utils::Log(3, "Static: DynamicLoadScene is null, cannot update mesh.");
                            return;
                        }

                        // ʹ��std::lock_guard��ס��������ȷ���̰߳�ȫ
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

    // ���¶�̬�߶ȳ�
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
                // ˢ�¶�̬���صĸ߶ȳ�
                auto Meshs = physx::RefreshDynamicLoadHeightField(
                    lastUpdateTimestamp, UniqueSet,
                    HeightFieldSet,
                    HeightFieldSamplePtrSet,
                    RemoveHeightFieldKey
                );

                // ��������屻���ػ��Ƴ�
                if (!Meshs.empty() || !RemoveHeightFieldKey.empty()) {
#ifdef _PHYSX_DEBUG
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    Utils::Log(1, "Load %d meshs cost %d ms", Meshs.size(), duration.count());
#endif
                    try {
                        // ��� HeightFieldScene �Ƿ�Ϊ null
                        if (GameData.HeightFieldScene == nullptr) {
                            Utils::Log(3, "HeightField: HeightFieldScene is null, cannot update mesh.");
                            return;
                        }

                        // ʹ�� std::lock_guard ��ס��������ȷ���̰߳�ȫ
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

    // ���¶�̬����
    void UpdateDynamicRigid() {
        Throttler throttler;
        std::unordered_map<PrunerPayload, PxTransformT, PrunerPayloadHash> cache{};
        std::unordered_map<PrunerPayload, uint64_t, PrunerPayloadHash> ptrCache{};
        std::set<PrunerPayload> currentSceneObjects{};

        while (GameData.Scene == Scene::Gaming) {
            throttler.executeTaskWithSleep("DynamicRigidUpdateSleep", std::chrono::milliseconds(GameData.Config.ESP.PhysxDynamicRefreshInterval), [&currentSceneObjects, &cache, &ptrCache] {
                FVector currentPosition = GameData.Camera.Location + GameData.Radar.WorldOriginLocation;

                // �����ǰλ�ü���Ϊ�㣬ֱ�ӷ���
                if (currentPosition.IsNearlyZero()) {
                    return;
                }

                std::set<PrunerPayload> willRemoveShape{};
#ifdef _PHYSX_DEBUG
                auto start = std::chrono::high_resolution_clock::now();
#endif
                // ���ض�̬����
                auto Meshs = physx::LoadDynamicRigidShape(
                    currentSceneObjects,
                    cache,
                    ptrCache,
                    willRemoveShape,
                    { currentPosition.X, currentPosition.Y, currentPosition.Z },
                    GameData.Config.ESP.PhysxLoadRadius * 100.f
                );

                // ����ж�̬���屻���ػ��Ƴ�
                if (!Meshs.empty() || !willRemoveShape.empty()) {
#ifdef _PHYSX_DEBUG
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    Utils::Log(1, "Dynamic: Load %d meshs cost %d ms", Meshs.size(), duration.count());
#endif
                    try {
                        // ���DynamicRigidScene�Ƿ�Ϊnull
                        if (GameData.DynamicRigidScene == nullptr) {
                            Utils::Log(3, "Dynamic: DynamicRigidScene is null, cannot update mesh.");
                            return;
                        }

                        // ʹ��std::lock_guard��ס��������ȷ���̰߳�ȫ
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

