#pragma once
#include <DMALibrary/Memory/Memory.h>
#include "common/Data.h"
#include "common/Entitys.h"
#include "utils/KmBox.h"
#include "utils/Lurker.h"
#include "utils/KmBoxNet.h"
#include "utils/MoBox.h"
#include <Hack/LineTraceHook.h>
#include <set>
#define MAX_flt			(3.402823466e+38F)
#define SMALL_NUMBER		(1.e-8f)

void TriggerMouseLeft() {
    AimBotConfig Config;
    kmNet_mouse_left(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(Config.Delay1));
    kmNet_mouse_left(0);
}

Vector3 LocalPos;
float AIM_Dis;
Vector3 AIM_V3;
Vector2 AIM_V2;
bool isConnectKMB = false;


float Fov;
float Tangent;
Vector3 Pos, Rot, AxisX, AxisY, AxisZ;
Vector3 sway, recoil, vAxisX, vAxisY, vAxisZ;
ULONG GameCenterW;
ULONG GameCenterH;
ULONG GameCenterX;
ULONG GameCenterY;


constexpr inline auto NAME_None = FName(0, 0);
constexpr inline auto INDEX_NONE = -1;
WeaponData CurrentWeaponData;
float RemainMouseX = 0.0f;
float RemainMouseY = 0.0f;
float AutoSwitchTargetStartTime = 0;
float RecoilTimeStartTime = 0;
FVector RecoilLocation;
float LineTraceSingleRecoilTimeStartTime = 0;
FVector LineTraceSingleRecoilLocation;
uint64_t LastCurrentWeapon = 0;

float ZSave[10];
int ZCount = 0;

float recoilTime = 0.0f;
uint64_t LastActor = 0;
bool bIsSaveComplete = false;
class AimBot
{
public:
    static void StopAiming(bool UseSleep = true)
    {
        //if (UseSleep) Sleep(1);
        GameData.AimBot.Lock = false;
        GameData.AimBot.Target = 0;
        RemainMouseX = 0.f;
        RemainMouseY = 0.f;
        AutoSwitchTargetStartTime = 0;
        RecoilTimeStartTime = 0;
        RecoilLocation = { 0.f, 0.f, 0.f };
        LineTraceSingleRecoilTimeStartTime = 0;
        LineTraceSingleRecoilLocation = { 0.f, 0.f, 0.f };
        LastCurrentWeapon = 0;
        Data::SetEnemyInfoMap({});
    }

    static void CycleTime(float MinTime, float MaxTime, float& InTime, int& CycleCount)
    {
        float InitTime = InTime;
        float Duration = MaxTime - MinTime;

        if (InTime > MaxTime)
        {
            CycleCount = FloorToInt((MaxTime - InTime) / Duration);
            InTime = InTime + Duration * CycleCount;
        }
        else if (InTime < MinTime)
        {
            CycleCount = FloorToInt((InTime - MinTime) / Duration);
            InTime = InTime - Duration * CycleCount;
        }

        if (InTime == MaxTime && InitTime < MinTime)
        {
            InTime = MinTime;
        }

        if (InTime == MinTime && InitTime > MaxTime)
        {
            InTime = MaxTime;
        }

        CycleCount = Abs(CycleCount);
    }

    static void RemapTimeValue(float& InTime, float& CycleValueOffset, FRichCurve RichCurve, int KeysNum, std::vector<FRichCurveKey> Keys)
    {
        const int32 NumKeys = KeysNum;

        if (NumKeys < 2)
        {
            return;
        }

        if (InTime <= Keys[0].Time)
        {
            if (RichCurve.PreInfinityExtrap != RCCE_Linear && RichCurve.PreInfinityExtrap != RCCE_Constant)
            {
                float MinTime = Keys[0].Time;
                float MaxTime = Keys[NumKeys - 1].Time;

                int CycleCount = 0;
                CycleTime(MinTime, MaxTime, InTime, CycleCount);

                if (RichCurve.PreInfinityExtrap == RCCE_CycleWithOffset)
                {
                    float DV = Keys[0].Value - Keys[NumKeys - 1].Value;
                    CycleValueOffset = DV * CycleCount;
                }
                else if (RichCurve.PreInfinityExtrap == RCCE_Oscillate)
                {
                    if (CycleCount % 2 == 1)
                    {
                        InTime = MinTime + (MaxTime - InTime);
                    }
                }
            }
        }
        else if (InTime >= Keys[NumKeys - 1].Time)
        {
            if (RichCurve.PostInfinityExtrap != RCCE_Linear && RichCurve.PostInfinityExtrap != RCCE_Constant)
            {
                float MinTime = Keys[0].Time;
                float MaxTime = Keys[NumKeys - 1].Time;

                int CycleCount = 0;
                CycleTime(MinTime, MaxTime, InTime, CycleCount);

                if (RichCurve.PostInfinityExtrap == RCCE_CycleWithOffset)
                {
                    float DV = Keys[NumKeys - 1].Value - Keys[0].Value;
                    CycleValueOffset = DV * CycleCount;
                }
                else if (RichCurve.PostInfinityExtrap == RCCE_Oscillate)
                {
                    if (CycleCount % 2 == 1)
                    {
                        InTime = MinTime + (MaxTime - InTime);
                    }
                }
            }
        }
    }

    static float Eval(float InTime, float InDefaultValue, FRichCurve RichCurve, int KeysNum, std::vector<FRichCurveKey> Keys)
    {
        // Remap time if extrapolation is present and compute offset value to use if cycling 
        float CycleValueOffset = 0;
        RemapTimeValue(InTime, CycleValueOffset, RichCurve, KeysNum, Keys);

        const int32 NumKeys = KeysNum;

        // If the default value hasn't been initialized, use the incoming default value
        float InterpVal = RichCurve.DefaultValue == MAX_flt ? InDefaultValue : RichCurve.DefaultValue;

        if (NumKeys == 0)
        {
            // If no keys in curve, return the Default value.
        }
        else if (NumKeys < 2 || (InTime <= Keys[0].Time))
        {
            if (RichCurve.PreInfinityExtrap == RCCE_Linear && NumKeys > 1)
            {
                float DT = Keys[1].Time - Keys[0].Time;

                if (IsNearlyZero(DT))
                {
                    InterpVal = Keys[0].Value;
                }
                else
                {
                    float DV = Keys[1].Value - Keys[0].Value;
                    float Slope = DV / DT;

                    InterpVal = Slope * (InTime - Keys[0].Time) + Keys[0].Value;
                }
            }
            else
            {
                // Otherwise if constant or in a cycle or oscillate, always use the first key value
                InterpVal = Keys[0].Value;
            }
        }
        else if (InTime < Keys[NumKeys - 1].Time)
        {
            // perform a lower bound to get the second of the interpolation nodes
            int32 first = 1;
            int32 last = NumKeys - 1;
            int32 count = last - first;

            while (count > 0)
            {
                int32 step = count / 2;
                int32 middle = first + step;

                if (InTime >= Keys[middle].Time)
                {
                    first = middle + 1;
                    count -= step + 1;
                }
                else
                {
                    count = step;
                }
            }

            int32 InterpNode = first;
            const float Diff = Keys[InterpNode].Time - Keys[InterpNode - 1].Time;

            if (Diff > 0.f && Keys[InterpNode - 1].InterpMode != RCIM_Constant)
            {
                const float Alpha = (InTime - Keys[InterpNode - 1].Time) / Diff;
                const float P0 = Keys[InterpNode - 1].Value;
                const float P3 = Keys[InterpNode].Value;

                if (Keys[InterpNode - 1].InterpMode == RCIM_Linear)
                {
                    InterpVal = Lerp(P0, P3, Alpha);
                }
                else
                {
                    const float OneThird = 1.0f / 3.0f;
                    const float P1 = P0 + (Keys[InterpNode - 1].LeaveTangent * Diff * OneThird);
                    const float P2 = P3 - (Keys[InterpNode].ArriveTangent * Diff * OneThird);

                    InterpVal = BezierInterp(P0, P1, P2, P3, Alpha);
                }
            }
            else
            {
                InterpVal = Keys[InterpNode - 1].Value;
            }
        }
        else
        {
            if (RichCurve.PostInfinityExtrap == RCCE_Linear)
            {
                float DT = Keys[NumKeys - 2].Time - Keys[NumKeys - 1].Time;

                if (IsNearlyZero(DT))
                {
                    InterpVal = Keys[NumKeys - 1].Value;
                }
                else
                {
                    float DV = Keys[NumKeys - 2].Value - Keys[NumKeys - 1].Value;
                    float Slope = DV / DT;

                    InterpVal = Slope * (InTime - Keys[NumKeys - 1].Time) + Keys[NumKeys - 1].Value;
                }
            }
            else
            {
                // Otherwise if constant or in a cycle or oscillate, always use the last key value
                InterpVal = Keys[NumKeys - 1].Value;
            }
        }
        return InterpVal + CycleValueOffset;
    }

    static void SimulateWeaponTrajectory(FVector Direction, float Distance, float TrajectoryGravityZ,
        float BallisticDragScale, float BallisticDropScale,
        float BDS, float SimulationSubstepTime, float VDragCoefficient,
        FRichCurve RichCurve, int KeysNum, std::vector<FRichCurveKey> Keys,
        float& BulletDrop, float& TravelTime)
    {
        float TravelDistance = 0.0f;
        float CurrentDrop = 0.0f;
        BulletDrop = 0.0f;
        TravelTime = 0.0f;

        Direction.Normalize();
        Direction = Direction * 100.0f;

        while (1)
        {
            float BulletSpeed = Eval(TravelDistance * BDS * BallisticDragScale, 0.0, RichCurve, KeysNum, Keys);

            FVector Velocity = Direction * BulletSpeed;
            Velocity.Z += CurrentDrop;

            FVector Acceleration = Velocity * SimulationSubstepTime;
            float AccelerationLen = Acceleration.Length() / 100.0f;
            if (TravelDistance + AccelerationLen > Distance)
            {
                float RemainDistance = Distance - TravelDistance;
                float AccelerationSpeed = AccelerationLen / SimulationSubstepTime;
                float RemainTime = RemainDistance / AccelerationSpeed;

                TravelTime += RemainTime;
                BulletDrop += RemainTime * CurrentDrop;
                break;
            }
            TravelDistance += AccelerationLen;
            TravelTime += SimulationSubstepTime;
            BulletDrop += SimulationSubstepTime * CurrentDrop;
            CurrentDrop += SimulationSubstepTime * TrajectoryGravityZ * 100 * VDragCoefficient * BallisticDropScale;
        }
    }
    static void Move(int X, int Y)
    {
        if (!GameData.Config.AimBot.Connected || !GameData.Config.AimBot.Enable)
        {
            return;
        }

        switch (GameData.Config.AimBot.Controller) {
        case 0:
            KmBox::Move(X, Y);
            if (GameData.Config.AimBot.Delay > 0) Sleep(GameData.Config.AimBot.Delay);
            break;
        case 1:
            KmBoxNet::Move(X, Y);
            if (GameData.Config.AimBot.Delay > 0) Sleep(GameData.Config.AimBot.Delay);
            break;
        case 2:
            Lurker::Move(X, Y);
            if (GameData.Config.AimBot.Delay > 0) Sleep(GameData.Config.AimBot.Delay);
            break;
        case 3:
            MoBox::Move(X, Y);
            if (GameData.Config.AimBot.Delay > 0) Sleep(GameData.Config.AimBot.Delay);
            break;
        default:
            return;
        }



    }
    static void AimBotAPI1()
    {
        float dynamicRecoilStrength = 0.0f;
        if (GameData.Keyboard.IsKeyDown(VK_LBUTTON))
        {
            // 提高基础压枪强度
            float baseRecoilStrength = 0.6f * GameData.Config.AimBot.wybYSpeed;

            // 增加递增幅度，让压枪后期更有力
            float scalingFactor = 1.0f + (0.08f * recoilTime); // 递增速度加大
            // 增加 recoilTime 累积速度
            recoilTime += 0.1f;

            // 提高最大压枪强度上限
            dynamicRecoilStrength = baseRecoilStrength * scalingFactor;
            dynamicRecoilStrength = (dynamicRecoilStrength > 2.0f * baseRecoilStrength)
                ? 2.0f * baseRecoilStrength
                : dynamicRecoilStrength;

            // 实际进行压枪操作
            Move(0, dynamicRecoilStrength);
        }
        else
        {

            recoilTime = 0.0f;
            dynamicRecoilStrength = 0.0f;
        }



    }
    static void AimBotAPI(FVector2D MoveXY, AimBotConfig Config)
    {
        FVector FMouseXY = { (float)MoveXY.X, (float)MoveXY.Y, 0.0f };
        FMouseXY.Normalize();

        if (MoveXY.X == 0 && MoveXY.Y == 0) {
            RemainMouseX = RemainMouseY = 0.0f;
            return;
        }

        float InitialValue = Config.InitialValue;

        if (GameData.AimBot.Type == EntityType::Wheel)
        {
            Config.XSpeed = Config.AimWheelSpeed;
            Config.YSpeed = Config.AimWheelSpeed;
        }

        float MouseX = RemainMouseX + std::clamp((InitialValue * (Config.XSpeed / 100.0f)) * FMouseXY.X, -(float)abs(MoveXY.X), (float)abs(MoveXY.X));
        float MouseY = RemainMouseY + std::clamp((InitialValue * (Config.YSpeed / 100.0f)) * FMouseXY.Y, -(float)abs(MoveXY.Y), (float)abs(MoveXY.Y));
        MouseX = round(MouseX);
        MouseY = round(MouseY);
        RemainMouseX = MouseX - truncf(MouseX);
        RemainMouseY = MouseY - truncf(MouseY);
        //Utils::Log(1, "Move: %f %f", MouseX, MouseY);
        if (abs(MouseX) > 0 || abs(MouseY) > 0) {
            //  Utils::Log(1, "Move: %f %f", MouseX, MouseY);
            Move(MouseX, MouseY);
        }
        
    }
    static void AimBotAPI_SG(FVector2D MoveXY, AimBotConfig Config)
    {
        if (MoveXY.X == 0 && MoveXY.Y == 0) {
            return;
        }

        float MouseX = MoveXY.X * Config.XSpeed / 100.0f;
        float MouseY = MoveXY.Y * Config.YSpeed / 100.0f;

        if (abs(MouseX) > 0 || abs(MouseY) > 0) {
            Move(MouseX, MouseY);
        }

        //const float Threshold = 3.f;//靠近人物的范围，越小需要越靠近目标才会开枪，提高准确度，但是会降低容错，越低，开枪概率越小
      /*  if (abs(MoveXY.X) < Config.Threshold && abs(MoveXY.Y) < Config.Threshold) {
            TriggerMouseLeft();
        }*/
    }
    static void AimBotAPI(VMMDLL_SCATTER_HANDLE hScatter, FVector2D MoveXY, AimBotConfig Config)
    {
        FVector FMouseXY = { (float)MoveXY.X, (float)MoveXY.Y, 0.0f };
        FMouseXY.Normalize();

        float InitialValue = Config.InitialValue;

        if (GameData.AimBot.Type == EntityType::Wheel)
        {
            Config.XSpeed = Config.AimWheelSpeed;
            Config.YSpeed = Config.AimWheelSpeed;
        }

        float MouseX = RemainMouseX + std::clamp((InitialValue * (Config.XSpeed / 100.0f)) * FMouseXY.X, -(float)abs(MoveXY.X), (float)abs(MoveXY.X));
        float MouseY = RemainMouseY + std::clamp((InitialValue * (Config.YSpeed / 100.0f)) * FMouseXY.Y, -(float)abs(MoveXY.Y), (float)abs(MoveXY.Y));

        RemainMouseX = MouseX - truncf(MouseX);
        RemainMouseY = MouseY - truncf(MouseY);

        if (abs(MouseX) > 0 || abs(MouseY) > 0) {
            /*mem.AddScatterWrite(hScatter, GameData.PlayerController + GameData.Offset["InputYawScale"], -MoveXY.Y / (100.f + 5.f * (100.f - Config.YSpeed)));
            mem.AddScatterWrite(hScatter, GameData.PlayerController + GameData.Offset["InputYawScale"] + 0x4, MoveXY.X / (100.f + 5.f * (100.f - Config.XSpeed)));
            */
            mem.AddScatterWrite(hScatter, GameData.PlayerController + GameData.Offset["InputYawScale"], -MouseY / 100.f);
            mem.AddScatterWrite(hScatter, GameData.PlayerController + GameData.Offset["InputYawScale"] + 0x4, MouseX / 100.f);
            mem.ExecuteWriteScatter(hScatter);
        }

    }

    static bool GetBoneIsAllFalse(const bool Bones[17])
    {
        for (size_t i = 0; i < 17; i++)
        {
            if (Bones[i]) {
                return true;
            }
        }
        return false;
    }

    static void GetScopingAttachPointRelativeZ(
        const VMMDLL_SCATTER_HANDLE& hScatter,
        const FTransform WeaponComponentToWorld,
        float& ScopingAttachPointRelativeZ,
        FTransform& SocketWorldTransform,
        FTransform& ScopeMeshComponentToWorld
    )
    {
        FRotator ScopeSocketRelativeRotation;
        FVector ScopeSocketRelativeLocation;
        FVector ScopeSocketRelativeScale;

        if (CurrentWeaponData.ScopeSocket)
        {
            mem.AddScatterRead(hScatter, CurrentWeaponData.ScopeAimCameraSocket + GameData.Offset["StaticRelativeRotation"], (FRotator*)&ScopeSocketRelativeRotation);
            mem.AddScatterRead(hScatter, CurrentWeaponData.ScopeAimCameraSocket + GameData.Offset["StaticRelativeLocation"], (FVector*)&ScopeSocketRelativeLocation);
            mem.AddScatterRead(hScatter, CurrentWeaponData.ScopeAimCameraSocket + GameData.Offset["StaticRelativeScale"], (FVector*)&ScopeSocketRelativeScale);
            mem.AddScatterRead(hScatter, CurrentWeaponData.ScopeStaticMeshComponent + GameData.Offset["ComponentToWorld"], (FTransform*)&ScopeMeshComponentToWorld);
            mem.ExecuteReadScatter(hScatter);
        }

        if (CurrentWeaponData.ScopeSocket)
        {
            SocketWorldTransform = FTransform(ScopeSocketRelativeRotation, ScopeSocketRelativeLocation, ScopeSocketRelativeScale) * WeaponComponentToWorld;
            const float RelativeZ_1 = SocketWorldTransform.GetRelativeTransform(ScopeMeshComponentToWorld).Translation.Z;
            const float RelativeZ_2 = ScopeMeshComponentToWorld.GetRelativeTransform(WeaponComponentToWorld).Translation.Z;
            ScopingAttachPointRelativeZ = RelativeZ_1 + RelativeZ_2;
        }
        else {
            ScopingAttachPointRelativeZ = WeaponComponentToWorld.GetRelativeTransform(SocketWorldTransform).Translation.Z;
        }
    }

    static uint64_t GetStaticMeshComponentScopeType(uint64_t Mesh) {
        uint64_t Result = 0;
        auto AttachedStaticComponentMap = mem.Read<TMap<TEnumAsByte<EWeaponAttachmentSlotID>, uint64_t>>(Mesh + GameData.Offset["AttachedStaticComponentMap"]);
        AttachedStaticComponentMap.GetValue(EWeaponAttachmentSlotID::UpperRail, Result);
        return Result;
    }

    static bool FindSocket(uint64_t StaticMesh, FName InSocketName, ULONG64& OutSocket) {
        if (InSocketName == NAME_None)
            return false;
        auto Sockets = mem.Read<TArray<uint64_t>>(StaticMesh + GameData.Offset["StaticSockets"]);
        if (!Sockets.size()) return false;
        for (const auto& SocketPtr : Sockets.GetVector()) {

            auto SocketName = mem.Read<FName>(SocketPtr + GameData.Offset["StaticSocketName"]);
            if (SocketName == InSocketName) {
                OutSocket = SocketPtr;
                return true;
            }
        }
        return false;

    }

    static bool GetSocketByName(uint64_t Mesh, FName InSocketName, ULONG64& OutSocket)
    {
        uint64_t StaticMMesh = mem.Read<uint64_t>(Mesh + GameData.Offset["StaticMesh"]);
        if (Utils::ValidPtr(StaticMMesh))
            return false;

        return FindSocket(StaticMMesh, InSocketName, OutSocket);
    }

    static std::pair<float, float> GetBulletDropAndTravelTime(const FVector& GunLocation, const FRotator& GunRotation, const FVector& TargetPos,
        float ZeroingDistance, float BulletDropAdd, float InitialSpeed, float TrajectoryGravityZ, float BallisticDragScale,
        float BallisticDropScale, float BDS, float SimulationSubstepTime, float VDragCoefficient, FRichCurve RichCurve, int KeysNum, std::vector<FRichCurveKey> Keys)
    {
        const float ZDistanceToTarget = TargetPos.Z - GunLocation.Z;
        const float DistanceToTarget = GunLocation.Distance(TargetPos) / 100.0f;
        float TravelTime = DistanceToTarget / InitialSpeed;
        float BulletDrop = 0.5f * TrajectoryGravityZ * TravelTime * TravelTime * 100.0f;

        float TravelTimeZero = ZeroingDistance / InitialSpeed;
        float BulletDropZero = 0.5f * TrajectoryGravityZ * TravelTimeZero * TravelTimeZero * 100.0f;

        if (KeysNum > 0)
        {
            SimulateWeaponTrajectory(GunRotation.GetUnitVector(), DistanceToTarget, TrajectoryGravityZ,
                BallisticDragScale, BallisticDropScale,
                BDS, SimulationSubstepTime,
                VDragCoefficient,
                RichCurve, KeysNum, Keys, BulletDrop, TravelTime);


            SimulateWeaponTrajectory(FVector(1.0f, 0.0f, 0.0f), ZeroingDistance, TrajectoryGravityZ,
                BallisticDragScale, BallisticDropScale, BDS, SimulationSubstepTime, VDragCoefficient,
                RichCurve, KeysNum, Keys, TravelTimeZero, BulletDropZero);
        }

        BulletDrop = fabsf(BulletDrop) - fabsf(BulletDropAdd);
        if (BulletDrop < 0.0f)
            BulletDrop = 0.0f;
        BulletDropZero = fabsf(BulletDropZero) + fabsf(BulletDropAdd);

        const float TargetPitch = asinf((ZDistanceToTarget + BulletDrop) / 100.0f / DistanceToTarget);
        const float ZeroPitch = IsNearlyZero(ZeroingDistance) ? 0.0f : atan2f(BulletDropZero / 100.0f, ZeroingDistance);
        const float FinalPitch = TargetPitch - ZeroPitch;
        const float AdditiveZ = DistanceToTarget * sinf(FinalPitch) * 100.0f - ZDistanceToTarget;

        return std::pair(AdditiveZ, TravelTime);
    }

    // 判断是否狙击枪
    static bool isSnaperGun(const std::string& ID1) {
        if (ID1 == "WeapMosinNagant_C" || ID1 == "WeapMk14_C" || ID1 == "WeapM24_C" ||
            ID1 == "WeapAWM_C" || ID1 == "WeapMadsQBU88_C" || ID1 == "WeapQBU88_C" ||
            ID1 == "WeapMini14_C" || ID1 == "WeapJuliesKar98k_C" || ID1 == "WeapKar98k_C" ||
            ID1 == "WeapWin94_C" || ID1 == "WeapSKS_C" || ID1 == "WeapFNFal_C" || ID1 == "WeapMk47Mutant_C" || ID1 == "WeapL6_C")
        {
            return true;
        }
        return false;
    }

    static float GetInitialVelocitys(const std::string& ID1)
    {
        if (ID1 == "WeapL6_C")
            return 35.0f; //35.0
        else if (ID1 == "WeapAK47_C" || ID1 == "WeapLunchmeatsAK47_C")
            return 54.0f;
        else if (ID1 == "WeapAUG_C")
            return 50.0f;
        else if (ID1 == "WeapQBZ95_C")
            return 51.0f;
        else if (ID1 == "WeapJuliesKar98k_C" || ID1 == "WeapKar98k_C")
            return 40.0f;
        else if (ID1 == "WeapMosinNagant_C")
            return 40.0f;
        else if (ID1 == "WeapM24_C")
            return 35.0f;
        else if (ID1 == "WeapSKS_C")
            return 47.0f;
        else if (ID1 == "WeapSCAR-L_C")
            return 49.0f;
        else if (ID1 == "WeapDuncansHK416_C" || ID1 == "WeapHK416_C")
            return 50.0f;
        else if (ID1 == "WeapM16A4_C")
            return 48.0f;
        else if (ID1 == "WeapAWM_C")
            return 35.0f;
        else if (ID1 == "WeapMini14_C")
            return 60.0f;
        else if (ID1 == "WeapMadsQBU88_C" || ID1 == "WeapQBU88_C")
            return 45.0f;
        else if (ID1 == "WeapGroza_C")
            return 49.0f;
        else if (ID1 == "WeapUMP_C")
            return 56.0f;
        else if (ID1 == "WeapMk14_C")
            return 38.0f;
        else if (ID1 == "WeapFNFal_C")
            return 37.0f;
        else if (ID1 == "WeapBerylM762_C")
            return 54.0f;
        else if (ID1 == "WeapMk47Mutant_C")
            return 47.0f;
        else if (ID1 == "WeapM249_C")
            return 50.0f;
        else if (ID1 == "WeapWin94_C")
            return 45.0f;
        else if (ID1 == "WeapVector_C")
            return 60.0f;
        else if (ID1 == "WeapVSS_C")
            return 60.0f;
        else if (ID1 == "WeapDP28_C")
            return 52.0f;
        else if (ID1 == "WeapBizonPP19_C")
            return 49.0f;
        else if (ID1 == "WeapG36C_C")
            return 52.0f;
        return 0.0f; // 默认返回0
    }
  
    static float GetZPosition(uint64_t TargetActor, Vector3 Postion)
    {
        if (!LastActor)
        {
            LastActor = TargetActor;
            bIsSaveComplete = false;
        }
        if (LastActor != TargetActor || ZCount > 10 || ZCount < 0)
        {
            LastActor = TargetActor;
            ZCount = 0;
            ZeroMemory(&ZSave, sizeof(ZSave));
            bIsSaveComplete = false;
        }
        if (ZCount < 10)
        {
            ZSave[ZCount] = Postion.z;
            if (ZCount == 9)
            {
                bIsSaveComplete = true;
            }
            ZCount++;
        }
        if (bIsSaveComplete)
        {
            float Times = 0.f;
            float WholePos = 0.f;
            float FirstPosition = ZSave[0];
            if (FirstPosition)
            {
                for (int i = 0; i < 9; i++)
                {
                    if (i >= 0 && i < 9)
                    {
                        Times += 1.f;
                        WholePos += ZSave[i] - ZSave[i + 1];
                    }
                }
                bIsSaveComplete = false;
                return WholePos / Times;
            }
        }
        return 0.f;
    }
    // 获取预测位置
    static Vector3 GetPrediction(Vector3 target, float Dis, Vector3 TargetVelocity, float ProjectileSpeed, float m_gravity, bool IsS, float drop, bool IsVehicle) {
        float flyTime = Dis / ProjectileSpeed;
        float temp = 1.33f;

        if (Dis >= 0 && Dis < 100) {
            drop *= 0.68f;
            temp = 1.25f;
        }
        else if (Dis >= 100 && Dis < 200) {
            if (IsS) {
                drop *= 0.72f;
                temp = 1.2f;
            }
            else {
                drop *= 0.6f;
                temp = 1.24f;
            }
        }
        else if (Dis >= 200 && Dis < 250) {
            if (IsS) {
                drop *= 0.74f;
                temp = 1.17f;
            }
            else {
                drop *= 0.67f;
                temp = 1.21f;
            }
        }
        else if (Dis >= 250 && Dis < 300) {
            if (IsS) {
                drop *= 0.8f;
                temp = 1.18f;
            }
            else {
                drop *= 0.75f;
                temp = 1.22f;
            }
        }
        else if (Dis >= 300 && Dis < 350) {
            if (IsS) {
                drop *= 0.94f;
                temp = 1.22f;
            }
            else {
                drop *= 0.83f;
                temp = 1.25f;
            }
        }
        else if (Dis >= 350 && Dis < 400) {
            if (IsS) {
                drop *= 0.96f;
                temp = 1.235f;
            }
            else {
                drop *= 1.03f;
                temp = 1.29f;
            }
        }
        else if (Dis >= 400 && Dis < 450) {
            if (IsS) {
                drop *= 1.06f;
                temp = 1.24f;
            }
            else {
                drop *= 1.15f;
                temp = 1.38f;
            }
        }
        else if (Dis >= 450 && Dis < 500) {
            if (IsS) {
                drop *= 1.06f;
                temp = 1.24f;
            }
            else {
                drop *= 1.15f;
                temp = 1.38f;
            }
        }
        else if (Dis >= 500 && Dis < 550) {
            if (IsS) {
                drop *= 1.25f;
                temp = 1.32f;
            }
            else {
                drop *= 1.35f;
                temp = 1.44f;
            }
        }
        else if (Dis >= 550 && Dis < 600) {
            if (IsS) {
                drop *= 1.37f;
                temp = 1.35f;
            }
            else {
                drop *= 1.5f;
                temp = 1.46f;
            }
        }
        else if (Dis >= 600 && Dis < 650) {
            if (IsS) {
                drop *= 1.46f;
                temp = 1.36f;
            }
            else {
                drop *= 1.6f;
                temp = 1.48f;
            }
        }
        else if (Dis >= 650 && Dis < 700) {
            if (IsS) {
                drop *= 1.57f;
                temp = 1.4f;
            }
            else {
                drop *= 1.7f;
                temp = 1.5f;
            }
        }
        else if (Dis >= 700 && Dis < 750) {
            if (IsS) {
                drop *= 1.65f;
                temp = 1.42f;
            }
            else {
                drop *= 1.8f;
                temp = 1.52f;
            }
        }
        else if (Dis >= 750 && Dis < 800) {
            if (IsS) {
                drop *= 1.74f;
                temp = 1.44f;
            }
            else {
                drop *= 1.9f;
                temp = 1.55f;
            }
        }
        else if (Dis >= 800 && Dis < 850) {
            if (IsS) {
                drop *= 1.84f;
                temp = 1.47f;
            }
            else {
                drop *= 2.0f;
                temp = 1.57f;
            }
        }
        else if (Dis >= 850 && Dis < 900) {
            if (IsS) {
                drop *= 1.9f;
                temp = 1.52f;
            }
            else {
                drop *= 2.1f;
                temp = 1.6f;
            }
        }
        else if (Dis >= 900 && Dis < 950) {
            if (IsS) {
                drop *= 1.92f;
                temp = 1.54f;
            }
            else {
                drop *= 2.2f;
                temp = 1.7f;
            }
        }
        else {
            if (IsS) {
                drop *= 2.0f;
                temp = 1.58f;
            }
            else {
                drop *= 2.3f;
                temp = 1.76f;
            }
        }

        // 计算预测位置
        flyTime *= temp;
        Vector3 PredictedPos;
        PredictedPos.x = target.x + TargetVelocity.x * flyTime;
        PredictedPos.y = target.y + TargetVelocity.y * flyTime;
        float flyTime2 = powf(flyTime, 2);
        if (drop == 0.f)
            drop = 50.0f;
        PredictedPos.z = target.z + 0.5f * m_gravity * flyTime2 * drop;
        if (!IsVehicle)
            PredictedPos.z -= GetZPosition(GameData.AimBot.Target, target);
        return PredictedPos;
    }

  


    static void Run()
    {
        bool AimAndShot = false;
        auto hScatter = mem.CreateScatterHandle();
        auto hWriteScatter = mem.CreateScatterHandle();
        Throttler Throttlered;
        FName FMouseX = { GameData.Offset["MouseX"] };
        FName FMouseY = { GameData.Offset["MouseY"] };
        FInputAxisProperties MouseX;
        FInputAxisProperties MouseY;
        bool FistAim = false;
        AimBotConfig Config;
        Config.FPS = 1500;

        while (true)
        {
            //Timer timer("1");

            Throttlered.executeTaskWithSleep("AimBotSleep", std::chrono::milliseconds(static_cast<int>(1000 / Config.FPS)), [] {});

            //std::cout << timer.get() << std::endl;
            //continue;

            //if (GameData.Config.AimBot.Controller == 0 && GameData.Config.AimBot.Delay > 0) std::this_thread::sleep_for(std::chrono::microseconds(GameData.Config.AimBot.Delay));

            if (GameData.Scene != Scene::Gaming)
            {
                Sleep(GameData.ThreadSleep);
                continue;
            }

            if (GameData.bShowMouseCursor || Utils::ValidPtr(GameData.AcknowledgedPawn) || !GameData.Config.AimBot.Enable || GameData.LocalPlayerInfo.Health <= 0)
            {
                //无预瞄点时检查上面的条件是否正确，指定是有不对的
                //Utils::Log(1, "not work AimBot");
                StopAiming();
                continue;
            }

            if (GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType == WeaponType::Other)
            {
                StopAiming();
                continue;
            }

            Config = GameData.Config.AimBot.Configs[GameData.Config.AimBot.ConfigIndex].Weapon[WeaponTypeToString[GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType]];
            
            if (!Config.enable)
            {
                StopAiming();
                continue;
            }
            if (Config.bIsScoping_CP && !GameData.LocalPlayerInfo.IsScoping) {
                StopAiming();
                continue;
            }
            FVector TargetPos;
            float TargetDistance;
            FVector TargetVelocity;
            FVector LastUpdateRotation;
            FVector TargetAcceleration;
            bool IsScoping = false;
            bool IsReloading = false;
            FRotator Recoil;
            FRotator ControlRotation;
            float LeanLeftAlpha_CP = 0.f;
            float LeanRightAlpha_CP = 0.f;
            bool NeedEndHook = false;

            bool IsFirstKey = GameData.Keyboard.IsKeyDown(Config.First.Key);
            bool IsSecondKey = GameData.Keyboard.IsKeyDown(Config.Second.Key);
            bool IsGroggyKey = GameData.Keyboard.IsKeyDown(Config.Groggy.Key);
            bool IsWheelKey = GameData.Keyboard.IsKeyDown(Config.Wheel.Key);
            bool CanAim = IsFirstKey || IsSecondKey || IsGroggyKey || IsWheelKey;
            bool InScopePoint = (Config.HotkeyMerge && !IsFirstKey && IsSecondKey && GameData.LocalPlayerInfo.IsScoping && GameData.Config.AimBot.ShowPoint);
            SHORT BulletNumber = 1.f;
            mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.CurrentWeapon + GameData.Offset["CurrentAmmoData"], (SHORT*)&BulletNumber);
            mem.ExecuteReadScatter(hScatter);
            
            Player Player = Data::GetPlayersItem(GameData.AimBot.Target);
            //if (Config.AimAndShot && Player.IsVisible && TargetDistance <= 30 && GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType == WeaponType::SG && BulletNumber > 0)
            //{
            //    TriggerMouseLeft();
            //}
            
            //AimAndShot = false;
            if (CanAim || (GameData.LocalPlayerInfo.IsScoping && GameData.Config.AimBot.ShowPoint))
            {
                if (Utils::ValidPtr(GameData.AcknowledgedPawn))
                {
                    Sleep(1);
                    continue;
                }

                if (InScopePoint)
                {
                    CanAim = false;
                }
                else {

                    if (Config.HotkeyMerge && !IsFirstKey && IsSecondKey)
                    {
                        StopAiming();
                        continue;
                    }
                    else if (Config.HotkeyMerge && IsFirstKey && !IsSecondKey && !GetBoneIsAllFalse(Config.First.Bones))
                    {
                        StopAiming();
                        continue;
                    }
                    else if (!Config.HotkeyMerge && IsFirstKey && !IsSecondKey && !GetBoneIsAllFalse(Config.First.Bones))
                    {
                        StopAiming();
                        continue;
                    }
                    else if (!Config.HotkeyMerge && !IsFirstKey && IsSecondKey && !GetBoneIsAllFalse(Config.Second.Bones))
                    {
                        StopAiming();
                        continue;
                    }
                }

                std::unordered_map<uint64_t, tMapInfo> EnemyInfoMap = Data::GetEnemyInfoMap();

                if (Utils::ValidPtr(LastCurrentWeapon) || LastCurrentWeapon != GameData.LocalPlayerInfo.CurrentWeapon)
                {
                    uint64_t WeaponTrajectoryData;
                    FWeaponTrajectoryConfig TrajectoryConfig;
                    uint64_t BallisticCurve;
                    FTransform WeaponComponentToWorld;

                    mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.CurrentWeapon + GameData.Offset["WeaponTrajectoryData"], (uint64_t*)&WeaponTrajectoryData);
                    mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.CurrentWeapon + GameData.Offset["TrajectoryGravityZ"], (float*)&CurrentWeaponData.TrajectoryGravityZ);
                    mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.CurrentWeapon + GameData.Offset["FiringAttachPoint"], (FName*)&CurrentWeaponData.FiringAttachPoint);
                    mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.CurrentWeapon + GameData.Offset["ScopingAttachPoint"], (FName*)&CurrentWeaponData.ScopingAttachPoint);
                    mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.CurrentWeapon + GameData.Offset["Mesh3P"], (uint64_t*)&CurrentWeaponData.Mesh3P);
                    mem.ExecuteReadScatter(hScatter);

                    if (Utils::ValidPtr(WeaponTrajectoryData))
                    {
                        StopAiming();
                        continue;
                    }

                    mem.AddScatterRead(hScatter, WeaponTrajectoryData + GameData.Offset["TrajectoryConfig"], &CurrentWeaponData.TrajectoryConfig);
                    //Utils::Log(1, "CurrentWeaponData.TrajectoryConfig: %p", CurrentWeaponData.TrajectoryConfig);
                    mem.ExecuteReadScatter(hScatter);

                    CurrentWeaponData.Mesh3P = Decrypt::Xe(CurrentWeaponData.Mesh3P);
                    //  Utils::Log(1, "CurrentWeaponData.TrajectoryConfig: %p", CurrentWeaponData.Mesh3P);
                    TrajectoryConfig.BallisticCurve = CurrentWeaponData.GetTrajectoryConfig<uint64_t>(GameData.Offset["BallisticCurve"]);
                    CurrentWeaponData.TrajectoryConfigs.InitialSpeed = CurrentWeaponData.GetTrajectoryConfig<float>(0);
                    CurrentWeaponData.TrajectoryConfigs.SimulationSubstepTime = CurrentWeaponData.GetTrajectoryConfig<float>(0x40);
                    CurrentWeaponData.TrajectoryConfigs.VDragCoefficient = CurrentWeaponData.GetTrajectoryConfig<float>(0x44);
                    CurrentWeaponData.TrajectoryConfigs.BDS = CurrentWeaponData.GetTrajectoryConfig<float>(0x48);
                    CurrentWeaponData.TrajectoryConfigs.ReferenceDistance = CurrentWeaponData.GetTrajectoryConfig<float>(0x34);


                    //  Utils::Log(1, "CurrentWeaponData.TrajectoryConfig: %f", CurrentWeaponData.TrajectoryConfigs.InitialSpeed);


                    if (CurrentWeaponData.TrajectoryConfigs.InitialSpeed < 100)
                    {
                        CurrentWeaponData.TrajectoryConfigs.InitialSpeed = 800;
                    }

                    mem.AddScatterRead(hScatter, TrajectoryConfig.BallisticCurve + GameData.Offset["FloatCurves"], (FRichCurve*)&CurrentWeaponData.FloatCurves);
                    mem.AddScatterRead(hScatter, TrajectoryConfig.BallisticCurve + GameData.Offset["FloatCurves"] + GameData.Offset["Keys"], (FRichCurveKeyArray*)&CurrentWeaponData.RichCurveKeyArray);
                    mem.ExecuteReadScatter(hScatter);

                    std::vector<FRichCurveKey> Keys(CurrentWeaponData.RichCurveKeyArray.Count);

                    mem.Read(CurrentWeaponData.RichCurveKeyArray.Data, Keys.data(), sizeof(FRichCurveKey) * CurrentWeaponData.RichCurveKeyArray.Count);
                    CurrentWeaponData.RichCurveKeys = Keys;

                    LastCurrentWeapon = GameData.LocalPlayerInfo.CurrentWeapon;
                }
                if (GameData.Config.AimBot.qjyaqiang && !GameData.AimBot.Lock && (GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType == WeaponType::AR || GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType == WeaponType::LMG) && BulletNumber > 0)
                {
                    AimBotAPI1();  // 单独处理压枪
                }
                else
                {
                    recoilTime = 0.0f;

                }
                //Utils::Log(1, "CurrentWeaponData.FiringAttachPoin: %d", CurrentWeaponData.FiringAttachPoint.ComparisonIndex);
               // Utils::Log(1, "CurrentWeaponData.ScopingAttachPoint: %d", CurrentWeaponData.ScopingAttachPoint.ComparisonIndex);


                uint64_t InputAxisProperties;
                int InputAxisPropertiesCount;

                if (!FistAim)
                {
                    mem.AddScatterRead(hScatter, GameData.PlayerInput + GameData.Offset["InputAxisProperties"], (uint64_t*)&InputAxisProperties);
                    mem.AddScatterRead(hScatter, GameData.PlayerInput + GameData.Offset["InputAxisProperties"] + 0x8, (int*)&InputAxisPropertiesCount);
                    mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.CurrentWeapon + GameData.Offset["Mesh3P"], (uint64_t*)&CurrentWeaponData.Mesh3P);
                    mem.ExecuteReadScatter(hScatter);
                    //Utils::Log(1, "GameData.PlayerInput: %p", GameData.PlayerInput);
                    //Utils::Log(1, "InputAxisPropertiesCount: %p", InputAxisProperties);
                    //Utils::Log(1, "InputAxisPropertiesCount: %d", InputAxisPropertiesCount);30109433
                    CurrentWeaponData.Mesh3P = Decrypt::Xe(CurrentWeaponData.Mesh3P);

                    if (Utils::ValidPtr(CurrentWeaponData.Mesh3P))
                    {
                        StopAiming();
                        continue;
                    }

                    if (GameData.Config.AimBot.Connected)
                    {
                        if (GameData.Config.AimBot.Controller == 0)
                        {
                            KmBox::Clear();
                        }

                        if (GameData.Config.AimBot.Controller == 1)
                        {
                            KmBoxNet::Clear();
                        }
                    }
                }

                if (!FistAim)
                {
                    std::vector<std::pair<FName, FInputAxisProperties>> InputAxisPropertiesLists;

                    for (int i = 0; i < InputAxisPropertiesCount; i++)
                    {
                        std::pair<FName, FInputAxisProperties> Info;
                        InputAxisPropertiesLists.push_back(Info);
                    }

                    int i = 0;
                    for (auto& InputAxis : InputAxisPropertiesLists)
                    {
                        mem.AddScatterRead(hScatter, InputAxisProperties + (i * (sizeof(FName) + sizeof(FInputAxisProperties))), (std::pair<FName, FInputAxisProperties>*) & InputAxis);
                        i++;
                    }

                    mem.ExecuteReadScatter(hScatter);


                    for (auto& InputAxis : InputAxisPropertiesLists)
                    {
                    
                        if (InputAxis.first.ComparisonIndex == FMouseX.ComparisonIndex || InputAxis.first.Number == FMouseX.ComparisonIndex)
                        {
                            MouseX = InputAxis.second;
                        }
                        if (InputAxis.first.ComparisonIndex == FMouseY.ComparisonIndex || InputAxis.first.Number == FMouseX.ComparisonIndex)
                        {
                            MouseY = InputAxis.second;
                        }
                 
                    }

                    FistAim = true;
                }

                if (!GameData.AimBot.Lock && !Utils::ValidPtr(CurrentWeaponData.Mesh3P))
                {
                    CurrentWeaponData.ScopeStaticMeshComponent = GetStaticMeshComponentScopeType(CurrentWeaponData.Mesh3P);
                    CurrentWeaponData.ScopeSocket = GetSocketByName(CurrentWeaponData.ScopeStaticMeshComponent, CurrentWeaponData.ScopingAttachPoint, CurrentWeaponData.ScopeAimCameraSocket);
                }
//30109433
                if (false)
                {

                    FistAim = true;
                    CurrentWeaponData.SkeletalMesh = mem.Read<uint64_t>(CurrentWeaponData.Mesh3P + GameData.Offset["SkeletalMesh"]);
                    CurrentWeaponData.Skeleton = mem.Read<uint64_t>(CurrentWeaponData.SkeletalMesh + GameData.Offset["Skeleton"]);
                    CurrentWeaponData.SkeletalSockets = mem.Read<TArray<uint64_t>>(CurrentWeaponData.Skeleton + GameData.Offset["SkeletalSockets"]);

                    CurrentWeaponData.SkeletalMeshSockets.clear();

                    for (auto& pSocket : CurrentWeaponData.SkeletalSockets.GetVector())
                    {
                        CurrentWeaponData.SkeletalMeshSockets.push_back({ pSocket });
                    }

                    for (auto& SocketItem : CurrentWeaponData.SkeletalMeshSockets)
                    {
                        //Utils::Log(1, "SocketItem.pSocket: %p", SocketItem.pSocket);
                        mem.AddScatterRead(hScatter, SocketItem.pSocket + GameData.Offset["SkeletalSocketName"], (FName*)&SocketItem.SocketName);
                    }
                    mem.ExecuteReadScatter(hScatter);

                    for (auto& SocketItem : CurrentWeaponData.SkeletalMeshSockets)
                    {
                        if (SocketItem.SocketName == CurrentWeaponData.FiringAttachPoint)
                        {
                            CurrentWeaponData.FiringAttachPointSocketBone = SocketItem.pSocket;
                            FRotator RelativeRotation = mem.Read<FRotator>(CurrentWeaponData.FiringAttachPointSocketBone + 0x44);
                            FVector Direction = RelativeRotation.GetUnitVector();
                            Direction.Normalize();

                            //Trajectory.Location = FiringLocation;
                            //Utils::Log(1, "SocketItem: %d %f %f %f", SocketItem.SocketName.ComparisonIndex, Direction.X, Direction.Y, Direction.Z);
                        }
                    }
                    
                }

                if (GameData.AimBot.Target == 0)
                {
                    StopAiming();
                    continue;
                }

                if (GameData.AimBot.Type == EntityType::Player)
                {
                    if (!InScopePoint && Config.HotkeyMerge && !IsFirstKey && IsSecondKey)
                    {
                        StopAiming();
                        continue;
                    }

                    if (CanAim) GameData.AimBot.Lock = true;
                    //Player Player = Data::GetPlayersItem(GameData.AimBot.Target);
                    int AimBone = GameData.AimBot.Bone;

                    //拉枪自瞄
                    if (Config.FOVenable) {
                        FVector2D ScreenLocation = Player.Skeleton.ScreenBones[AimBone];
                        float Distance = Utils::CalculateDistance(GameData.Config.Overlay.ScreenWidth / 2, GameData.Config.Overlay.ScreenHeight / 2, ScreenLocation.X, ScreenLocation.Y);

                        if (Distance > Config.FOV) {
                            StopAiming();
                            continue;
                        }
                    }

                    if (IsFirstKey && IsSecondKey)
                    {
                        int SecondBone = -1;
                        for (size_t i = 0; i < 17; i++)
                        {
                            if (Config.Second.Bones[i]) {
                                SecondBone = BoneIndex[i];
                                break;
                            }
                        }
                        if (SecondBone != -1) AimBone = SecondBone;
                    }

                    if (Player.Distance >= 75 && Player.Distance <= 200 && AimBone == EBoneIndex::ForeHead) {
                        AimBone = EBoneIndex::Head;
                    }

                    if (CanAim && Config.LineTraceSingle && RecoilTimeStartTime == 0 && Player.State != CharacterState::Dead)
                    {            
                        
                        EBoneIndex VisibilityBoneIndex;
                        EBoneIndex targetBone = static_cast<EBoneIndex>(AimBone);
                        if (LineTraceHook::GetLocation(Player, targetBone, &VisibilityBoneIndex))
                        {
                            //Utils::Log(1, "LineTraceSingleRecoilLocation");
                            AimBone = VisibilityBoneIndex;
                            Player = Data::GetPlayersItem(GameData.AimBot.Target);
                            LineTraceSingleRecoilLocation = Player.Skeleton.LocationBones[AimBone];
                            LineTraceSingleRecoilTimeStartTime = 0;
                            Config.VisibleCheck = false;
                            Player.IsVisible = true;
                        }
                        else if (LineTraceSingleRecoilLocation == FVector(0.f, 0.f, 0.f)) {
                            Config.VisibleCheck = true;
                            Player.IsVisible = false;
                            GameData.AimBot.PredictedPos = FVector();
                        }

 
                        Config.FPS = 1500;
                        Config.InitialValue *= 2;
                        Config.AimSpeedMaxFactor *= 2;


                    }
                    else
                    {
                        TargetPos = Player.Skeleton.LocationBones[AimBone];// +Player.LastUpdateVelocity * 0.1; // (float)GameData.Config.Debug.B1;//(Player.Distance / ) * 1.f
                    }
                   

                    uint64_t VehicleRiderComponent;
                    uint64_t MovementComponent;
                    int SeatIndex = -1;

                    mem.AddScatterRead(hScatter, Player.Entity + GameData.Offset["VehicleRiderComponent"], (uint64_t*)&VehicleRiderComponent);
                    mem.ExecuteReadScatter(hScatter);

                    mem.AddScatterRead(hScatter, VehicleRiderComponent + GameData.Offset["LastVehiclePawn"], (uint64_t*)&MovementComponent);
                    mem.AddScatterRead(hScatter, VehicleRiderComponent + GameData.Offset["SeatIndex"], (int*)&SeatIndex);
                    mem.ExecuteReadScatter(hScatter);

                    //TargetPos = Player.Skeleton.LocationBones[AimBone];
                    TargetDistance = GameData.Camera.Location.Distance(Player.Location) / 100.0f;
                    FVector ReplicatedMovement;

                    if (LineTraceSingleRecoilLocation != FVector(0.f, 0.f, 0.f))
                    {
                        //Utils::Log(1, "%f %f %f", LineTraceSingleRecoilLocation.X, LineTraceSingleRecoilLocation.Y, LineTraceSingleRecoilLocation.Z);
                        if (LineTraceSingleRecoilTimeStartTime == 0)
                        {
                            LineTraceSingleRecoilTimeStartTime = GetTickCount64();
                        }
                        else if ((GetTickCount64() - LineTraceSingleRecoilTimeStartTime >= Config.RecoilTime * 200) || Config.RecoilTime <= 0) {
                            LineTraceSingleRecoilTimeStartTime = 0;
                            LineTraceSingleRecoilLocation = FVector(0.f, 0.f, 0.f);
                        }

                        TargetPos = LineTraceSingleRecoilLocation;
                    }

                    if (Player.Health <= 0) {
                        bool ShouldSwitchTarget = Config.AutoSwitch &&
                            ((Config.IgnoreGroggy) || (!Config.IgnoreGroggy && Player.GroggyHealth <= 0));

                        if (ShouldSwitchTarget) {
                            Config.SwitchingDelay += Config.RecoilTime;

                            bool TimeElapsed = Config.SwitchingDelay > 0 &&
                                (GetTickCount64() - AutoSwitchTargetStartTime >= Config.SwitchingDelay * 100);

                            if (Config.SwitchingDelay > 0 && AutoSwitchTargetStartTime == 0) {
                                AutoSwitchTargetStartTime = GetTickCount64();
                                RecoilLocation = TargetPos;
                                Data::SetEnemyInfoMap({});
                            }
                            else if (TimeElapsed || Config.SwitchingDelay <= 0) {
                                Data::SetEnemyInfoMap({});
                                AutoSwitchTargetStartTime = 0;
                                RecoilLocation = TargetPos;
                                GameData.AimBot.Lock = false;
                                GameData.AimBot.Target = 0;
                            }
                        }
                        else {
                            if ((Config.IgnoreGroggy) || (!Config.IgnoreGroggy && Player.GroggyHealth <= 0)) {
                                if (Config.RecoilTime > 0 && RecoilTimeStartTime == 0) {
                                    RecoilTimeStartTime = GetTickCount64();
                                    RecoilLocation = TargetPos;
                                    Data::SetEnemyInfoMap({});
                                }
                                else if ((GetTickCount64() - RecoilTimeStartTime >= Config.RecoilTime * 100) || Config.RecoilTime <= 0) {
                                    Sleep(1);
                                    continue;
                                }
                                else {
                                    TargetPos = RecoilLocation;
                                }
                            }
                        }

                        if (RecoilLocation != FVector(0.f, 0.f, 0.f))
                        {
                            TargetPos = RecoilLocation;
                        }
                    }

                    if (TargetPos.X == 0 || TargetPos.Y == 0 || TargetPos.Z == 0)
                    {
                        StopAiming();
                        continue;
                    }

                    if (Config.IgnoreGroggy && !IsGroggyKey && Player.Health <= 0 && Player.GroggyHealth > 0 && RecoilTimeStartTime == 0)
                    {
                        StopAiming();
                        continue;
                    }

                    if (Config.VisibleCheck && !Player.IsVisible)
                    {
                        StopAiming();
                        GameData.AimBot.PredictedPos = FVector();
                        continue;
                    }

                    if (SeatIndex == -1)
                    {
                        mem.AddScatterRead(hScatter, Player.MeshComponent + GameData.Offset["ComponentToWorld"], (FTransform*)&Player.ComponentToWorld);
                        mem.AddScatterRead(hScatter, Player.CharacterMovement + GameData.Offset["LastUpdateVelocity"], (FVector*)&TargetVelocity);
                        //mem.AddScatterRead(hScatter, Player.CharacterMovement + GameData.Offset["LastUpdateVelocity"] - 0x28, (FVector*)&TargetAcceleration);
                        mem.AddScatterRead(hScatter, Player.AnimScriptInstance + GameData.Offset["PreEvalPawnState"], (EAnimPawnState*)&Player.PreEvalPawnState);
                        //mem.AddScatterRead(hScatter, Player.CharacterMovement + GameData.Offset["LastUpdateVelocity"] - 0x10, (FVector*)&LastUpdateRotation);
                        //mem.AddScatterRead(hScatter, Player.RootComponent + GameData.Offset["ComponentVelocity"], (FVector*)&LastUpdateRotation);
                    }
                    else {
                        mem.AddScatterRead(hScatter, MovementComponent + GameData.Offset["ReplicatedMovement"], (FVector*)&ReplicatedMovement);
                    }
                    mem.ExecuteReadScatter(hScatter);

                    Player.Location = Player.ComponentToWorld.Translation;

                    if (RecoilTimeStartTime == 0)
                    {
                        float TimeStampDelta = GameData.WorldTimeSeconds - EnemyInfoMap[Player.Entity].TimeStamp;
                        EnemyInfoMap[Player.Entity].TimeStamp = GameData.WorldTimeSeconds;
                        //PosInfo
                        [&] {
                            auto& PosInfo = EnemyInfoMap[Player.Entity].PosInfo.Info;

                            if (Player.State == CharacterState::Dead) {
                                PosInfo.clear();
                            }
                            else {

                                if (TimeStampDelta)
                                    PosInfo.push_front({ GameData.WorldTimeSeconds, Player.Location });

                                //Utils::Log(1, "GameData.WorldTimeSeconds %f", GameData.WorldTimeSeconds);

                                if (PosInfo.size() > 200)
                                    PosInfo.pop_back();

                                float SumTimeDelta = 0.0f;
                                FVector SumPosDif;

                                for (size_t i = 1; i < PosInfo.size(); i++) {
                                    const float DeltaTime = PosInfo[i - 1].Time - PosInfo[i].Time;
                                    const FVector DeltaPos = PosInfo[i - 1].Pos - PosInfo[i].Pos;
                                    const FVector DeltaVelocity = DeltaPos * (1.0f / DeltaTime);
                                    const float DeltaSpeedPerHour = DeltaVelocity.Length() / 100.0f * 3.6f;

                                    if (DeltaTime > 0.05f || DeltaSpeedPerHour > 500.0f) {
                                        PosInfo.clear();
                                    }
                                    else {
                                        SumTimeDelta = SumTimeDelta + DeltaTime;
                                        SumPosDif = SumPosDif + DeltaPos;

                                        if (SumTimeDelta > 0.15f)
                                            break;
                                    }
                                }
                                if (SumTimeDelta > 0.1f) {
                                    Player.Velocity = SumPosDif * (1.0f / SumTimeDelta);
                                }
                            }
                            }();
                        Data::SetEnemyInfoMap(EnemyInfoMap);
                    }

                    if (SeatIndex == -1)
                    {
                        if (Player.PreEvalPawnState == EAnimPawnState::PS_SecondaryLocomotion || Config.PredictionMode == 1)
                        {
                            TargetVelocity = Player.Velocity;
                        }
                        else {
                            TargetVelocity.Z = Player.Velocity.Z;
                         
                        }
                    }
                    else {
                        TargetVelocity = ReplicatedMovement;
                    }

                    // Utils::Log(1, "TargetAcceleration: %f %f %f", TargetAcceleration.X, TargetAcceleration.Y, TargetAcceleration.Z);
                }
                else if (GameData.AimBot.Type == EntityType::Wheel) {
                    if (!IsWheelKey)
                    {
                        StopAiming();
                        continue;
                    }

                    if (CanAim) GameData.AimBot.Lock = true;
                    FRotator VehicleRotator;
                    VehicleWheelInfo Wheel = Data::GetVehicleWheelsItem(GameData.AimBot.Target);
                    //mem.AddScatterRead(hScatter, Wheel.Wheel + GameData.Offset["WheelVelocity"], (FVector*)&TargetVelocity);
                    mem.AddScatterRead(hScatter, Wheel.Wheel + GameData.Offset["WheelLocation"], &Wheel.Location);
                    mem.AddScatterRead(hScatter, Wheel.Wheel + GameData.Offset["DampingRate"], &Wheel.DampingRate);
                    mem.AddScatterRead(hScatter, Wheel.Vehicle + GameData.Offset["ReplicatedMovement"], (FVector*)&TargetVelocity);
                    mem.AddScatterRead(hScatter, Wheel.Vehicle + GameData.Offset["ReplicatedMovement"] + 0x24, (FRotator*)&VehicleRotator);
                    mem.ExecuteReadScatter(hScatter);

                    TargetPos = Wheel.Location;

                    auto UpdatePost = VectorHelper::RotateVector(VehicleRotator.GetMatrix(), TargetPos);

                    if (Config.AimWheelBone == 0)
                    {
                        TargetPos.Z += VectorHelper::RotateVector(VehicleRotator.GetMatrix(), { 0, 0, Wheel.ShapeRadius - 5 }).Z;
                    }
                    else if (Config.AimWheelBone == 1)
                    {
                        TargetPos.Z += VectorHelper::RotateVector(VehicleRotator.GetMatrix(), { 0, 0, 10 }).Z;
                    }
                    else if (Config.AimWheelBone == 3)
                    {
                        TargetPos.Z -= VectorHelper::RotateVector(VehicleRotator.GetMatrix(), { 0, 0, 10 }).Z;
                    }
                    else if (Config.AimWheelBone == 4)
                    {
                        TargetPos.Z -= VectorHelper::RotateVector(VehicleRotator.GetMatrix(), { 0, 0, Wheel.ShapeRadius - 5 }).Z;
                    }

                    //TargetPos.X -= VectorHelper::RotateVector(VehicleRotator.GetMatrix(), { 15, 0, 0 }).X;
                    //TargetPos.Y -= VectorHelper::RotateVector(VehicleRotator.GetMatrix(), { 0, 5, 0 }).Y;

                    //Utils::Log(1, " VehicleRotator %f %f %f", VehicleRotator.Yaw, VehicleRotator.Pitch, VehicleRotator.Roll);
                   // Utils::Log(1, " UpdatePost %f %f %f", UpdatePost.X, UpdatePost.Y, UpdatePost.Z);

                    TargetDistance = GameData.Camera.Location.Distance(Wheel.Location) / 100.0f;

                    if (Wheel.DampingRate > 2.f || Wheel.DampingRate == 0.1f || Wheel.DampingRate == 0.0f)
                    {
                        Wheel.State = WheelState::FlatTire;
                    }

                    if (Wheel.State == WheelState::FlatTire) {
                        StopAiming();
                        continue;
                    }
                }

                SHORT BulletNumber = 1.f;

                mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.AnimScriptInstance + GameData.Offset["bIsScoping_CP"], (bool*)&IsScoping);
                mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.AnimScriptInstance + GameData.Offset["bIsReloading_CP"], (bool*)&IsReloading);
                mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.AnimScriptInstance + GameData.Offset["RecoilADSRotation_CP"], (FRotator*)&Recoil);
                mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.AnimScriptInstance + GameData.Offset["ControlRotation_CP"], (FRotator*)&ControlRotation);
                mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.AnimScriptInstance + GameData.Offset["LeanLeftAlpha_CP"], (float*)&LeanLeftAlpha_CP);
                mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.AnimScriptInstance + GameData.Offset["LeanRightAlpha_CP"], (float*)&LeanRightAlpha_CP);
                mem.AddScatterRead(hScatter, CurrentWeaponData.Mesh3P + GameData.Offset["ComponentToWorld"], (FTransform*)&CurrentWeaponData.ComponentToWorld);
                mem.AddScatterRead(hScatter, GameData.LocalPlayerInfo.CurrentWeapon + GameData.Offset["CurrentAmmoData"], (SHORT*)&BulletNumber);

                mem.ExecuteReadScatter(hScatter);

                if (IsReloading || (Config.NoBulletNotAim && BulletNumber == 0))
                {
                    StopAiming();
                    continue;
                }

                if (Config.NoRecoil) Recoil.Yaw += (LeanRightAlpha_CP - LeanLeftAlpha_CP) * Recoil.Pitch / 3.0f;

                float ScopingAttachPointRelativeZ = 0.f;
                FTransform SocketWorldTransform;
                FTransform ScopeMeshComponentToWorld;
                GetScopingAttachPointRelativeZ(hScatter, CurrentWeaponData.ComponentToWorld, ScopingAttachPointRelativeZ, SocketWorldTransform, ScopeMeshComponentToWorld);
                float BulletDropAdd = ScopingAttachPointRelativeZ - CurrentWeaponData.ComponentToWorld.GetRelativeTransform(SocketWorldTransform).Translation.Z;

                //FTransform GunTransform = CurrentWeaponData.ScopeAimCameraSocket ? ScopeMeshComponentToWorld : CurrentWeaponData.ComponentToWorld;
                FTransform GunTransform = CurrentWeaponData.ComponentToWorld;
                FVector GunLocation = GunTransform.Translation;
                FRotator GunRotation = GunTransform.Rotation;
                FVector AimLocation = GunLocation.Length() > 0.0f ? GunLocation : GameData.LocalPlayerInfo.ComponentToWorld.Translation;
                FRotator AimRotation = (IsScoping) ? GunRotation : ControlRotation;

                if (!Config.NoRecoil && !Config.OriginalRecoil)
                {
                    AimRotation = AimRotation - Recoil;
                }
                else if (!Config.NoRecoil && Config.OriginalRecoil)
                {
                    AimRotation = ControlRotation;
                }

                // FRotator AimRotation = GunRotation;// ControlRotation + Recoil;
                TargetDistance = GameData.Camera.Location.Distance(TargetPos) / 100.0f;

                if (TargetDistance > Config.AimDistance)
                {
                    StopAiming();
                    continue;
                }


                float Aimxx = Config.AimPre_X;
                float Aimyy = Config.AimPre_Y;
                //float Aimzz = Config.AimPre_Z;
                auto ZeroingDistance = 100.0f;
                float BulletDrop = 0;
                float TravelTime = 0;
                float BallisticDragScale = 1;//步枪
                float BallisticDropScale = 1;
                //float BallisticDropScale = GetDragForce(TargetDistance, GameData.LocalPlayerInfo.WeaponEntityInfo.Name);//子弹上下   

                float TimeToReach = TargetDistance / CurrentWeaponData.TrajectoryConfigs.InitialSpeed;

                auto Result = GetBulletDropAndTravelTime(//弹道算法
                    AimLocation,
                    AimRotation,
                    TargetPos,
                    ZeroingDistance,
                    BulletDropAdd,
                    CurrentWeaponData.TrajectoryConfigs.InitialSpeed * Aimxx,
                    CurrentWeaponData.TrajectoryGravityZ * Aimyy,
                    BallisticDragScale, BallisticDropScale,
                    CurrentWeaponData.TrajectoryConfigs.BDS,
                    CurrentWeaponData.TrajectoryConfigs.SimulationSubstepTime,
                    CurrentWeaponData.TrajectoryConfigs.VDragCoefficient,
                    CurrentWeaponData.FloatCurves,
                    CurrentWeaponData.RichCurveKeys.size(),
                    CurrentWeaponData.RichCurveKeys

                );

                BulletDrop = Result.first;
                
                FVector PredictedPos = FVector(TargetPos.X, TargetPos.Y, TargetPos.Z);//+ TargetVelocity * (TravelTime / 1.f);
                //240-300米外开始预测 很少情况使用(0917 0033 阿震反馈太高了）
                //bool isBulletDrop = Config.IsBulletDrop;
                //bool PredPosBeginDistance = TargetDistance >= Config.PredPosStd;//目标大于设置预判阈值开启预判


                    
                 TravelTime = Result.second;
                 //新410测试算法
                 Vector3 TargetPos_410 = Vector3(TargetPos.X, TargetPos.Y, TargetPos.Z);
                 Vector3  Velocity = Vector3(TargetVelocity.X, TargetVelocity.Y, TargetVelocity.Z);
                 std::string WeaponNane = GameData.LocalPlayerInfo.WeaponEntityInfo.Name;// GNames::GetName(ObjectID);
                 /*if (WeaponNane == "" || WeaponNane == "fail") {
                     WeaponNane = "WeapAUG_C";
                 }
                 Utils::Log(1, "New 410 _ WeaponName: %s", WeaponNane.c_str());*/
                 float InitialSpeed = CurrentWeaponData.TrajectoryConfigs.InitialSpeed * Aimxx;
                 bool Weap_IsJu = isSnaperGun(WeaponNane);
                 if (InitialSpeed == 0)
                 {
                     InitialSpeed = 800;
                 }
                 float drop = GetInitialVelocitys(WeaponNane);

                 float AIM_H = GameData.Camera.Location.Z - TargetPos.Z;
                 //输出AIM_H	
                 //Utils::Log(1, "AIM_H: %f", AIM_H);
                 if (AIM_H > 300 || AIM_H < -300)
                 {
                     drop = drop * (1 + TargetDistance / 1000 / 20);
                 }
                 //if (Config.IsUsingNew1120_BulletDrop) {
                 //drop = drop * Aimxx;//11.20 更新了子弹下坠预判的算法 修改X值实现子弹下坠预判更新
                 //}
                 //Utils::Log(1, "New410_BulletDrop： %d", drop);


                 Vector3 actorPrePos = GetPrediction(TargetPos_410, TargetDistance, Velocity, InitialSpeed, 9.72f, Weap_IsJu, drop, false);//0.9.72  9.800000191f

                 PredictedPos = FVector(actorPrePos.x, actorPrePos.y, actorPrePos.z);

               
                if (!Config.Prediction)
                {
                    PredictedPos = TargetPos;
                }

                float CameraFOV = GameData.Camera.FOV;
                float DefaultFOV = 0.0f;
                float MouseXSensitivity = MouseX.Sensitivity;
                float MouseYSensitivity = MouseY.Sensitivity;
                float AimSpeedMaxFactor = Config.AimSpeedMaxFactor;

                //Utils::Log(1, "MouseXSensitivity: %f %f", MouseXSensitivity, MouseYSensitivity);

                if (IsNearlyZero(CameraFOV))
                    continue;
                if (IsNearlyZero(DefaultFOV))
                    DefaultFOV = 90.0f;

                const float FOVRatio = DefaultFOV / CameraFOV;


                auto GetMouseXY = [&](FRotator RotationInput) {
                    RotationInput.Clamp();
                    return FVector2D{
                        float(RotationInput.Yaw / MouseXSensitivity * 0.4f * FOVRatio),
                        float(-RotationInput.Pitch / MouseYSensitivity * 0.4f * FOVRatio) };
                    };

                FRotator RotationInput = (PredictedPos - GameData.Camera.Location).GetDirectionRotator() - AimRotation;
                RotationInput.Clamp();

                FVector2D MoveXY = GetMouseXY(RotationInput * AimSpeedMaxFactor);

                //  FVector2D MoveXY = { PredictedPos.X,PredictedPos .Y};

                GameData.AimBot.PredictedPos = PredictedPos;

                if (CanAim)
                {
                    if (GameData.Config.AimBot.Mode == 0)
                    {
                        
                        AimBotAPI(MoveXY, Config);
                        
                        //Utils::Log(1, "AimBot: %f %f", MoveXY.X, MoveXY.Y);
                        if (GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType == WeaponType::SG || GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType == WeaponType::SMG
                            || GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType == WeaponType::AR || GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType == WeaponType::DMR
                            || GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType == WeaponType::SR
                            
                            
                            ) {
                            
                            if (Config.AimAndShot) {

                                if (TargetDistance < Config.banjiAimDistance)
                                {
                                    AimBotAPI_SG(MoveXY, Config);
                                }

                            }

                        }
                        
                    }
                    else
                    {
                        AimBotAPI(hWriteScatter, MoveXY, Config);
                    }
                }
            }
            else {
                FistAim = false;
                StopAiming();
            }
            if (Config.AimAndShot) {
                if (IsReloading || (Config.NoBulletNotAim && BulletNumber == 0))
                {
                    std::cout << "no bullet" << std::endl;
                }
                else {
                    if (GameData.AimBot.Target == Player.Entity && GameData.AimBot.Lock) {

                        TriggerMouseLeft();
                    }
                }
                
            }
            //std::cout << timer.get() << std::endl;
            /*std::cout << time.get() << std::endl;
            Throttlered.executeTaskWithSleep("AimBotSleep", std::chrono::milliseconds(static_cast<int>(1000 / Config.FPS)), [] {});
            Utils::Log(1, "fps: %d %d", static_cast<int>(1000 / Config.FPS), Config.FPS);*/

        }
        mem.CloseScatterHandle(hScatter);
        mem.CloseScatterHandle(hWriteScatter);
    }
};