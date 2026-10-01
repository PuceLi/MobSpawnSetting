#include "SpawnerMod.h"

#include "ll/api/Config.h"
#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/command/ServerCommandRegisterEvent.h"

#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/server/commands/CommandPermissionLevel.h"

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

    using namespace ll::event;
    auto& bus = EventBus::getInstance();

    bus.emplaceListener<command::ServerCommandRegisterEvent>([this](command::ServerCommandRegisterEvent&) {
        auto& command = ll::command::CommandRegistrar::getInstance(false)
                            .getOrCreateCommand("mobspawn", "生物生成控制", CommandPermissionLevel::GameDirectors);

        command.overload().text("reload").execute([this](CommandOrigin const&, CommandOutput& output) {
            if (reloadConfig()) {
                output.success("§a配置文件已重载");
            } else {
                output.error("§c重新加载配置文件失败 请查看控制台日志");
            }
        });

        command.overload().text("info").execute([this](CommandOrigin const&, CommandOutput& output) {
            output.success("§eMobSpawnSetting 配置详情");

            output.success("§6[全局设置]");
            output.success("  白名单模式: §f{}", mConfig.whitelistMode ? "§a启用" : "§7禁用");
            output.success("  家族过滤: §f{}", mConfig.enableFamilyFilter ? "§a启用" : "§7禁用");
            if (mConfig.enableFamilyFilter && !mConfig.targetFamilies.empty()) {
                output.success("    目标家族: §f{}",
                    [&]() {
                        std::string result;
                        for (size_t i = 0; i < mConfig.targetFamilies.size(); ++i) {
                            if (i > 0) result += ", ";
                            result += mConfig.targetFamilies[i];
                        }
                        return result;
                    }());
            }
            output.success("  ID过滤: §f{}", mConfig.enableIdentifierFilter ? "§a启用" : "§7禁用");
            if (mConfig.enableIdentifierFilter && !mConfig.targetMonsterIds.empty()) {
                output.success("    目标ID: §f{}",
                    [&]() {
                        std::string result;
                        for (size_t i = 0; i < mConfig.targetMonsterIds.size(); ++i) {
                            if (i > 0) result += ", ";
                            result += mConfig.targetMonsterIds[i];
                        }
                        return result;
                    }());
                output.success("    正则表达式: §f{}", mConfig.useRegex ? "§a启用" : "§7禁用");
            }
            output.success("  密度倍率: §f{:.2f}", mConfig.densityMultiplier);
            output.success("  全局上限倍率: §f{:.2f}", mConfig.globalCapMultiplier);
            output.success("  生成速度: §f{}x", mConfig.spawnSpeed);

            output.success("§6[功能状态]");
            output.success("  独立维度配置: §f{}", mConfig.enableDimensionConfig ? "§a启用" : "§7禁用");
            output.success("  独立生物配置: §f{}", mConfig.enableMobConfig ? "§a启用" : "§7禁用");
            output.success("  独立生物群系配置: §f{}", mConfig.enableBiomeConfig ? "§a启用" : "§7禁用");

            if (mConfig.enableDimensionConfig) {
                output.success("§6[维度配置]");
                if (mDimensionConfigs.dimensions.empty()) {
                    output.success("  §7未配置任何维度");
                } else {
                    output.success("  §7共 {} 个维度配置", mDimensionConfigs.dimensions.size());
                    for (const auto& dimConfig : mDimensionConfigs.dimensions) {
                        std::string dimName = dimConfig.dimensionId == 0 ? "主世界"
                                            : dimConfig.dimensionId == 1 ? "下界"
                                            : dimConfig.dimensionId == 2 ? "末地"
                                                                         : "自定义维度";
                        output.success("  §e{} §7(ID: §f{}) - {}",
                            dimName,
                            dimConfig.dimensionId,
                            dimConfig.enabled ? "§a启用" : "§c禁用");
                        if (dimConfig.enabled) {
                            output.success("    密度倍率: §f{:.2f}", dimConfig.densityMultiplier);
                            output.success("    上限倍率: §f{:.2f}", dimConfig.globalCapMultiplier);
                            output.success("    生成速度: §f{}x", dimConfig.spawnSpeed);
                        }
                    }
                }
            }

            if (mConfig.enableBiomeConfig) {
                output.success("§6[生物群系配置]");
                if (mBiomeConfigs.biomes.empty()) {
                    output.success("  §7未配置任何生物群系");
                } else {
                    output.success("  §7共 {} 个生物群系配置", mBiomeConfigs.biomes.size());
                    for (const auto& biomeConfig : mBiomeConfigs.biomes) {
                        output.success("  §e{} §7- {}",
                            biomeConfig.biomeName,
                            biomeConfig.enabled ? "§a启用" : "§c禁用");
                        if (biomeConfig.enabled) {
                            output.success("    密度倍率: §f{:.2f}", biomeConfig.densityMultiplier);
                            output.success("    概率倍率: §f{:.2f}", biomeConfig.spawnProbabilityMultiplier);
                            if (!biomeConfig.mobConfigs.empty()) {
                                output.success("    特定生物配置: §f{} 个", biomeConfig.mobConfigs.size());
                                for (const auto& mobConfig : biomeConfig.mobConfigs) {
                                    output.success("      §b{} §7- {}",
                                        mobConfig.identifier,
                                        mobConfig.enabled ? "§a启用" : "§c禁用");
                                }
                            }
                        }
                    }
                }
            }

            if (mConfig.enableMobConfig) {
                output.success("§6[生物配置]");
                if (mMobConfigs.mobs.empty()) {
                    output.success("  §7未配置任何生物");
                } else {
                    output.success("  §7共 {} 个生物配置", mMobConfigs.mobs.size());
                    for (const auto& mobConfig : mMobConfigs.mobs) {
                        output.success("  §e{} §7- {}",
                            mobConfig.identifier,
                            mobConfig.enabled ? "§a启用" : "§c禁用");
                        if (mobConfig.enabled) {
                            output.success("    概率倍率: §f{:.2f}", mobConfig.spawnProbabilityMultiplier);
                            output.success("    密度倍率: §f{:.2f}", mobConfig.densityMultiplier);
                            if (mobConfig.minGroupSize >= 0 && mobConfig.maxGroupSize >= 0) {
                                output.success("    群体大小: §f{} - {}",
                                    mobConfig.minGroupSize, mobConfig.maxGroupSize);
                            }
                        }
                    }
                }
            }
        });
    });

    return true;
}

bool SpawnerMod::disable() {
    getSelf().getLogger().info("MobSpawnSetting 已禁用");
    return true;
}

bool SpawnerMod::reloadConfig() {
    auto& logger = getSelf().getLogger();
    logger.info("正在重新加载配置文件...");

    std::filesystem::path configDir = getSelf().getConfigDir();

    Config newConfig;
    if (!ll::config::loadConfig(newConfig, configDir / "config.json")) {
        logger.error("重新加载主配置文件失败");
        return false;
    }
    mConfig = newConfig;

    DimensionConfigs newDimConfigs;
    if (ll::config::loadConfig(newDimConfigs, configDir / "dimensions.json")) {
        mDimensionConfigs = newDimConfigs;
        logger.info("维度配置已重新加载");
    } else {
        logger.warn("重新加载维度配置失败");
    }

    MobConfigs newMobConfigs;
    if (ll::config::loadConfig(newMobConfigs, configDir / "mobs.json")) {
        mMobConfigs = newMobConfigs;
        logger.info("生物配置已重新加载");
    } else {
        logger.warn("重新加载生物配置失败");
    }

    BiomeConfigs newBiomeConfigs;
    if (ll::config::loadConfig(newBiomeConfigs, configDir / "biomes.json")) {
        mBiomeConfigs = newBiomeConfigs;
        logger.info("生物群系配置已重新加载");
    } else {
        logger.warn("重新加载生物群系配置失败");
    }

    logger.info("配置文件重新加载完成 维度密度配置需要重启服务器或重新加载维度才能完全生效");

    return true;
}

} // namespace SpawnerSetting

LL_REGISTER_MOD(SpawnerSetting::SpawnerMod, SpawnerSetting::SpawnerMod::getInstance());
