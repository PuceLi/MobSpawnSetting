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
#include "mc/world/level/biome/Biome.h"

#include <string>
#include <regex>
#include <unordered_map>
#include <mutex>

namespace SpawnerSetting {

namespace {

std::mutex cacheMutex;
std::unordered_map<int, const DimensionConfig*> dimensionConfigCache;
std::unordered_map<std::string, const BiomeConfig*> biomeConfigCache;

const DimensionConfig* getDimensionConfig(int dimensionId) {
    auto& config = SpawnerMod::getInstance().getConfig();

    if (!config.enableDimensionConfig) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(cacheMutex);

    auto it = dimensionConfigCache.find(dimensionId);
    if (it != dimensionConfigCache.end()) {
        return it->second;
    }

    auto& dimConfigs = SpawnerMod::getInstance().getDimensionConfigs();
    for (const auto& dimConfig : dimConfigs.dimensions) {
        if (dimConfig.dimensionId == dimensionId && dimConfig.enabled) {
            dimensionConfigCache[dimensionId] = &dimConfig;
            return &dimConfig;
        }
    }

    dimensionConfigCache[dimensionId] = nullptr;
    return nullptr;
}

const BiomeConfig* getBiomeConfig(const std::string& biomeName) {
    auto& config = SpawnerMod::getInstance().getConfig();

    if (!config.enableBiomeConfig) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(cacheMutex);

    auto it = biomeConfigCache.find(biomeName);
    if (it != biomeConfigCache.end()) {
        return it->second;
    }

    auto& biomeConfigs = SpawnerMod::getInstance().getBiomeConfigs();
    for (const auto& biomeConfig : biomeConfigs.biomes) {
        if (biomeConfig.biomeName == biomeName && biomeConfig.enabled) {
            biomeConfigCache[biomeName] = &biomeConfig;
            return &biomeConfig;
        }
    }

    biomeConfigCache[biomeName] = nullptr;
    return nullptr;
}

const MobSpawnConfig* getMobConfig(const std::string& mobId, const std::string& biomeName, int) {
    auto& config = SpawnerMod::getInstance().getConfig();

    if (config.enableBiomeConfig && !biomeName.empty()) {
        auto biomeConfig = getBiomeConfig(biomeName);
        if (biomeConfig) {
            for (const auto& mobConfig : biomeConfig->mobConfigs) {
                if (mobConfig.identifier == mobId) {
                    return &mobConfig;
                }
            }
        }
    }

    if (config.enableMobConfig) {
        auto& mobConfigs = SpawnerMod::getInstance().getMobConfigs();
        for (const auto& mobConfig : mobConfigs.mobs) {
            if (mobConfig.identifier == mobId) {
                return &mobConfig;
            }
        }
    }

    return nullptr;
}

void applyDensityMultiplier(Dimension* dim) {
    auto& config = SpawnerMod::getInstance().getConfig();
    auto& logger = SpawnerMod::getInstance().getSelf().getLogger();

    int dimensionId = (int)dim->getDimensionId();
    float multiplier = config.densityMultiplier;

    auto dimConfig = getDimensionConfig(dimensionId);
    if (dimConfig) {
        multiplier = dimConfig->densityMultiplier;
    }

    if (multiplier == 1.0f) return;

    float originalVal = dim->mMobsPerChunkSurface[0];

    for (float& val : dim->mMobsPerChunkSurface) {
        val *= multiplier;
    }
    for (float& val : dim->mMobsPerChunkUnderground) {
        val *= multiplier;
    }

    logger.info("维度 ID: {} | 密度倍率: {:.1f} | 地表密度上限: {:.1f} -> {:.1f}",
        dimensionId, multiplier, originalVal, dim->mMobsPerChunkSurface[0]);
}

}

void SpawnerMod::clearCache() {
    std::lock_guard<std::mutex> lock(cacheMutex);
    dimensionConfigCache.clear();
    biomeConfigCache.clear();
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

    std::string myId = (std::string const&)this->getActorIdentifier().mFullName;

    int dimensionId = (int)this->getDimensionId();
    auto& blockSource = this->getDimensionBlockSource();
    auto biome = blockSource.tryGetBiome(this->getPosition());
    std::string biomeName;
    if (biome) {
        biomeName = biome->mHash->getString();
    }

    auto mobConfig = getMobConfig(myId, biomeName, dimensionId);

    if (mobConfig && !mobConfig->enabled) {
        return false;
    }

    if (config.enableFamilyFilter) {
        for (const auto& familyName : config.targetFamilies) {
            if (this->hasFamily(HashedString(familyName.c_str()))) {
                isFamilyMatch = true;
                break;
            }
        }
    }

    if (config.enableIdentifierFilter) {
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

    int dimensionId = (int)region.getDimensionId();

    float multiplier = config.globalCapMultiplier;
    int speed = config.spawnSpeed;

    auto dimConfig = getDimensionConfig(dimensionId);
    if (dimConfig) {
        multiplier = dimConfig->globalCapMultiplier;
        speed = dimConfig->spawnSpeed;
    }

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

} // namespace SpawnerSetting
