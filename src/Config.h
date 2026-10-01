#pragma once

#include "ll/api/Config.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace SpawnerSetting {

// 单个生物的生成配置
struct MobSpawnConfig {
    std::string identifier;              		// 生物 ID
    bool enabled = true;                 		// 是否允许生成
    float spawnProbabilityMultiplier = 1.0f; 	// 生成概率倍率
    float densityMultiplier = 1.0f;      		// 密度倍率
};

// 维度配置
struct DimensionConfig {
    int dimensionId;                     		// 维度 ID
    bool enabled = true;                 		// 是否启用配置
    float densityMultiplier = 1.0f;      		// 维度密度倍率
    float globalCapMultiplier = 1.0f;    		// 维度全局上限倍率
    int spawnSpeed = 1;                  		// 维度生成速度倍率
};

// 生物群系配置
struct BiomeConfig {
    std::string biomeName;               		// 生物群系名称
    bool enabled = true;                 		// 是否启用配置
    float densityMultiplier = 1.0f;      		// 生物群系密度倍率
    float spawnProbabilityMultiplier = 1.0f;  	// 生成概率倍率
    std::vector<MobSpawnConfig> mobConfigs;   	// 该生物群系的生物配置
};

// 主配置文件
struct Config {
    int version = 12;

    bool whitelistMode = false;
    bool enableFamilyFilter = false;
    std::vector<std::string> targetFamilies;
    bool enableIdentifierFilter = false;
    std::vector<std::string> targetMonsterIds;
    bool useRegex = false;

    float densityMultiplier = 1.0f;
    float globalCapMultiplier = 1.0f;
    int spawnSpeed = 1;
    int minGroupSize = -1;               // 全局最小群体大小（-1 使用默认值）
    int maxGroupSize = -1;               // 全局最大群体大小（-1 使用默认值）

    bool enableDimensionConfig = false;  // 是否启用独立维度配置
    bool enableMobConfig = false;        // 是否启用独立生物配置
    bool enableBiomeConfig = false;      // 是否启用独立生物群系配置
};

struct DimensionConfigs {
    int version = 1;
    std::vector<DimensionConfig> dimensions;
};

struct MobConfigs {
    int version = 1;
    std::vector<MobSpawnConfig> mobs;
};

struct BiomeConfigs {
    int version = 1;
    std::vector<BiomeConfig> biomes;
};

} // namespace SpawnerSetting