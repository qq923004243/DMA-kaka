#pragma once
#include <DMALibrary/Memory/Memory.h>
#include <Common/Data.h>
#include <Common/Entitys.h>
#include <Utils/Utils.h>
#include <Utils/Throttler.h>
#include <Hack/GNames.h>
#include <Hack/Decrypt.h>

namespace LineTraceHook
{



    bool GetLocation(const Player& TargetCharacter, const EBoneIndex& FirstBoneIndex, EBoneIndex* VisibilityBoneIndex)
    {
		const auto& locationBones = TargetCharacter.Skeleton.LocationBones;

		int firstIndexToCheck = static_cast<int>(FirstBoneIndex);

		if (locationBones.find(firstIndexToCheck) != locationBones.end())
		{
			FVector bonePos = locationBones.at(firstIndexToCheck);
			bool isVisible = LineTrace::LineTraceSingle(GameData.Camera.Location, bonePos);
			
			if (isVisible) {

				*VisibilityBoneIndex = FirstBoneIndex;
				return true;

			}
				
			
		}
		for (const auto& bone : locationBones)
		{
			
			int idx = bone.first;
			if (idx == 0)continue;
			FVector bonePos = bone.second;
			bool isVisible = LineTrace::LineTraceSingle(GameData.Camera.Location, bonePos);

			if (isVisible) {

				*VisibilityBoneIndex = static_cast<EBoneIndex>(idx);
				return true;

			}

		}

		return false;
	}
}
