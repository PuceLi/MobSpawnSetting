#include "SpawnerMod.h"

#include "ll/api/Config.h"
#include "ll/api/mod/RegisterHelper.h"

#include <filesystem>

namespace SpawnerSetting {

SpawnerMod& SpawnerMod::getInstance() {
    static SpawnerMod instance;
    return instance;
}

bool SpawnerMod::load() {
    auto& logger = getSelf().getLogger();
    logger.info("加载 MobSpawnSetting 中...");
    logger.info("Author: PuceLi");

    std::filesystem::path configDir = getSelf().getConfigDir();

    std::filesystem::path mainConfigPath = configDir / "config.json";
    if (!ll::config::loadConfig(mConfig, mainConfigPath)) {
        logger.info("生成主配置文件...");
        if (!ll::config::saveConfig(mConfig, mainConfigPath)) {
            logger.error("无法保存主配置文件");
            return false;
        }
    }

    std::filesystem::path dimConfigPath = configDir / "dimensions.json";
    if (!ll::config::loadConfig(mDimensionConfigs, dimConfigPath)) {
        logger.info("生成维度配置文件...");
        mDimensionConfigs.dimensions = {};
        if (!ll::config::saveConfig(mDimensionConfigs, dimConfigPath)) {
            logger.error("无法保存维度配置文件");
        }
    }

    std::filesystem::path mobConfigPath = configDir / "mobs.json";
    if (!ll::config::loadConfig(mMobConfigs, mobConfigPath)) {
        logger.info("生成默认生物配置文件...");
        mMobConfigs.mobs = {};
        if (!ll::config::saveConfig(mMobConfigs, mobConfigPath)) {
            logger.error("无法保存生物配置文件");
        }
    }

    std::filesystem::path biomeConfigPath = configDir / "biomes.json";
    if (!ll::config::loadConfig(mBiomeConfigs, biomeConfigPath)) {
        logger.info("生成生物群系配置文件...");
        mBiomeConfigs.biomes = {};
        if (!ll::config::saveConfig(mBiomeConfigs, biomeConfigPath)) {
            logger.error("无法保存生物群系配置文件");
        }
    }

    logger.info("配置文件加载完成");
    return true;
}

bool SpawnerMod::enable() {
    auto& logger = getSelf().getLogger();
    logger.info("MobSpawnSetting 已启用");

    clearCache();

    return true;
}

bool SpawnerMod::disable() {
    auto& logger = getSelf().getLogger();
    logger.info("MobSpawnSetting 禁用中...");

    clearCache();

    logger.info("MobSpawnSetting 已禁用");
    return true;
}

bool SpawnerMod::unload() {
    auto& logger = getSelf().getLogger();
    logger.info("MobSpawnSetting 卸载中...");

    clearCache();

    logger.info("MobSpawnSetting 已卸载");
    return true;
}

} // namespace SpawnerSetting

LL_REGISTER_MOD(SpawnerSetting::SpawnerMod, SpawnerSetting::SpawnerMod::getInstance());
