#include "SpawnerMod.h"

#include "ll/api/memory/Hook.h"
#include "ll/api/io/Logger.h"

#include "mc/world/actor/Mob.h"
#include "mc/world/level/dimension/Dimension.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/BedrockSpawner.h"
#include "mc/deps/core/string/HashedString.h"
#include "mc/world/actor/ActorDefinitionIdentifier.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/ChunkPos.h"
#include "mc/world/level/chunk/LevelChunkVolumeData.h"

#include <string>
#include <regex>

namespace SpawnerSetting {

namespace {

void applyDensityMultiplier(Dimension* dim) {
    auto& config = SpawnerMod::getInstance().getConfig();
    float multiplier = config.densityMultiplier;
    auto& logger = SpawnerMod::getInstance().getSelf().getLogger();

    if (multiplier == 1.0f) return;

    float originalVal = dim->mMobsPerChunkSurface[0];

    for (float& val : dim->mMobsPerChunkSurface) {
        val *= multiplier;
    }
    for (float& val : dim->mMobsPerChunkUnderground) {
        val *= multiplier;
    }

    logger.info("维度 ID: {} | 密度倍率: {:.1f} | 地表密度上限: {:.1f} -> {:.1f}",
        (int)dim->getDimensionId(), multiplier, originalVal, dim->mMobsPerChunkSurface[0]);
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    DimensionInitHook,
    ll::memory::HookPriority::Normal,
    Dimension,
    &Dimension::$init,
    void,
    ::br::worldgen::StructureSetRegistry const& structureSetRegistry
) {
    origin(structureSetRegistry);
    applyDensityMultiplier(this);
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    CheckSpawnRulesHook,
    ll::memory::HookPriority::Normal,
    Mob,
    &Mob::$checkSpawnRules,
    bool,
    bool fromSpawner
) {
    const auto& config = SpawnerMod::getInstance().getConfig();

    bool isFamilyMatch = false;
    bool isIdMatch = false;

    if (config.enableFamilyFilter) {
        for (const auto& familyName : config.targetFamilies) {
            if (this->hasFamily(HashedString(familyName.c_str()))) {
                isFamilyMatch = true;
                break;
            }
        }
    }

    if (config.enableIdentifierFilter) {
        std::string myId = (std::string const&)this->getActorIdentifier().mFullName;
        for (const auto& targetId : config.targetMonsterIds) {
            if (config.useRegex) {
                try {
                    std::regex pattern(targetId);
                    if (std::regex_search(myId, pattern)) {
                        isIdMatch = true;
                        break;
                    }
                } catch (...) {
                }
            } else {
                if (myId == targetId) {
                    isIdMatch = true;
                    break;
                }
            }
        }
    }

    bool isTarget = isFamilyMatch || isIdMatch;

    if (config.whitelistMode) {
        if (!isTarget) return false;
    } else {
        if (isTarget) return false;
    }

    return origin(fromSpawner);
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    BedrockSpawnerTickHook,
    ll::memory::HookPriority::Normal,
    BedrockSpawner,
    &BedrockSpawner::$tick,
    void,
    ::BlockSource& region,
    ::LevelChunkVolumeData const& levelChunkVolumeData,
    ::ChunkPos const chunkPos
) {
    auto& config = SpawnerMod::getInstance().getConfig();
    float multiplier = config.globalCapMultiplier;
    int speed = config.spawnSpeed;
    if (speed < 1) speed = 1;

    unsigned int currentRealCount = this->mTotalEntityCount;

    for (int i = 0; i < speed; ++i) {
        if (multiplier <= 0.0f || multiplier == 1.0f) {
            origin(region, levelChunkVolumeData, chunkPos);
            continue;
        }

        unsigned int fakeCount = static_cast<unsigned int>(currentRealCount / multiplier);

        if (fakeCount >= 200) {
            if (i == 0) {
                this->mTotalEntityCount = currentRealCount;
                origin(region, levelChunkVolumeData, chunkPos);
                currentRealCount = this->mTotalEntityCount;
            }
            break;
        }

        this->mTotalEntityCount = fakeCount;
        origin(region, levelChunkVolumeData, chunkPos);

        unsigned int newCount = this->mTotalEntityCount;
        int delta = (int)newCount - (int)fakeCount;
        currentRealCount += delta;

        this->mTotalEntityCount = currentRealCount;
    }
}

}
} // namespace SpawnerSetting