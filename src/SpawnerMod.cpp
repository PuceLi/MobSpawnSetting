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

    std::filesystem::path configPath = getSelf().getConfigDir() / "config.json";

    if (!ll::config::loadConfig(mConfig, configPath)) {
        logger.warn("无法从 {} 读取配置文件", configPath.string());
        logger.info("正在保存默认配置");

        if (!ll::config::saveConfig(mConfig, configPath)) {
            logger.error("无法保存默认配置到 {}", configPath.string());
            return false;
        }
    }

    logger.info("配置文件加载成功");
    return true;
}

bool SpawnerMod::enable() {
    auto& logger = getSelf().getLogger();
    logger.info("MobSpawnSetting 已启用");
    logger.info("  - 白名单模式: {}", mConfig.whitelistMode ? "启用" : "禁用");
    logger.info("  - 密度倍率: {:.2f}", mConfig.densityMultiplier);
    logger.info("  - 全局上限倍率: {:.2f}", mConfig.globalCapMultiplier);
    logger.info("  - 生成速度: {}x", mConfig.spawnSpeed);
    return true;
}

bool SpawnerMod::disable() {
    getSelf().getLogger().info("MobSpawnSetting 已禁用");
    return true;
}

} // namespace SpawnerSetting

LL_REGISTER_MOD(SpawnerSetting::SpawnerMod, SpawnerSetting::SpawnerMod::getInstance());