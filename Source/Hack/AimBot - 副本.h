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
    kmNet_mouse_left(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
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
    //mk47   gouza   g36c   win94   famasi  m249  AK    m762   AUG  
    static float GetDragzuoyou(float Distance, std::string WeaponEntityName) {
        float zuoyouOffDecay = 0.00f;
        if (Distance > 0 && Distance <= 50) {



        }

        if (Distance > 50 && Distance <= 100) {



        }

        if (Distance > 100 && Distance <= 150) {



        }
        if (Distance > 150 && Distance <= 200) {
            if (WeaponEntityName == "WeapHK416_C" || WeaponEntityName == "WeapDuncans_M416_C")
            {
                zuoyouOffDecay = 0.13f;

            }
            if (WeaponEntityName == "WeapMads_QBU88_C" || WeaponEntityName == "WeapQBU88_C")
            {
                zuoyouOffDecay = 0.13f;;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                zuoyouOffDecay = 0.13f;;

            }
            if (WeaponEntityName == "WeapSCAR-L_C")
            {
                zuoyouOffDecay = 0.14f;;

            }
            if (WeaponEntityName == "WeapQBZ95_C")
            {
                zuoyouOffDecay = 0.14f;;

            }
            if (WeaponEntityName == "WeapAUG_C")
            {
                zuoyouOffDecay = 0.14f;;

            }
            if (WeaponEntityName == "WeapAK47_C" || WeaponEntityName == "WeapLunchmeats_AK47_C")
            {
                zuoyouOffDecay = 0.16f;
            }
            if (WeaponEntityName == "WeapK2_C")
            {
                zuoyouOffDecay = 0.14f;;
            }
            if (WeaponEntityName == "WeapFamasG2_C")
            {
                zuoyouOffDecay = 0.15f;;

            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                zuoyouOffDecay = 0.14f;;

            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                zuoyouOffDecay = 0.14f;;
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                zuoyouOffDecay = 0.15f;;
            }

            if (WeaponEntityName == "WeapDragunov_C")
            {
                zuoyouOffDecay = 0.17f;
            }
            if (WeaponEntityName == "WeapBerylM762_C")
            {
                zuoyouOffDecay = 0.17f;

            }
            if (WeaponEntityName == "WeapG36C_C")
            {
                zuoyouOffDecay = 0.17f;

            }
            if (WeaponEntityName == "WeapACE32_C")
            {
                zuoyouOffDecay = 0.16f;

            }
            if (WeaponEntityName == "WeapM249_C")
            {
                zuoyouOffDecay = 0.16f;
            }
            if (WeaponEntityName == "WeapMG3_C")
            {
                zuoyouOffDecay = 0.15f;;
            }
            if (WeaponEntityName == "WeapGroza_C")//222222
            {
                zuoyouOffDecay = 0.17f;

            }
            if (WeaponEntityName == "WeapM16A4_C")
            {
                zuoyouOffDecay = 0.17f;
            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                zuoyouOffDecay = 0.19f;
            }
            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                zuoyouOffDecay = 0.25f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                zuoyouOffDecay = 0.32f;
            }
            if (WeaponEntityName == "WeapAWM_C")
            {
                zuoyouOffDecay = 0.34f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                zuoyouOffDecay = 0.25f;
            }
            if (WeaponEntityName == "WeapL6_C")
            {
                zuoyouOffDecay = 0.44f;
            }
            if (WeaponEntityName == "WeapMk47Mutant_C")
            {
                zuoyouOffDecay = 0.27f;

            }


            if (WeaponEntityName == "WeapWin94_C")//名字错了
            {
                zuoyouOffDecay = 0.22f;
            }

        }
        if (Distance > 200 && Distance <= 250) {


            if (WeaponEntityName == "WeapMads_QBU88_C" || WeaponEntityName == "WeapQBU88_C")
            {
                zuoyouOffDecay = 0.15f;;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                zuoyouOffDecay = 0.17f;;

            }
            if (WeaponEntityName == "WeapSCAR-L_C")
            {
                zuoyouOffDecay = 0.19f;;

            }
            if (WeaponEntityName == "WeapAUG_C")
            {
                zuoyouOffDecay = 0.19f;

            }
            if (WeaponEntityName == "WeapK2_C")
            {
                zuoyouOffDecay = 0.19f;
            }

            if (WeaponEntityName == "WeapHK416_C" || WeaponEntityName == "WeapDuncans_M416_C")
            {
                zuoyouOffDecay = 0.19f;

            }
            if (WeaponEntityName == "WeapQBZ95_C")
            {
                zuoyouOffDecay = 0.20f;

            }

            if (WeaponEntityName == "WeapAK47_C" || WeaponEntityName == "WeapLunchmeats_AK47_C")
            {
                zuoyouOffDecay = 0.221;
            }


            if (WeaponEntityName == "WeapSKS_C")
            {
                zuoyouOffDecay = 0.17f;

            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                zuoyouOffDecay = 0.21f;
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                zuoyouOffDecay = 0.20f;
            }

            if (WeaponEntityName == "WeapDragunov_C")
            {
                zuoyouOffDecay = 0.23f;
            }
            if (WeaponEntityName == "WeapBerylM762_C")
            {
                zuoyouOffDecay = 0.23f;

            }

            if (WeaponEntityName == "WeapACE32_C")
            {
                zuoyouOffDecay = 0.22f;

            }

            if (WeaponEntityName == "WeapMG3_C")
            {
                zuoyouOffDecay = 0.19f;
            }

            if (WeaponEntityName == "WeapM16A4_C")
            {
                zuoyouOffDecay = 0.24f;
            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                zuoyouOffDecay = 0.28f;
            }
            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                zuoyouOffDecay = 0.28f;
            }

            if (WeaponEntityName == "WeapWin94_C")//名字错了
            {
                zuoyouOffDecay = 0.27f;
            }
            if (WeaponEntityName == "WeapG36C_C")//Weapon_G36C_C
            {
                zuoyouOffDecay = 0.25f;//0.25

            }
            if (WeaponEntityName == "WeapMk47Mutant_C")
            {
                zuoyouOffDecay = 0.28f;

            }
            if (WeaponEntityName == "WeapFamasG2_C")
            {
                zuoyouOffDecay = 0.27f;//0.19

            }
            if (WeaponEntityName == "WeapM249_C")
            {
                zuoyouOffDecay = 0.37f;//0.23
            }
            if (WeaponEntityName == "WeapGroza_C")//222222
            {
                zuoyouOffDecay = 0.26f;//0.39

            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                zuoyouOffDecay = 0.19f;
            }
            if (WeaponEntityName == "WeapL6_C")
            {
                zuoyouOffDecay = 0.37f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                zuoyouOffDecay = 0.25f;
            }


            if (WeaponEntityName == "WeapAWM_C")
            {
                zuoyouOffDecay = 0.27f;
            }

        }
        if (Distance > 250 && Distance <= 300) {

            if (WeaponEntityName == "WeapMads_QBU88_C" || WeaponEntityName == "WeapQBU88_C")
            {
                zuoyouOffDecay = 0.16f;;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                zuoyouOffDecay = 0.18f;;

            }
            if (WeaponEntityName == "WeapSCAR-L_C")
            {
                zuoyouOffDecay = 0.21f;;
            }


            if (WeaponEntityName == "WeapAUG_C")
            {
                zuoyouOffDecay = 0.21f;

            }
            if (WeaponEntityName == "WeapK2_C")
            {
                zuoyouOffDecay = 0.20f;
            }

            if (WeaponEntityName == "WeapHK416_C" || WeaponEntityName == "WeapDuncans_M416_C")
            {
                zuoyouOffDecay = 0.22f;

            }
            if (WeaponEntityName == "WeapQBZ95_C")
            {
                zuoyouOffDecay = 0.21f;

            }




            if (WeaponEntityName == "WeapSKS_C")
            {
                zuoyouOffDecay = 0.18f;

            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                zuoyouOffDecay = 0.18f;
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                zuoyouOffDecay = 0.21f;
            }

            if (WeaponEntityName == "WeapDragunov_C")
            {
                zuoyouOffDecay = 0.22f;
            }
            if (WeaponEntityName == "WeapBerylM762_C")
            {
                zuoyouOffDecay = 0.25f;

            }

            if (WeaponEntityName == "WeapM16A4_C")
            {
                zuoyouOffDecay = 0.25f;
            }
            if (WeaponEntityName == "WeapWin94_C")//名字错了
            {
                zuoyouOffDecay = 0.25f;
            }
            if (WeaponEntityName == "WeapG36C_C")//Weapon_G36C_C
            {
                zuoyouOffDecay = 0.27f;//0.25

            }
            if (WeaponEntityName == "WeapMk47Mutant_C")      //1111111111111
            {
                zuoyouOffDecay = 0.28f;

            }

            if (WeaponEntityName == "WeapFamasG2_C")
            {
                zuoyouOffDecay = 0.29f;//0.19

            }
            if (WeaponEntityName == "WeapM249_C")
            {
                zuoyouOffDecay = 0.37f;//0.23
            }
            if (WeaponEntityName == "WeapGroza_C")//222222
            {
                zuoyouOffDecay = 0.26f;//0.39

            }
            if (WeaponEntityName == "WeapAK47_C" || WeaponEntityName == "WeapLunchmeats_AK47_C")
            {
                zuoyouOffDecay = 0.26;
            }
            if (WeaponEntityName == "WeapACE32_C")
            {
                zuoyouOffDecay = 0.25f;

            }
            if (WeaponEntityName == "WeapMG3_C")
            {
                zuoyouOffDecay = 0.30f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                zuoyouOffDecay = 0.25f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                zuoyouOffDecay = 0.21f;
            }
            if (WeaponEntityName == "WeapL6_C")
            {
                zuoyouOffDecay = 0.36f;
            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                zuoyouOffDecay = 0.27f;
            }
            if (WeaponEntityName == "WeapAWM_C")
            {
                zuoyouOffDecay = 0.28f;
            }
            //                    300




            if (WeaponEntityName == "WeapKar98k_C")
            {
                zuoyouOffDecay = 0.20f;//0.24    WeapKar98k_C    WeapJuliesKar98k_C
            }



        }
        if (Distance > 300 && Distance <= 350)
        {
            if (WeaponEntityName == "WeapMads_QBU88_C" || WeaponEntityName == "WeapQBU88_C")
            {
                zuoyouOffDecay = 0.17f;;
            }
            if (WeaponEntityName == "WeapSCAR-L_C")
            {
                zuoyouOffDecay = 0.22f;;
            }


            if (WeaponEntityName == "WeapAUG_C")
            {
                zuoyouOffDecay = 0.22f;

            }
            if (WeaponEntityName == "WeapK2_C")
            {
                zuoyouOffDecay = 0.21f;
            }
            if (WeaponEntityName == "WeapHK416_C" || WeaponEntityName == "WeapDuncans_M416_C")
            {
                zuoyouOffDecay = 0.23f;

            }
            if (WeaponEntityName == "WeapQBZ95_C")
            {
                zuoyouOffDecay = 0.22f;

            }
            if (WeaponEntityName == "WeapM16A4_C")
            {
                zuoyouOffDecay = 0.26f;
            }
            if (WeaponEntityName == "WeapG36C_C")//Weapon_G36C_C
            {
                zuoyouOffDecay = 0.28f;

            }
            if (WeaponEntityName == "WeapFamasG2_C")
            {
                zuoyouOffDecay = 0.30f;

            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                zuoyouOffDecay = 0.23f;
            }
            if (WeaponEntityName == "WeapBerylM762_C")
            {
                zuoyouOffDecay = 0.26f;

            }
            if (WeaponEntityName == "WeapM249_C")
            {
                zuoyouOffDecay = 0.37f;
            }
            if (WeaponEntityName == "WeapAK47_C" || WeaponEntityName == "WeapLunchmeats_AK47_C")
            {
                zuoyouOffDecay = 0.27;
            }
            if (WeaponEntityName == "WeapACE32_C")
            {
                zuoyouOffDecay = 0.26f;

            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                zuoyouOffDecay = 0.22f;
            }

            if (WeaponEntityName == "WeapMG3_C")
            {
                zuoyouOffDecay = 0.31f;
            }
            if (WeaponEntityName == "WeapL6_C")
            {
                zuoyouOffDecay = 0.37f;
            }
            if (WeaponEntityName == "WeapKar98k_C")
            {
                zuoyouOffDecay = 0.20f;//0.24    WeapKar98k_C    WeapJuliesKar98k_C
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                zuoyouOffDecay = 0.23f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                zuoyouOffDecay = 0.24f;
            }

            if (WeaponEntityName == "WeapFNFal_C")
            {
                zuoyouOffDecay = 0.26f;
            }

            if (WeaponEntityName == "WeapAWM_C")
            {
                zuoyouOffDecay = 0.26f;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                zuoyouOffDecay = 0.25f;;

            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                zuoyouOffDecay = 0.25f;

            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                zuoyouOffDecay = 0.19f;
            }









            if (WeaponEntityName == "WeapWin94_C")//高度问题
            {
                zuoyouOffDecay = 0.25f;
            }

            if (WeaponEntityName == "WeapMk47Mutant_C") //高度问题
            {
                zuoyouOffDecay = 0.28f;

            }



            if (WeaponEntityName == "WeapGroza_C")//高度问题
            {
                zuoyouOffDecay = 0.26f;

            }




        }
        if (Distance > 350 && Distance <= 400) {

            if (WeaponEntityName == "WeapMads_QBU88_C" || WeaponEntityName == "WeapQBU88_C")
            {
                zuoyouOffDecay = 0.17f;;
            }

            if (WeaponEntityName == "WeapM16A4_C")
            {
                zuoyouOffDecay = 0.26f;
            }

            if (WeaponEntityName == "WeapDragunov_C")
            {
                zuoyouOffDecay = 0.23f;
            }

            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                zuoyouOffDecay = 0.22f;
            }


            if (WeaponEntityName == "WeapL6_C")
            {
                zuoyouOffDecay = 0.37f;
            }
            if (WeaponEntityName == "WeapKar98k_C")
            {
                zuoyouOffDecay = 0.20f;//0.24    WeapKar98k_C    WeapJuliesKar98k_C
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                zuoyouOffDecay = 0.23f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                zuoyouOffDecay = 0.24f;
            }

            if (WeaponEntityName == "WeapFNFal_C")
            {
                zuoyouOffDecay = 0.26f;
            }

            if (WeaponEntityName == "WeapAWM_C")
            {
                zuoyouOffDecay = 0.26f;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                zuoyouOffDecay = 0.25f;;

            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                zuoyouOffDecay = 0.25f;

            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                zuoyouOffDecay = 0.19f;
            }









            if (WeaponEntityName == "WeapWin94_C")//高度问题
            {
                zuoyouOffDecay = 0.25f;
            }

            if (WeaponEntityName == "WeapMk47Mutant_C") //高度问题
            {
                zuoyouOffDecay = 0.28f;

            }





        }
        if (Distance > 400 && Distance <= 450) {



        }
        if (Distance > 450 && Distance <= 500) {


            if (WeaponEntityName == "WeapAWM_C")
            {
                zuoyouOffDecay = 0.88f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                zuoyouOffDecay = 0.68f;
            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                zuoyouOffDecay = 1.12f;

            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                zuoyouOffDecay = 0.83f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                zuoyouOffDecay = 0.92f;
            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                zuoyouOffDecay = 1.04f;
            }
            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                zuoyouOffDecay = 0.84f;
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                zuoyouOffDecay = 1.18f;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                zuoyouOffDecay = 1.14f;
            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                zuoyouOffDecay = zuoyouOffDecay = 0.98f;
            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                zuoyouOffDecay = 1.00f;
            }


        }
        if (Distance > 500 && Distance <= 550) {

            if (WeaponEntityName == "WeapAWM_C")
            {
                zuoyouOffDecay = 0.89f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                zuoyouOffDecay = 0.71f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                zuoyouOffDecay = 0.84f;
            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                zuoyouOffDecay = 1.22f;

            }
            if (WeaponEntityName == "WeapM24_C")
            {
                zuoyouOffDecay = 0.99f;
            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                zuoyouOffDecay = 1.07f;
            }
            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                zuoyouOffDecay = 0.90f;
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                zuoyouOffDecay = 1.29f;
            }

            if (WeaponEntityName == "WeapMk14_C")
            {
                zuoyouOffDecay = 1.06f;
            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                zuoyouOffDecay = 1.10f;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                zuoyouOffDecay = 1.24f;//1.24

            }

        }
        if (Distance > 550 && Distance <= 600) {

            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                zuoyouOffDecay = 0.91f;
            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                zuoyouOffDecay = 1.07f;
            }


            if (WeaponEntityName == "WeapAWM_C")
            {
                zuoyouOffDecay = 0.95f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                zuoyouOffDecay = 0.81f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                zuoyouOffDecay = 0.89f;
            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                zuoyouOffDecay = 1.17f;
            }

            if (WeaponEntityName == "WeapFNFal_C")
            {
                zuoyouOffDecay = 1.20f;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                zuoyouOffDecay = 1.34f;//1.24

            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                zuoyouOffDecay = 1.34f;
            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                zuoyouOffDecay = 1.24f;

            }
            if (WeaponEntityName == "WeapM24_C")
            {
                zuoyouOffDecay = 1.05f;
            }
        }
        if (Distance > 600 && Distance <= 650) {

            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                zuoyouOffDecay = 0.94f;
            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                zuoyouOffDecay = 1.09f;
            }


            if (WeaponEntityName == "WeapAWM_C")
            {
                zuoyouOffDecay = 1.00f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                zuoyouOffDecay = 0.85f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                zuoyouOffDecay = 0.94f;
            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                zuoyouOffDecay = 1.19f;
            }

            if (WeaponEntityName == "WeapFNFal_C")
            {
                zuoyouOffDecay = 1.23f;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                zuoyouOffDecay = 1.46f;//1.24

            }


            if (WeaponEntityName == "WeapM24_C")
            {
                zuoyouOffDecay = 1.05f;
            }


            if (WeaponEntityName == "WeapMk12_C")
            {
                zuoyouOffDecay = 1.42f;
            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                zuoyouOffDecay = 1.34f;

            }

        }
        if (Distance > 650 && Distance <= 700) {

            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                zuoyouOffDecay = 0.95f;
            }
            if (WeaponEntityName == "WeapAWM_C")
            {
                zuoyouOffDecay = 1.01f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                zuoyouOffDecay = 0.97f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                zuoyouOffDecay = 0.91f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                zuoyouOffDecay = 1.06f;
            }

        }
        if (Distance > 700 && Distance <= 750) {

            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                zuoyouOffDecay = 1.26f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                zuoyouOffDecay = 1.07f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                zuoyouOffDecay = 1.11f;
            }


            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                zuoyouOffDecay = 0.97f;
            }

            if (WeaponEntityName == "WeapAWM_C")
            {
                zuoyouOffDecay = 1.06f;
            }
        }

        return zuoyouOffDecay;



    }
   
    static float GetDragForce(float Distance, std::string WeaponEntityName) {
        // std::string WeaponEntityName = GameData.LocalPlayerInfo.WeaponEntityInfo.Name;
        float FallOffDecay = 0.00f;

        if (Distance > 10 && Distance <= 200) {

            if (WeaponEntityName == "WeapG36C_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapSCAR-L_C")
            {
                FallOffDecay = 0.54f;

            }

            if (WeaponEntityName == "WeapQBZ95_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapM16A4_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapMk47Mutant_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapMads_QBU88_C" || WeaponEntityName == "WeapQBU88_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapAK47_C" || WeaponEntityName == "WeapLunchmeats_AK47_C")
            {
                FallOffDecay = 0.49f;

            }
            if (WeaponEntityName == "WeapHK416_C" || WeaponEntityName == "WeapDuncans_M416_C")
            {
                FallOffDecay = 0.50f;

            }
            if (WeaponEntityName == "WeapAUG_C")
            {
                FallOffDecay = 0.50f;

            }
            if (WeaponEntityName == "WeapACE32_C")
            {
                FallOffDecay = 0.48f;

            }
            if (WeaponEntityName == "WeapK2_C")
            {
                FallOffDecay = 0.52f;

            }

            if (WeaponEntityName == "WeapM249_C")
            {
                FallOffDecay = 0.50f;
            }
            if (WeaponEntityName == "WeapBerylM762_C")
            {
                FallOffDecay = 0.33f;

            }








            if (WeaponEntityName == "WeapGroza_C")//WeapGroza_C
            {
                FallOffDecay = 0.006f;

            }





            if (WeaponEntityName == "WeapFamasG2_C")
            {
                FallOffDecay = 0.006f;

            }





            if (WeaponEntityName == "WeapMk12_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapAWM_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapL6_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapWin94_C")
            {
                FallOffDecay = 0.59f;
            }

            if (WeaponEntityName == "WeapMG3_C")
            {
                FallOffDecay = 0.52f;
            }
        }
        if (Distance > 200 && Distance <= 250) {
            if (WeaponEntityName == "WeapAK47_C" || WeaponEntityName == "WeapLunchmeats_AK47_C")
            {
                FallOffDecay = 0.60f;

            }
            if (WeaponEntityName == "WeapGroza_C")
            {
                FallOffDecay = 0.01f;

            }
            if (WeaponEntityName == "WeapBerylM762_C")
            {
                FallOffDecay = 0.36f;

            }

            if (WeaponEntityName == "WeapMk47Mutant_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapWin94_C")//抬枪低
            {
                FallOffDecay = 0.62f;
            }
            if (WeaponEntityName == "WeapM249_C")
            {
                FallOffDecay = 0.70f;
            }







            if (WeaponEntityName == "WeapFamasG2_C")
            {
                FallOffDecay = 0.01f;

            }
            if (WeaponEntityName == "WeapG36C_C")
            {
                FallOffDecay = 0.56f;

            }
            if (WeaponEntityName == "WeapHK416_C" || WeaponEntityName == "WeapDuncans_M416_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapSCAR-L_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapQBZ95_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapAUG_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapACE32_C")
            {
                FallOffDecay = 0.51f;

            }
            if (WeaponEntityName == "WeapK2_C")
            {
                FallOffDecay = 0.54f;

            }

            if (WeaponEntityName == "WeapMads_QBU88_C" || WeaponEntityName == "WeapQBU88_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapMG3_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapM16A4_C")
            {
                FallOffDecay = 0.54;
            }



            if (WeaponEntityName == "WeapMk14_C")
            {
                FallOffDecay = 0.57f;
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                FallOffDecay = 0.57f;
            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                FallOffDecay = 0.57f;
            }
            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                FallOffDecay = 0.57f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                FallOffDecay = 0.70f;
            }
            if (WeaponEntityName == "WeapAWM_C")
            {
                FallOffDecay = 0.70f;
            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                FallOffDecay = 0.70f;

            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                FallOffDecay = 0.70f;

            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                FallOffDecay = 0.59f;

            }
        }
        if (Distance > 250 && Distance <= 300) {
            if (WeaponEntityName == "WeapAK47_C" || WeaponEntityName == "WeapLunchmeats_AK47_C")
            {
                FallOffDecay = 0.65f;

            }
            if (WeaponEntityName == "WeapGroza_C")
            {
                FallOffDecay = 0.15f;

            }
            if (WeaponEntityName == "WeapBerylM762_C")
            {
                FallOffDecay = 0.039f;

            }
            if (WeaponEntityName == "WeapG36C_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapHK416_C" || WeaponEntityName == "WeapDuncans_M416_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapSCAR-L_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapQBZ95_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapAUG_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapACE32_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapK2_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapFamasG2_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapMads_QBU88_C" || WeaponEntityName == "WeapQBU88_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapMG3_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapM16A4_C")
            {
                FallOffDecay = 0.54;
            }



            if (WeaponEntityName == "WeapMk14_C")
            {
                FallOffDecay = 0.57f;
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                FallOffDecay = 0.57f;
            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                FallOffDecay = 0.57f;
            }
            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                FallOffDecay = 0.57f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                FallOffDecay = 0.70f;
            }
            if (WeaponEntityName == "WeapAWM_C")
            {
                FallOffDecay = 0.70f;
            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                FallOffDecay = 0.70f;

            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                FallOffDecay = 0.70f;

            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                FallOffDecay = 0.59f;

            }

            if (WeaponEntityName == "WeapM249_C")
            {
                FallOffDecay = 0.70f;
            }




            if (WeaponEntityName == "WeapMk47Mutant_C")
            {
                FallOffDecay = 2.00f;

            }
            if (WeaponEntityName == "WeapWin1894_C")//抬枪低
            {
                FallOffDecay = 1.10f;
            }




        }
        if (Distance > 300 && Distance <= 350) {

            if (WeaponEntityName == "WeapGroza_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapBerylM762_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapAK47_C" || WeaponEntityName == "WeapLunchmeats_AK47_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapG36C_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapHK416_C" || WeaponEntityName == "WeapDuncans_M416_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapSCAR-L_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapQBZ95_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapAUG_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapACE32_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapK2_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapFAMASG2_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapMG3_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapM16A4_C")
            {
                FallOffDecay = 0.54;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                FallOffDecay = 0.70f;
            }
            if (WeaponEntityName == "WeapAWM_C")
            {
                FallOffDecay = 0.70f;
            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                FallOffDecay = 0.70f;

            }
            if (WeaponEntityName == "WeapM249_C")
            {
                FallOffDecay = 0.70f;
            }
            if (WeaponEntityName == "WeapMads_QBU88_C" || WeaponEntityName == "WeapQBU88_C")
            {
                FallOffDecay = 0.59f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                FallOffDecay = 0.57f;
            }
            if (WeaponEntityName == "WeapMk47Mutant_C")
            {
                FallOffDecay = 2.80f;

            }
            if (WeaponEntityName == "WeapWin94_C")//抬枪低
            {
                FallOffDecay = 2.41f;
            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                FallOffDecay = 0.65f;
            }
            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                FallOffDecay = 0.60f;
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                FallOffDecay = 0.71f;
            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                FallOffDecay = 0.72f;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                FallOffDecay = 0.81f;

            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                FallOffDecay = 0.72f;

            }
        }
        if (Distance > 350 && Distance <= 400)
        {
            if (WeaponEntityName == "WeapGroza_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapBerylM762_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapAK47_C" || WeaponEntityName == "WeapLunchmeats_AK47_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapG36C_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapHK416_C" || WeaponEntityName == "WeapDuncans_M416_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapSCAR-L_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapQBZ95_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapAUG_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapACE32_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapK2_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapFAMASG2_C")
            {
                FallOffDecay = 0.54f;

            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapMG3_C")
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapM249_C")
            {
                FallOffDecay = 0.70f;
            }
            if (WeaponEntityName == "WeapM16A4_C")
            {
                FallOffDecay = 0.57;
            }
            if (WeaponEntityName == "WeapAWM_C")
            {
                FallOffDecay = 0.71f;
            }
            if (WeaponEntityName == "WeapMads_QBU88_C" || WeaponEntityName == "WeapQBU88_C")
            {
                FallOffDecay = 0.69f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                FallOffDecay = 0.63f;
            }
            if (WeaponEntityName == "WeapMk47Mutant_C")
            {
                FallOffDecay = 5.20f;

            }
            if (WeaponEntityName == "WeapWin94_C")//抬枪低
            {
                FallOffDecay = 4.51f;
            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                FallOffDecay = 0.70f;
            }
            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                FallOffDecay = 0.69f;
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                FallOffDecay = 0.81f;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                FallOffDecay = 0.85f;

            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                FallOffDecay = 0.81f;

            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                FallOffDecay = 0.78f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                FallOffDecay = 0.75f;
            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                FallOffDecay = 0.78f;
            }

        }
        if (Distance > 400 && Distance <= 450) {


            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                FallOffDecay = 0.54f;
            }
            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                FallOffDecay = 0.70f;
            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                FallOffDecay = 0.81f;

            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                FallOffDecay = 0.78f;
            }
            if (WeaponEntityName == "WeapM16A4_C")
            {
                FallOffDecay = 0.57;
            }
            if (WeaponEntityName == "WeapAWM_C")
            {
                FallOffDecay = 0.77f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                FallOffDecay = 0.68f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                FallOffDecay = 0.83f;
            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                FallOffDecay = 0.87f;///1111111111111111111111111
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                FallOffDecay = 0.85f;
            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                FallOffDecay = 0.78f;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                FallOffDecay = 0.9f;

            }
        }
        if (Distance > 450 && Distance <= 500) {

            if (WeaponEntityName == "WeapAWM_C")
            {
                FallOffDecay = 0.78f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                FallOffDecay = 0.56f;
            }
            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                FallOffDecay = 0.81f;
            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                FallOffDecay = 1.01f;

            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                FallOffDecay = 0.98f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                FallOffDecay = 0.78f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                FallOffDecay = 0.87f;
            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                FallOffDecay = 1.01f;
            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                FallOffDecay = 0.88f;
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                FallOffDecay = 1.08f;
            }

            if (WeaponEntityName == "WeapMini14_C")
            {
                FallOffDecay = 1.1f;

            }

        }
        if (Distance > 500 && Distance <= 550) {


            if (WeaponEntityName == "WeapAWM_C")
            {
                FallOffDecay = 0.88f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                FallOffDecay = 0.68f;
            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                FallOffDecay = 1.12f;

            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                FallOffDecay = 0.83f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                FallOffDecay = 0.92f;
            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                FallOffDecay = 1.04f;
            }
            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                FallOffDecay = 0.84f;
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                FallOffDecay = 1.18f;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                FallOffDecay = 1.14f;
            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                FallOffDecay = FallOffDecay = 0.98f;
            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                FallOffDecay = 1.00f;
            }


        }
        if (Distance > 550 && Distance <= 600) {

            if (WeaponEntityName == "WeapAWM_C")
            {
                FallOffDecay = 0.89f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                FallOffDecay = 0.71f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                FallOffDecay = 0.84f;
            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                FallOffDecay = 1.22f;

            }
            if (WeaponEntityName == "WeapM24_C")
            {
                FallOffDecay = 0.99f;
            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                FallOffDecay = 1.07f;
            }
            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                FallOffDecay = 0.90f;
            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                FallOffDecay = 1.29f;
            }

            if (WeaponEntityName == "WeapMk14_C")
            {
                FallOffDecay = 1.06f;
            }
            if (WeaponEntityName == "WeapFNFal_C")
            {
                FallOffDecay = 1.10f;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                FallOffDecay = 1.24f;//1.24

            }

        }
        if (Distance > 600 && Distance <= 650) {

            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                FallOffDecay = 0.91f;
            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                FallOffDecay = 1.07f;
            }


            if (WeaponEntityName == "WeapAWM_C")
            {
                FallOffDecay = 0.95f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                FallOffDecay = 0.81f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                FallOffDecay = 0.89f;
            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                FallOffDecay = 1.17f;
            }

            if (WeaponEntityName == "WeapFNFal_C")
            {
                FallOffDecay = 1.20f;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                FallOffDecay = 1.34f;//1.24

            }
            if (WeaponEntityName == "WeapMk12_C")
            {
                FallOffDecay = 1.34f;
            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                FallOffDecay = 1.24f;

            }
            if (WeaponEntityName == "WeapM24_C")
            {
                FallOffDecay = 1.05f;
            }
        }
        if (Distance > 650 && Distance <= 700) {

            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                FallOffDecay = 0.94f;
            }
            if (WeaponEntityName == "WeapMk14_C")
            {
                FallOffDecay = 1.09f;
            }


            if (WeaponEntityName == "WeapAWM_C")
            {
                FallOffDecay = 1.00f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                FallOffDecay = 0.85f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                FallOffDecay = 0.94f;
            }
            if (WeaponEntityName == "WeapDragunov_C")
            {
                FallOffDecay = 1.19f;
            }

            if (WeaponEntityName == "WeapFNFal_C")
            {
                FallOffDecay = 1.23f;
            }
            if (WeaponEntityName == "WeapMini14_C")
            {
                FallOffDecay = 1.46f;//1.24

            }


            if (WeaponEntityName == "WeapM24_C")
            {
                FallOffDecay = 1.05f;
            }


            if (WeaponEntityName == "WeapMk12_C")
            {
                FallOffDecay = 1.42f;
            }
            if (WeaponEntityName == "WeapSKS_C")
            {
                FallOffDecay = 1.34f;

            }

        }
        if (Distance > 700 && Distance <= 750) {

            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                FallOffDecay = 0.95f;
            }
            if (WeaponEntityName == "WeapAWM_C")
            {
                FallOffDecay = 1.01f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                FallOffDecay = 0.97f;
            }
            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                FallOffDecay = 0.91f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                FallOffDecay = 1.06f;
            }

        }
        if (Distance > 750 && Distance <= 800) {

            if (WeaponEntityName == "WeapJulies_Kar98k_C" || WeaponEntityName == "WeapKar98k_C")
            {
                FallOffDecay = 0.96f;
            }
            if (WeaponEntityName == "WeapL6_C")//抬枪
            {
                FallOffDecay = 1.07f;
            }
            if (WeaponEntityName == "WeapM24_C")
            {
                FallOffDecay = 1.11f;
            }


            if (WeaponEntityName == "WeapMosinNagant_C" || WeaponEntityName == "WeapMosin_C")
            {
                FallOffDecay = 0.97f;
            }

            if (WeaponEntityName == "WeapAWM_C")
            {
                FallOffDecay = 1.06f;
            }
        }

        return FallOffDecay;
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
        //MouseX = round(MouseX);
        //MouseY = round(MouseY);
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

        const float Threshold = 3.f;//靠近人物的范围，越小需要越靠近目标才会开枪，提高准确度，但是会降低容错，越低，开枪概率越小
        if (abs(MoveXY.X) < Threshold && abs(MoveXY.Y) < Threshold) {
            TriggerMouseLeft();
        }
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
        //Utils::Log(1, "Move: %f %f", MouseX, MouseY);
        if (abs(MouseX) > 0 || abs(MouseY) > 0) {
            //  Utils::Log(1, "Move: %f %f", MouseX, MouseY);
            Move(MouseX, MouseY);
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
    struct FTransform_MOD
    {
        float Rotation_X;
        float Rotation_Y;
        float Rotation_Z;
        float Rotation_W;
        float Translation_X;
        float Translation_Y;
        float Translation_Z;
        float Null;
        float Scale3D_X;
        float Scale3D_Y;
        float Scale3D_Z;
    };
    // 判断是否狙击枪
    static bool isSnaperGun(const std::string& ID1) {
        if (ID1 == "WeapMosinNagant_C" || ID1 == "WeapMk14_C" || ID1 == "WeapM24_C" ||
            ID1 == "WeapAWM_C" || ID1 == "WeapMadsQBU88_C" || ID1 == "WeapQBU88_C" ||
            ID1 == "WeapMini14_C" || ID1 == "WeapJuliesKar98k_C" || ID1 == "WeapKar98k_C" ||
            ID1 == "WeapWin94_C" || ID1 == "WeapSKS_C" || ID1 == "WeapFNFal_C" || ID1 == "WeapMk47Mutant_C") {
            return true;
        }
        return false;
    }

    static float GetInitialVelocitys(const std::string& ID1)
    {
        if (ID1 == "WeapAK47_C" || ID1 == "WeapLunchmeatsAK47_C")
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
            return 45.0f;
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
    static Vector3 Aim_Player(uint64_t BoneArry, FTransform_MOD Mat, int ID)
    {
        Vector3 VE = Vector3(0, 0, 0), Pos = Vector3(0, 0, 0);
        float X, Y, Z, _X, _Y, _Z, _11, _12, _13, _21, _22, _23, _31, _32, _33;
        X = Mat.Rotation_X + Mat.Rotation_X;
        Y = Mat.Rotation_Y + Mat.Rotation_Y;
        Z = Mat.Rotation_Z + Mat.Rotation_Z;
        _X = Mat.Rotation_X * X;
        _Y = Mat.Rotation_Y * Y;
        _Z = Mat.Rotation_Z * Z;
        _11 = (1 - (_Y + _Z)) * Mat.Scale3D_X;
        _22 = (1 - (_X + _Z)) * Mat.Scale3D_Y;
        _33 = (1 - (_X + _Y)) * Mat.Scale3D_Z;
        _Z = Mat.Rotation_Y * Z;
        _X = Mat.Rotation_W * X;
        _32 = (_Z - _X) * Mat.Scale3D_Z;
        _23 = (_Z + _X) * Mat.Scale3D_Y;
        _Y = Mat.Rotation_X * Y;
        _Z = Mat.Rotation_W * Z;
        _21 = (_Y - _Z) * Mat.Scale3D_Y;
        _12 = (_Y + _Z) * Mat.Scale3D_X;
        _Y = Mat.Rotation_W * Y;
        _Z = Mat.Rotation_X * Z;
        _31 = (_Z + _Y) * Mat.Scale3D_Z;
        _13 = (_Z - _Y) * Mat.Scale3D_X;

        VE = mem.Read<Vector3>(BoneArry + ID);

        Pos.x = VE.x * _11 + VE.y * _21 + VE.z * _31 + 1 * Mat.Translation_X;
        Pos.y = VE.x * _12 + VE.y * _22 + VE.z * _32 + 1 * Mat.Translation_Y;
        Pos.z = VE.x * _13 + VE.y * _23 + VE.z * _33 + 1 * Mat.Translation_Z;

        return Pos;
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
            drop = 55.f;
        PredictedPos.z = target.z + 0.4f * m_gravity * flyTime2 * drop;
        if (!IsVehicle)
            PredictedPos.z -= GetZPosition(GameData.AimBot.Target, target);
        return PredictedPos;
    }
    static void GetAxes(Vector3 Rotation, Vector3* X, Vector3* Y, Vector3* Z)
    {
        float radPitch = (Rotation.x * float(M_PI) / 180.f);
        float radYaw = (Rotation.y * float(M_PI) / 180.f);
        float radRoll = (Rotation.z * float(M_PI) / 180.f);

        float SP = sinf(radPitch);
        float CP = cosf(radPitch);
        float SY = sinf(radYaw);
        float CY = cosf(radYaw);
        float SR = sinf(radRoll);
        float CR = cosf(radRoll);

        X->x = CP * CY;
        X->y = CP * SY;
        X->z = SP;

        Y->x = SR * SP * CY - CR * SY;
        Y->y = SR * SP * SY + CR * CY;
        Y->z = -SR * CP;

        Z->x = -(CR * SP * CY + SR * SY);
        Z->y = CY * SR - CR * SP * SY;
        Z->z = CR * CP;
    }

    static void SubVector(Vector3 VecA, Vector3 VecB, Vector3* VecC)
    {
        VecC->x = VecA.x - VecB.x;
        VecC->y = VecA.y - VecB.y;
        VecC->z = VecA.z - VecB.z;
    }

    static float DotProduct(Vector3 VecA, Vector3 VecB)
    {
        return VecA.x * VecB.x + VecA.y * VecB.y + VecA.z * VecB.z;
    }

    static bool WorldScreenAA(Vector3 Location, Vector2* Screen, float* Distance)
    {
        Vector3 Temp = Vector3(0, 0, 0);
        float X, Y, Z;
        SubVector(Location, Pos, &Temp);
        Z = DotProduct(Temp, vAxisX);
        if (Z < 100099) {
            if (Z > 1) {
                X = DotProduct(Temp, vAxisY);
                Y = DotProduct(Temp, vAxisZ);
                X = GameCenterX + X * GameCenterX / Tangent / Z;
                Y = GameCenterY - Y * GameCenterX / Tangent / Z;
                if (X <= 0 || Y <= 0) { return false; }
                Screen->x = X;
                Screen->y = Y;
                *Distance = Z / 100;
                return true;
            }
            return false;
        }
        return false;
    }
    static bool ValidPtr(uint64_t Ptr)
    {
        return (bool)(Ptr < 0x1000000 || Ptr > 0x7FFFFFF00000 || Ptr % sizeof(uint64_t));
    }
    static bool WorldScreen(Vector3 Location, Vector2* Screen, float* Distance)
    {
        Vector3 Temp = Vector3(0, 0, 0);
        float X, Y, Z;
        SubVector(Location, Pos, &Temp);
        Z = DotProduct(Temp, AxisX);
        if (Z < 100099) {
            if (Z > 1) {
                X = DotProduct(Temp, AxisY);
                Y = DotProduct(Temp, AxisZ);
                X = GameCenterX + X * GameCenterX / Tangent / Z;
                Y = GameCenterY - Y * GameCenterX / Tangent / Z;
                if (X <= 0 || Y <= 0) { return false; }
                Screen->x = X;
                Screen->y = Y;
                *Distance = Z / 100;
                return true;
            }
            return false;
        }
        return false;
    }
    static void DrawPlayer(Vector2* Aim_v2, Vector3* Aim_v3, bool isHead, uint64_t BoneArry, FTransform_MOD Mat)
    {

        Vector3 temp_V3;

        float Distance;
        Vector3 VE = Vector3(0, 0, 0), Pos = Vector3(0, 0, 0);
        Vector2 Screen;
        float X, Y, Z, _X, _Y, _Z, _11, _12, _13, _21, _22, _23, _31, _32, _33;
        float bx, by, vx, vy, tx, ty, dx, dy, mX, my, mw, mh;

        X = Mat.Rotation_X + Mat.Rotation_X;
        Y = Mat.Rotation_Y + Mat.Rotation_Y;
        Z = Mat.Rotation_Z + Mat.Rotation_Z;
        _X = Mat.Rotation_X * X;
        _Y = Mat.Rotation_Y * Y;
        _Z = Mat.Rotation_Z * Z;
        _11 = (1 - (_Y + _Z)) * Mat.Scale3D_X;
        _22 = (1 - (_X + _Z)) * Mat.Scale3D_Y;
        _33 = (1 - (_X + _Y)) * Mat.Scale3D_Z;
        _Z = Mat.Rotation_Y * Z;
        _X = Mat.Rotation_W * X;
        _32 = (_Z - _X) * Mat.Scale3D_Z;
        _23 = (_Z + _X) * Mat.Scale3D_Y;
        _Y = Mat.Rotation_X * Y;
        _Z = Mat.Rotation_W * Z;
        _21 = (_Y - _Z) * Mat.Scale3D_Y;
        _12 = (_Y + _Z) * Mat.Scale3D_X;
        _Y = Mat.Rotation_W * Y;
        _Z = Mat.Rotation_X * Z;
        _31 = (_Z + _Y) * Mat.Scale3D_Z;
        _13 = (_Z - _Y) * Mat.Scale3D_X;

        VE = mem.Read<Vector3>(BoneArry + 736);

        Pos.x = VE.x * _11 + VE.y * _21 + VE.z * _31 + 1 * Mat.Translation_X;
        Pos.y = VE.x * _12 + VE.y * _22 + VE.z * _32 + 1 * Mat.Translation_Y;
        Pos.z = VE.x * _13 + VE.y * _23 + VE.z * _33 + 1 * Mat.Translation_Z;

        WorldScreen(Pos, &Screen, &Distance);



        Utils::Log(1, "get DP OK...");
        mX = Screen.x;
        my = Screen.y;
        *Aim_v2 = Screen;
        temp_V3 = Pos;

        if (!isHead)
        {
            VE = mem.Read<Vector3>(BoneArry + 208);

            Pos.x = VE.x * _11 + VE.y * _21 + VE.z * _31 + 1 * Mat.Translation_X;
            Pos.y = VE.x * _12 + VE.y * _22 + VE.z * _32 + 1 * Mat.Translation_Y;
            Pos.z = VE.x * _13 + VE.y * _23 + VE.z * _33 + 1 * Mat.Translation_Z;
            temp_V3 = Pos;
        }
        *Aim_v3 = temp_V3;
        AIM_Dis = (temp_V3 - LocalPos).Length() / 100;
        Utils::Log(1, "AIM_DIS: %f", AIM_Dis);
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
            if (Config.AimAndShot && Player.IsVisible && TargetDistance <= 30 && GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType == WeaponType::SG && BulletNumber > 0)
            {
                TriggerMouseLeft();
            }
            /**  if (AimAndShot && GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType == WeaponType::SG && BulletNumber > 0) {
                  TriggerMouseLeft();

              }*/
            AimAndShot = false;
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
                    //Utils::Log(1, "InputAxisPropertiesCount: %d", InputAxisPropertiesCount);
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
                        //
                        //// 标记是否有可见的目标骨骼
                        bool isAnyBoneVisible = false;

                        // 遍历所有目标骨骼
                        for (auto boneIndex : SkeletonLists::AimBones)
                        {
                            // 检查从相机位置到当前骨骼位置是否有视线
                            if (LineTrace::LineTraceSingle(GameData.Camera.Location, Player.Skeleton.LocationBones[boneIndex]))
                            {
                                // 如果可见，更新目标骨骼索引
                                AimBone = boneIndex;
                                // 更新后坐力位置
                                LineTraceSingleRecoilLocation = Player.Skeleton.LocationBones[AimBone];
                                // 重置后坐力时间
                                LineTraceSingleRecoilTimeStartTime = 0;
                                // 不再需要进行可见性检查
                                Config.VisibleCheck = false;
                                // 标记玩家可见
                                Player.IsVisible = true;
                                // 标记找到了可见的骨骼
                                isAnyBoneVisible = true;
                                // 跳出循环
                                break;
                            }
                        }
                        // 如果没有找到可见的骨骼
                        if (!isAnyBoneVisible)
                        {
                            // 需要进行可见性检查
                            Config.VisibleCheck = true;
                            // 标记玩家不可见
                            Player.IsVisible = false;
                            // 清除预测位置
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

                    //int visibleBone = -1;
                    //for (auto& Bone : SkeletonLists::AimBones) {

                    //    Player.Skeleton.BonesVisiablity[Bone] = LineTrace::LineTraceSingle(GameData.Camera.Location, Player.Skeleton.LocationBones[Bone]);
                    //    if (Player.Skeleton.BonesVisiablity[Bone]) {
                    //        visibleBone = Bone;
                    //        break;
                    //    }
                    //}

                    //EBoneIndex VisibilityBoneIndex;

                    //if (LineTraceHook::GetLocation(Player, visibleBone, &VisibilityBoneIndex))
                    //{
                    //    //Utils::Log(1, "LineTraceSingleRecoilLocation");
                    //    AimBone = VisibilityBoneIndex;
                    //    Player = Data::GetPlayersItem(GameData.AimBot.Target);
                    //    LineTraceSingleRecoilLocation = Player.Skeleton.LocationBones[AimBone];
                    //    LineTraceSingleRecoilTimeStartTime = 0;
                    //    Config.VisibleCheck = false;
                    //    Player.IsVisible = true;
                    //}
                    //else if (LineTraceSingleRecoilLocation == FVector(0.f, 0.f, 0.f)) {
                    //    Config.VisibleCheck = true;
                    //    Player.IsVisible = false;
                    //    GameData.AimBot.PredictedPos = FVector();
                    //}

                    

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
                bool isUsingNewPredPos_410 = Config.IsUsingNewPred_410;
                float Aimyy = Config.AimPre_Y;
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
                
                //float Aimxx1 = GetDragzuoyou(TargetDistance, GameData.LocalPlayerInfo.WeaponEntityInfo.Name);
                //const float DistanceToTarget = GunLocation.Distance(TargetPos) / 100.0f;
               // TravelTime = DistanceToTarget / CurrentWeaponData.TrajectoryConfigs.InitialSpeed * (Aimxx1 + Aimxx);//子弹飞行时间
                if (isUsingNewPredPos_410) {
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
                    Utils::Log(1, "AIM_H: %f", AIM_H);
                    if (AIM_H > 300 || AIM_H < -300)
                    {
                        drop = drop * (1 + TargetDistance / 1000 / 20);
                    }
                    //if (Config.IsUsingNew1120_BulletDrop) {
                    //drop = drop * Aimxx;//11.20 更新了子弹下坠预判的算法 修改X值实现子弹下坠预判更新
                    //}
                    Utils::Log(1, "New410_BulletDrop： %d", drop);


                    Vector3 actorPrePos = GetPrediction(TargetPos_410, TargetDistance, Velocity, InitialSpeed, 9.72f, Weap_IsJu, drop, false);
                    //actorPrePos.x = actorPrePos.x * Aimxx;
                    //actorPrePos.y = actorPrePos.y * Aimyy;
                    //actorPrePos.z = actorPrePos.z * Aimzz;
                    PredictedPos = FVector(actorPrePos.x, actorPrePos.y, actorPrePos.z);

                }
                else {
                    //老算法
                    

                     float Aimxx1 = GetDragzuoyou(TargetDistance, GameData.LocalPlayerInfo.WeaponEntityInfo.Name);
                     const float DistanceToTarget = GunLocation.Distance(TargetPos) / 100.0f;
                     TravelTime = DistanceToTarget / CurrentWeaponData.TrajectoryConfigs.InitialSpeed * (Aimxx1 + Aimxx);//子弹飞行时间
                     FVector PredictedPos = FVector(TargetPos.X, TargetPos.Y, TargetPos.Z + Abs(BulletDrop)) + TargetVelocity * (TravelTime / 0.885f);
                        
                    
                }
                //FVector PredictedPos = FVector(TargetPos.X, TargetPos.Y, TargetPos.Z + Abs(BulletDrop)) + TargetVelocity * (TravelTime / 0.885f);
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
 /*                       if (GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType == WeaponType::SG) {
                            
                            if (Config.AimAndShot) {

                                AimBotAPI(MoveXY, Config);
                            
                            }

                        }*/
                        
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

            //std::cout << timer.get() << std::endl;
            /*std::cout << time.get() << std::endl;
            Throttlered.executeTaskWithSleep("AimBotSleep", std::chrono::milliseconds(static_cast<int>(1000 / Config.FPS)), [] {});
            Utils::Log(1, "fps: %d %d", static_cast<int>(1000 / Config.FPS), Config.FPS);*/

        }
        mem.CloseScatterHandle(hScatter);
        mem.CloseScatterHandle(hWriteScatter);
    }
};